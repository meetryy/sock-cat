cmake_minimum_required(VERSION 3.0.0)
project(socketmsg VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

get_filename_component(PARENT_DIR ../ ABSOLUTE)
include_directories(${PARENT_DIR}/common/inc)
link_directories(${PARENT_DIR}/common/src)

include_directories(${SOURCE_DIR}/inc)
link_directories(${SOURCE_DIR}/src)

include_directories(${PARENT_DIR}/common/ftxui/include)
link_directories(${SOURCE_DIR}/common/ftxui/src)

# file (GLOB SOURCES "*.cpp")
add_executable(server 
src/main.cpp 
src/db.cpp 
src/gui.cpp
sqlite3/sqlite3.c 
${PARENT_DIR}/common/src/packet.cpp
${PARENT_DIR}/common/src/log.cpp
${PARENT_DIR}/common/src/uuid.cpp
${PARENT_DIR}/common/src/socket.cpp
${PARENT_DIR}/common/src/sckcomm.cpp)

# add_subdirectory(TermOx/src)
# target_link_libraries(server TermOx)

target_link_libraries(server -lws2_32)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

