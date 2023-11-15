#
#
#aux_source_directory(. DIR_SCRS)

include_directories(${CMAKE_SOURCE_DIR}/src/srv/algs/ai_alg)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/eth_link/ptz_protocol)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/alink/command/track)

aux_source_directory(. DIR_LIB_SRCS)
add_library(ptz_app ${DIR_LIB_SRCS})
set (PTZ_APP_LIBS ptz_app PARENT_SCOPE)


