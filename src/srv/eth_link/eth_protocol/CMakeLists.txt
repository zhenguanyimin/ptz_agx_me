#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(eth_protocol ${DIR_LIBS_SRCS})
set(ETH_PROCOTOL_LIBS eth_protocol PARENT_SCOPE)

