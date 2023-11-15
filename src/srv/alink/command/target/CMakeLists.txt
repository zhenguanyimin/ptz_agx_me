#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(target ${DIR_LIBS_SRCS})
set(TARGET_LIBS target PARENT_SCOPE)

