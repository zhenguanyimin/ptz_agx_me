#
#
#aux_source_directory(. DIR_SCRS)

aux_source_directory(. DIR_LIB_SRCS)
add_library(skyfend_tools ${DIR_LIB_SRCS})

set(TOOLS_LIBS skyfend_tools PARENT_SCOPE)

