#
#

add_subdirectory(task)
add_subdirectory(timer)
add_subdirectory(time)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(sysport ${DIR_LIBS_SRCS})
set(SYSPORT_LIBS sysport 
	${SYSPORT_TASK_LIBS}
	${SYSPORT_TIMER_LIBS}
	${SYSPORT_TIME_LIBS}
	PARENT_SCOPE)

