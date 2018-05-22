set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

cmake_minimum_required(VERSION 2.8.12)

project(blaze)

find_package(Boost 1.53.0 COMPONENTS
	  system thread iostreams filesystem regex REQUIRED)
find_package(Protobuf 2.5.0 REQUIRED)
find_package(Threads)

set(CMAKE_CXX_FLAGS "-std=c++0x -DBOOST_NO_CXX11_SCOPED_ENUMS")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -DNDEBUG")

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(GLOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../deps/glog-0.3.3")
set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../deps/googletest")
set(FLMDIR "${CMAKE_CURRENT_SOURCE_DIR}/../deps/falcon-lic")
set(MANAGER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${ProtobufIncludePath}
    ${MANAGER_DIR}/include
    ${GTEST_DIR}/include
    ${GLOG_DIR}/include)

link_directories(
	  ${GLOG_DIR}/lib) 

add_subdirectory(proto)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)

add_library(blaze STATIC ${SRC_LIST})
set_target_properties(blaze PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-fPIC")
target_link_libraries(blaze blaze_message)

add_executable(manager ${CMAKE_CURRENT_SOURCE_DIR}/src/executables/main.cpp)
target_link_libraries(manager
    blaze
    glog
    ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(admin ${CMAKE_CURRENT_SOURCE_DIR}/src/executables/admin.cpp)
target_link_libraries(admin
    blaze
    glog
    ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

enable_testing()
add_subdirectory(test)
