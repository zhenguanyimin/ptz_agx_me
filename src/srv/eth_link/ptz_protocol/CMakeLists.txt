#
#

include_directories(${CMAKE_SOURCE_DIR}/src/common/tools/)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(ptz_control_protocol ${DIR_LIBS_SRCS})
set(PTZ_PROTOCOL_LIBS ptz_control_protocol PARENT_SCOPE)

