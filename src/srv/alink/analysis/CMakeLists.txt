#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(analysis ${DIR_LIBS_SRCS})
set(ANALYSIS_LIBS analysis PARENT_SCOPE)

