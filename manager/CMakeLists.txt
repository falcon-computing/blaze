include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${ProtobufIncludePath}
    ${GTEST_DIR}/include
    ${GLOG_DIR}/include)

link_directories(
	  ${GLOG_DIR}/lib) 

#add_subdirectory(proto)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)

add_library(blaze STATIC ${SRC_LIST})
add_dependencies(blaze glog-download)

target_link_libraries(blaze blaze_message)

add_executable(manager ${CMAKE_CURRENT_SOURCE_DIR}/src/executables/main.cpp)
add_dependencies(manager glog-download)
add_dependencies(manager falcon-lic-download)

target_link_libraries(manager
    blaze
    glog
    ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(admin ${CMAKE_CURRENT_SOURCE_DIR}/src/executables/admin.cpp)
add_dependencies(admin glog-download)
target_link_libraries(admin
    blaze
    glog
    ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(test)
