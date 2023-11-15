#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(c2_network)
#add_subdirectory(DataPath)

aux_source_directory(. DIR_LIB_SRCS)
add_library(sysport_timer ${DIR_LIB_SRCS})

set(SYSPORT_TIMER_LIBS sysport_timer PARENT_SCOPE)

