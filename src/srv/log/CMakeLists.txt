#
#
#aux_source_directory(. DIR_SCRS)

#add_subdirectory(alink)


aux_source_directory(. DIR_LIB_SRCS)
add_library(log ${DIR_LIB_SRCS})
set(LOG_LIBS log PARENT_SCOPE)


