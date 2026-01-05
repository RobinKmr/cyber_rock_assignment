cmake_minimum_required(VERSION 3.16)
project(cyber_rock_assignment LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Boost ----
# find_package(Boost REQUIRED COMPONENTS system)
find_package(Boost REQUIRED COMPONENTS system)

# ---- Nanopb paths ----
set(NANOPB_DIR ${CMAKE_SOURCE_DIR}/libs/nanopb)
set(NANOPB_SRC
    ${NANOPB_DIR}/pb_encode.c
    ${NANOPB_DIR}/pb_decode.c
    ${NANOPB_DIR}/pb_common.c
)

# ---- Shared protobuf sources ----
set(PROTO_SRC
    ${CMAKE_SOURCE_DIR}/generated/proto/sensor.pb.c
)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${NANOPB_DIR}
    ${CMAKE_SOURCE_DIR}/generated/proto
)


# ---- Server ----
add_executable(server
    Server/server.cpp
    ${PROTO_SRC}
    ${NANOPB_SRC}
)

target_link_libraries(server
    Boost::system
)

# ---- Client ----
add_executable(client
    Client/client.cpp
    ${PROTO_SRC}
    ${NANOPB_SRC}
)

target_link_libraries(client
    Boost::system
)
