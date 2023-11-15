#
#

add_subdirectory(socket)
add_subdirectory(eth_protocol)
add_subdirectory(ptz_protocol)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(eth_link ${DIR_LIBS_SRCS})

set(ETH_LINK_LIBS eth_link
	${SOCKET_LIBS}
	${ETH_PROCOTOL_LIBS}
	${PTZ_PROTOCOL_LIBS}
	PARENT_SCOPE)
