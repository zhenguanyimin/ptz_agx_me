
add_subdirectory(system)
add_subdirectory(track)
add_subdirectory(target)

set(COMMAND_LIBS 
	${SYSTEM_LIBS}
	${TRACK_LIBS}
	${TARGET_LIBS}
	PARENT_SCOPE)