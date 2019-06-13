/*
###############################################################################################################
     ______        ______              ___         ____________     _________     _________     __     __
    |   __  `-    |   __  `-          /   \       |____    ____|   |   ______|   |  _______|   |  |  /  /
    |  |   \  \   |  |   \  \        /  ^  \           |  |        |  |          |  |          |  | /  /
    |  |__ /  /   |  |__ /  /       /  /_\  \          |  |        |  |____      |  |____      |  |/  /
    |   ___ ~`    |      _~`       /   ___   \         |  |        |   ____|     |   ____|     |     <
    |  |          |  |\  \        /  /     \  \        |  |        |  |          |  |          |  |\  \
    |  |          |  | \  \      /  /       \  \       |  |        |  |______    |  |______    |  | \  \
    |__|          |__|  \__\    /__/         \__\      |__|        |_________|   |_________|   |__|  \__\

###############################################################################################################
DESIGN STRATEGIES                                   thread.c                                   27th April 2017
###############################################################################################################

THREAD_T:   This is the structure tha stores information about a particular thread. It includes the following:

            *threadID   : ID of the thread used for differenciating each threads
            *status     : used in thr_join to update the status
            *baseAddress: address of the stack for that particular thread so that remove_pages can be done
            *exitFlag   : to check weather the thread has exited or not
            *joinedFlag : used to ensure that multiple threads annot join the same thread at the same time
            *stack_size : allocated memory for the stack of a particcular thread
            *threadCond : used to signal the joined thread about the current thread's status

THR_INIT:   This function is responsible for initializing the thread library. First I check for the argument
            stack_size is less than zero. An interesting aspect of the top stack address is that I declare a
            random variable and get the address of it to get the address of the stack which can be used later
            for determining the starting address of the stack of each thread. It is the best way to get the stack 
            address of the current function. The only disadvantage is that it is occupying 4 bytes extra on to 
            the stack. Aligning the top stack address and the stack size is crucial. The reason I put the data 
            of the root thread into my global list is that the root thread may call thread_join will call 
            thr_exit instead of return()'ing from main()). The base address of the root thread 
            is set to NULL because remove_pages() should not be done on the root thread later in thr_join. 

THR_CREATE: This function creates a new thread to run func(args). New_pages is called to allocate memory for
            the stack of the new thread. In order to make new_pages efficient i keep updating the top_stack address
            to the address of last created thread's stack. The only disagvantage with this method is, if thr_exit 
            is called in between, another thread will not be able to allocate stack memory on that spot again. 
            Thats a flaw. A better implementation is to create a linked list of free memory in order to utilize
            the memory of the previously threads' stack that has already been exited. I am doing this so that
            "largetest" fills up virtual address space quickly. 
            
			 * The memory address of the top of the stack is: 0xf1000f98  
			 * The memory address of the bottom of the heap is: 0x01006010
			 * I am giving the stack and the heap equal amounts of memory
			 * (0xf1000f98-0x01006010)/2 = 2013255622
			 * new pages fail if the address of the thread stack is less than (top_stack_address - 2013255622)
			 
            Some kind of book keeping is needed in order to keep track weather a thread is joined by using the 
            status from thr_exit. I use a doubly linked list in order to do so. Same global list used earlier. 
            The reason I use a doubly linked list is explained in the cond.c file. I malloc every thread_t struct
            (which goes into my list) instead of creating a local variable so that I have more control when i can
            free it from the memory instead of relying on the function return. It makes it easier in my opinion. 
            The disadvantage with this method is that it is using the heap space which is taking away memory from
            the user. 

THR_JOIN:   It finds the data regarding the thread from the linked list. The only way to do it is to loop through 
            the entire list which takes O(n) time. A better implementation would be using a red black tree which 
            takes log(n) time. I use a conditional variable for each thread. It is used to signal the joined 
            thread about the current thread's status. Remove pages is called here instead of thr_exit because once 
            thr_exit signals, it makes more sense to free any kind of memory related to that thread after vanish 
            is called in thr_exit. Here the global mutex becomes crucial. The global mutex is used to protect 
            multiple read and writes on the heap (linked list), multiple joins on the same thread, and multiple 
            exits on the same thread. This is also the reason why I pass in the global mutex into cond_wait to 
            make everything thread safe. The disadvantage of a global mutex is that it is not as efficient as 
            a mutex for each thread.

THR_EXIT:   This function signals the waiting thread and calls vanish(). No one else can schedule the thread once 
            vanish is called. In other words no one can use the stack. 
            
###############################################################################################################
*/
#include <thread.h>
#include "thr_internals.h"
#include <mutex.h>
#include <cond.h>
#include <syscall.h>
#include <syscall_ext.h>
#include <stdio.h>
#include <assert.h>

// Uncomment the following line to disable printf calls
#define _REMOVE_PRINTF_CALLS_
#ifdef _REMOVE_PRINTF_CALLS_
#define printf(...)
#endif // _REMOVE_PRINTF_CALLS_

//page_size
#define PAGE_SIZE_BYTES 4096
#define PAGE_SIZE_ADDRESS 0x1000

//global lists of threads
list_t list_of_threads;
//mutex to avoid multiple writes on the global list
mutex_t thread_list_mutex;

//stack size of each stack
unsigned int stack_size_global;
//address of the top of the stack
void* top_stack_address;
void* bottom_stack_address = (void *)0xf05a2000;


int thr_init(unsigned int stack_size)
{
    /* IMPLEMENT THIS */
    if(stack_size <=0)
    {
        return -1;
    }
    else
    {
        //initialize the global mutex for threads
        if (mutex_init(&thread_list_mutex) != 0)
        {
            return -1;
        }

        //variable for determining the address of the stack and initializing the global mutex
        int a;
        //printf("The memory address of stack top is: %p\n", (void*) &a);
        //initialize the global mutex for malloc
        if (mutex_init(&malloc_mutex)!= 0)
        {
            mutex_unlock(&thread_list_mutex);
            return -1;
        }
        
        //allocate memory for the data of the thread in the node. 
        thread_t* my_thread = malloc(sizeof(thread_t));
        //printf("The memory address of heap is: %p\n", my_thread);
        if(my_thread == NULL)
        {
            mutex_unlock(&thread_list_mutex);
            return -1;
        }
        //create the list
        list_create(&list_of_threads);
        
        top_stack_address = &a;
        //alligning the address size to the page size
        top_stack_address = (void *)(((((int)top_stack_address - 1)/PAGE_SIZE_BYTES)+1)*PAGE_SIZE_BYTES);
        //alligning the stack size to the page size
        stack_size = (((stack_size - 1)/PAGE_SIZE_BYTES)+1)*PAGE_SIZE_BYTES;
        stack_size_global = stack_size;
        
        /*initialize the elements in the tread_t struct*/
        
        //initializing the conditional variable
        if (cond_init(&my_thread->threadCond) != 0)
        {
            list_destroy(&list_of_threads);
            return -1;
        }
        
        int tid = gettid();
        my_thread->exitFlag = 0;
        my_thread->status = NULL;
        my_thread->threadID = tid;
        my_thread->joinedFlag = 0;
        my_thread->stack_size = stack_size;
        my_thread->baseAddress = NULL;
       
        //setting the page size

        /*insert the root thread into the list*/
        if ((list_insert(&list_of_threads, (void*)my_thread, tid))<0)
        {
        	list_destroy(&list_of_threads);
        	return -1;
        }
        return 0;
    }
}

int thr_create(void* (*func)(void*), void* arg)
{
    /* IMPLEMENT THIS */
    //STEP 1: provide memory for the stack 
    mutex_lock(&thread_list_mutex);

    if (func == NULL)
    {
        mutex_unlock(&thread_list_mutex);
        return -1;
    }

    //STEP 2: getting the memory for the new stack
    void* address = (void *) top_stack_address;
    while ((int)(new_pages(address, stack_size_global)<0))
    {
        address -= stack_size_global;
        
        if(address < (top_stack_address-2013255622))
        {
            mutex_unlock(&thread_list_mutex);            
            return -1;
        }
    }
    
    top_stack_address = address;
    
    //creating the storage for the data of the new thread;
    thread_t* child = malloc(sizeof(thread_t));
    if (child == NULL)
    {
        mutex_unlock(&thread_list_mutex);
    	return -1;
    }
    
        //set the other things of the thread_t
    if (cond_init(&child->threadCond) != 0)
    {
        free(child);
        child = NULL;
        //remove the thread that is currently occupying space since it failed
        remove_pages(address);
        mutex_unlock(&thread_list_mutex);
        return -1;
    }

    //STEP 3: create a new thread using thread_fork which creates a new child
    int id_child_thread_fork = thread_fork(func, arg, address + stack_size_global); 
    
    //if there is an error with thread fork
    if (id_child_thread_fork < 0)
    {
        cond_destroy(&child->threadCond);
        free(child);
        child = NULL;
        //remove all threads that are currently occupying space??
        remove_pages(address);
        mutex_unlock(&thread_list_mutex);
        return -1;
    }
    /*
     *STEP 4: book keeping
     *use a data structure
     *use the status to keep track weather a thread is joined
     *book keaping of the status
    */

    //initial values of the flags
    child->threadID = id_child_thread_fork;
    child->status = 0;
    child->baseAddress = address;
    child->stack_size = stack_size_global;
    child->exitFlag = 0;
    child->joinedFlag = 0;
    //insert the child thread_data into the list 
    
    //check if malloc fails 
    if ((list_insert(&list_of_threads, (void*)child, id_child_thread_fork))<0)
    {
    	cond_destroy(&child->threadCond);
		free(child);
		child = NULL;
		remove_pages(address);
		mutex_unlock(&thread_list_mutex);
		return -1;
    }
    
    mutex_unlock(&thread_list_mutex);
    
    return id_child_thread_fork;
}

int thr_join(int tid, void** status)
{
    /* IMPLEMENT THIS */
    mutex_lock(&thread_list_mutex);
    list_node_t* target_node;
    target_node = list_find(&list_of_threads, tid);
    if(target_node == NULL)
    {
        mutex_unlock(&thread_list_mutex);
        return -1;
    }
    thread_t* targetThread = (thread_t*)target_node->thread_data;

    if  (targetThread->joinedFlag == 1)
    {
        //unlock the mutex
        mutex_unlock(&thread_list_mutex);
        //return error
    	return -1;
    }
    else
    {
        //check only if it is not exited
		if(targetThread->exitFlag != 1)
		{
			targetThread->joinedFlag = 1;
			cond_wait(&targetThread->threadCond, &thread_list_mutex);
        }
        
        /*
         * make sure the thread is exited
         * cleans up the memory and updates your status
         * status you get from exit
         */

        //sets the status
        if(status!= NULL)
        {
            *status = targetThread->status;
        }
        //cleanup the flags and the conditional variable
        targetThread->joinedFlag = 0;
        targetThread->exitFlag = 1;
        targetThread->threadID = -1;
        cond_destroy(&targetThread->threadCond);

        //do not do remove page for the root thread
        if (targetThread->baseAddress != NULL)
        {
            //cleans the thread->free the memory (remove pages)
            if ((remove_pages(targetThread->baseAddress))<0)
            {
                mutex_unlock(&thread_list_mutex);
            	return -1;
            }
        }
        //remove from the list
        list_remove(&list_of_threads, target_node); 
        //free(targetThread);
        mutex_unlock(&thread_list_mutex);

    }
    return 0;
}

void thr_exit(void* status)
{
    /* IMPLEMENT THIS */
    
    //lock the global mutex 
    mutex_lock(&thread_list_mutex);
    //declaring variables to make things easier
    thread_t *targetThread;
    int tid = gettid();
    //find the node with the target thread
    list_node_t* target_node = list_find(&list_of_threads, tid);
    targetThread = (thread_t*)target_node->thread_data;
    //signal the thread and update the status
    targetThread->exitFlag = 1;
    targetThread->status = status;
    targetThread->joinedFlag = 0;
    cond_signal(&targetThread->threadCond);
    //unlock the global mutex
    mutex_unlock(&thread_list_mutex);
    //no one else can schedule the thread once the vanish is called->no one can use the stack
    vanish();
}

int thr_getid()
{
    /* IMPLEMENT THIS */
    return gettid();
}

int thr_yield(int tid)
{
    /* IMPLEMENT THIS */
    return yield(tid);
}
