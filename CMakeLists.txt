cmake_minimum_required(VERSION 2.6)

project(knowledgePoint)

add_definitions(-std=c++11)
#add_definitions(-DAPI_EXPORTS)
option(CUDA_USE_STATIC_CUDA_RUNTIME OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

#设置可执行文件输出目录
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG D:/04Bin)
#设置库输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG D:/04Bin/lib)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Ofast -g -D_MWAITXINTRIN_H_INCLUDED")


include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/)


FILE(GLOB SRC_FILES "src/*.cpp")
FILE(GLOB INC_FILES "src/*.h")

add_executable(${PROJECT_NAME} ${INC_FILES}${SRC_FILES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

