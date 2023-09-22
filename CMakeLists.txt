cmake_minimum_required(VERSION 3.10)
project(connectionpool)


set(CMAKE_BUILD_TYPE "Release")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
file(GLOB SRC_LIST "${PROJECT_SOURCE_DIR}/*.cc")

include_directories(${PROJECT_SOURCE_DIR})
#link_directories(${PROJECT_SOURCE_DIR})

add_executable(main ${SRC_LIST})

target_link_libraries(main mysqlclient)
target_precompile_headers(main PRIVATE pch.h)




