###########################################################################
#
#    #####          #######         #######         ######            ###
#   #     #            #            #     #         #     #           ###
#   #                  #            #     #         #     #           ###
#    #####             #            #     #         ######             #
#         #            #            #     #         #
#   #     #            #            #     #         #                 ###
#    #####             #            #######         #                 ###
#
#
# Please read the directions in writeup and in this config.mk carefully.
# Do -N-O-T- just dump things randomly in here until your project builds.
# If you do that, you run an excellent chance of turning in something
# which can't be graded.  If you think the build infrastructure is
# somehow restricting you from doing something you need to do, contact
# the course staff--don't just hit it with a hammer and move on.
#
# [Once you've read this message, please edit it out of your config.mk!!]
###########################################################################



###########################################################################
# This is the include file for the make file.
###########################################################################
# You should have to edit only this file to get things to build.
#

###########################################################################
# DEBUG
###########################################################################
# You can set CONFIG_DEBUG to "user" if you want
# the DEBUG flag to be #define'd for user code
# (all of P2 is kernel code).  This will cause
# the checks in contracts.h to be made, at the
# expense of some performance.
#
# Use "make veryclean" if you adjust CONFIG_DEBUG.
#
CONFIG_DEBUG = user

###########################################################################
# WARNING: Do not put extraneous test programs into the REQPROGS variables.
#          Doing so will put your grader in a bad mood which is likely to
#          make him or her less forgiving for other issues.
###########################################################################

###########################################################################
# Mandatory programs whose source is provided by course staff
###########################################################################
# A list of the programs in 410user/progs which are provided in source
# form and NECESSARY FOR THE KERNEL TO RUN
#
# The idle process is a really good thing to keep here.
#
410REQPROGS = idle shell init

###########################################################################
# WARNING: When we test your code, the two TESTS variables below will be
# ignored.  Your kernel MUST RUN WITHOUT THEM.
###########################################################################

###########################################################################
# Test programs provided by course staff you wish to run
###########################################################################
# A list of the test programs you want compiled in from the 410user/progs
# directory
#
410TESTS = actual_wait agility_drill beady_test bg cat cvar_test cyclone \
excellent getpid_test1 halt_test join_specific_test juggle largetest \
mandelbrot misbehave misbehave_wrap multitest mutex_destroy_test nibbles \
paraguay racer rwlock_downgrade_read_test startle thr_exit_join \
atomic_test syscall_test thr_join_exit mutex_test broadcast_test \
paradise_lost semaphore_test agility_drill_sem largetest2

###########################################################################
# Data files provided by course staff to be included in the RAM disk
###########################################################################
# A list of the data files you want copied in from the 410user/files
# directory (see 410user/progs/cat.c)
#
410FILES =
