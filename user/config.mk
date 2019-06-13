###########################################################################
#
# Instructions:
# Add any extra object files to be compiled into your libraries as needed.
# If you do not create any extra .c or .S files, no changes are necessary.
#
###########################################################################

###########################################################################
# Object files for your thread library
###########################################################################
THREAD_OBJS = mutex.o cond.o sem.o rwlock.o thread.o malloc.o linked_list.o

###########################################################################
# Object files for your syscall wrappers
###########################################################################
SYSCALL_OBJS = syscall.o

###########################################################################
# Object files for your atomic library
###########################################################################
ATOMIC_OBJS = atomic.o

###########################################################################
# Test programs you have written which you wish to run
###########################################################################
# A list of the test programs you want compiled in from the user/progs
# directory
#
STUDENTTESTS =

###########################################################################
# Data files you provide to be included in the RAM disk
###########################################################################
# A list of the data files you want copied in from the user/files
# directory (see 410user/progs/cat.c)
#
STUDENTFILES =
