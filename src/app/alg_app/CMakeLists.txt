#
#
#aux_source_directory(. DIR_SCRS)

# include_directories(${CMAKE_SOURCE_DIR}/src/srv/algs/yolov8_infer/include) #cdx
# include_directories(${CMAKE_SOURCE_DIR}/src/srv/algs/yolov8_infer) #cdx

include_directories(${CMAKE_SOURCE_DIR}/src/srv/algs/ai_alg)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/eth_link/ptz_protocol)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/alink/command/track)

aux_source_directory(. DIR_LIB_SRCS)
# aux_source_directory(./yolov8_infer DIR_LIBS_SRCS)#cdx

add_library(alg_app ${DIR_LIB_SRCS})
set (ALG_APP_LIBS alg_app PARENT_SCOPE)


