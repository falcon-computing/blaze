set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

cmake_minimum_required(VERSION 2.8.12)

project(blaze)

execute_process(
    COMMAND git describe --tags --always
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

set(BIN_VERSION "${GIT_VERSION}")
message(STATUS "version: ${BIN_VERSION}")

find_package(Boost 1.53.0 COMPONENTS
	  system thread iostreams filesystem regex REQUIRED)
find_package(Protobuf 2.5.0 REQUIRED)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Google REQUIRED)
find_package(FalconLM REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -DBOOST_NO_CXX11_SCOPED_ENUMS -fPIC -DVERSION=\\\"${BIN_VERSION}\\\"")
# current manager has issue with -O3, so default to O2 instead
set(CMAKE_CXX_FLAGS_RELEASE "-O1 -DNDEBUG -Wall -DUSELICENSE")

set(MANAGER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/manager")
set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common")
set(PLATFORMS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/platforms")

include_directories(
    ${Google_INCLUDE_DIRS}
    ${FalconLM_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS})

link_directories(
    ${Google_LIBRARY_DIRS}
    ${FalconLM_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS})

enable_testing()
add_subdirectory(common)
add_subdirectory(manager)
add_subdirectory(platforms)
add_subdirectory(examples)