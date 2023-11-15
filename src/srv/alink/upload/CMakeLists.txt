#
#

aux_source_directory(. DIR_LIBS_SRCS)
add_library(upload ${DIR_LIBS_SRCS})
set(UPLOAD_LIBS upload PARENT_SCOPE)

