

set(SOURCES
    sock.cpp
    CSockParcelCarrier.cpp
    CSockParcelSession.cpp
)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../Library/inc/elasys)

add_library(marshal_rpc STATIC ${SOURCES})

