#
#

add_subdirectory(file)

aux_source_directory(. DIR_LIBS_SRCS)
add_library(storage ${DIR_LIBS_SRCS})
set(STORAGE_LIBS storage
	${STORAGE_FILE_LIBS}
	PARENT_SCOPE)
