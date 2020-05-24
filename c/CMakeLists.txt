######################################################
#Configure flags
######################################################


SET(LIB_THREAD_STREAMROUTER_SW true)
SET(LIB_THREAD_ARCHITECTURE posix)
SET(LIB_TIMER_STREAMROUTER_SW true)
SET(LIB_TIMER_ARCHITECTURE posix)
SET(LIB_CLOCK_ARCHITECTURE posix)

######################################################
#Subdirectoris 
######################################################
add_subdirectory(lib_clock)
add_subdirectory(lib_convention)
add_subdirectory(lib_list)
add_subdirectory(lib_mini_printf)
add_subdirectory(lib_timer)
add_subdirectory(lib_thread)