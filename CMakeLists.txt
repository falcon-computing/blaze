set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

cmake_minimum_required(VERSION 2.8.12)

project(blaze)

find_package(Boost 1.53.0 COMPONENTS
	  system thread iostreams filesystem regex REQUIRED)
find_package(Protobuf 2.5.0 REQUIRED)
find_package(Threads)

set(CMAKE_CXX_FLAGS "-std=c++0x -DBOOST_NO_CXX11_SCOPED_ENUMS -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(GLOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/glog-0.3.3")
set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/googletest")
set(FLMDIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/falcon-lic")
set(MANAGER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/manager")
set(PLATFORMs_DIR "${CMAKE_CURRENT_SOURCE_DIR}/platforms")

add_subdirectory(common)
add_subdirectory(manager)
add_subdirectory(platforms)

enable_testing()
