
include_directories(${CMAKE_SOURCE_DIR}/src/srv/algs/yolov8_infer/include)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/algs/yolov8_infer)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/algs/ai_alg)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/algs)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/alink/command/track)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/alink/command/target)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/eth_link/ptz_protocol)
include_directories(${CMAKE_SOURCE_DIR}/src/srv/log/)

aux_source_directory(. DIR_LIBS_SRCS)
aux_source_directory(./yolov8_infer DIR_LIBS_SRCS)
aux_source_directory(./ai_alg DIR_LIBS_SRCS)

add_library(algs ${DIR_LIBS_SRCS})
set(ALGS_LIBS algs PARENT_SCOPE)
