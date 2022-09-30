cmake_minimum_required(VERSION 3.0.0)
project(socketmsg VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

get_filename_component(PARENT_DIR ../ ABSOLUTE)
include_directories(${PARENT_DIR}/common/inc)
link_directories(${PARENT_DIR}/common/src)

add_executable(client 
main.cpp 
${PARENT_DIR}/common/src/packet.cpp 
${PARENT_DIR}/common/src/uuid.cpp 
${PARENT_DIR}/common/src/socket.cpp
${PARENT_DIR}/common/src/sckcomm.cpp
${PARENT_DIR}/common/src/log.cpp
)

target_link_libraries(client -lws2_32)
target_link_libraries(client -lMswsock)
target_link_libraries(client -lAdvApi32)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

