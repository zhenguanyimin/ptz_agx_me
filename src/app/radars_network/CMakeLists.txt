#
#
#aux_source_directory(. DIR_SCRS)

aux_source_directory(. DIR_LIB_SRCS)
add_library(radar_network ${DIR_LIB_SRCS})
set (RADAR_NETWORK_LIBS radar_network PARENT_SCOPE)


