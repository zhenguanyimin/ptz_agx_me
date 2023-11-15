#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(sysport_task ${DIR_LIB_SRCS})

set(SYSPORT_TASK_LIBS sysport_task PARENT_SCOPE)

