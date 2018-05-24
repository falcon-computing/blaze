set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

cmake_minimum_required(VERSION 2.8.12)

project(blaze-platforms)

find_package(Boost 1.53.0 COMPONENTS
	  system thread iostreams filesystem regex REQUIRED)

set(CMAKE_CXX_FLAGS "-std=c++0x -DBOOST_NO_CXX11_SCOPED_ENUMS")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")

set(GLOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../deps/glog-0.3.3")
set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../deps/googletest")
set(FLMDIR "${CMAKE_CURRENT_SOURCE_DIR}/../deps/falcon-lic")
set(MANAGER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../manager")

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${ProtobufIncludePath}
    ${MANAGER_DIR}/include
    ${GTEST_DIR}/include
    ${GLOG_DIR}/include)

# check xlnx platform
if(DEFINED ENV{XILINX_SDX})
  message(STATUS "XILINX_SDX is defined, enable xlnx_opencl")
  set(XILINX_SDX $ENV{XILINX_SDX})
  add_subdirectory(xlnx_opencl)
else()
  message(WARNING "skip xlnx_opencl compilation")
endif()
