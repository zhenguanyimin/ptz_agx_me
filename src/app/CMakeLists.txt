
add_subdirectory(c2_network)
add_subdirectory(radars_network)
add_subdirectory(alg_app)
add_subdirectory(ptz)

set(APP_LIBS 
    ${C2_NETWORK_LIBS}
    ${RADAR_NETWORK_LIBS}
    ${ALG_APP_LIBS}
    ${PTZ_APP_LIBS}
    PARENT_SCOPE)

