#
#
#aux_source_directory(. DIR_SCRS)

add_subdirectory(alink)
add_subdirectory(eth_link)
add_subdirectory(log)
add_subdirectory(algs)

#aux_source_directory(. DIR_LIB_SRCS)
#add_library(main ${DIR_LIB_SRCS})
set(SRV_LIBS 
    ${ALINK_LIBS}
    ${ETH_LINK_LIBS}
    ${LOG_LIBS}
    ${ALGS_LIBS}
    PARENT_SCOPE)
