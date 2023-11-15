#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(socket ${DIR_LIBS_SRCS})
set(SOCKET_LIBS socket PARENT_SCOPE)

