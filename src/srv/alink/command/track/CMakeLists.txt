#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(track ${DIR_LIBS_SRCS})
set(TRACK_LIBS track PARENT_SCOPE)

