#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(app)
add_subdirectory(srv)
add_subdirectory(common)

aux_source_directory(. DIR_LIB_SRCS)
add_library(main ${DIR_LIB_SRCS})
set(SRC_LIBS main 
	${APP_LIBS} 
	${SRV_LIBS}
	${COMMON_LIBS}
	PARENT_SCOPE)

