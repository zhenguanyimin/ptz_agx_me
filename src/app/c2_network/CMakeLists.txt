#
#
#aux_source_directory(. DIR_SCRS)

aux_source_directory(. DIR_LIB_SRCS)
add_library(c2_network ${DIR_LIB_SRCS})
set (C2_NETWORK_LIBS c2_network PARENT_SCOPE)


