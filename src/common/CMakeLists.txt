#
#

add_subdirectory(sysport)
add_subdirectory(storage)
add_subdirectory(tools)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(common ${DIR_LIBS_SRCS})
set(COMMON_LIBS common 
	${SYSPORT_LIBS}
	${STORAGE_LIBS}
	${TOOLS_LIBS}
	PARENT_SCOPE)
