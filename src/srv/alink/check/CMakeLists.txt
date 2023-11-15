#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(check ${DIR_LIBS_SRCS})
set(CHECK_LIBS check PARENT_SCOPE)

