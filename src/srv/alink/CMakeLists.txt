#
#

add_subdirectory(analysis)
add_subdirectory(check)
add_subdirectory(command)
add_subdirectory(upload)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(alink ${DIR_LIBS_SRCS})
set(ALINK_LIBS alink 
	${ANALYSIS_LIBS}
	${CHECK_LIBS}
	${COMMAND_LIBS}
	${UPLOAD_LIBS}
	PARENT_SCOPE)

