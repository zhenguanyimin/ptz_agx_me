#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(system ${DIR_LIBS_SRCS})
set(SYSTEM_LIBS system PARENT_SCOPE)

