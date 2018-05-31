include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${ProtobufIncludePath}
    ${GTEST_DIR}/include
    ${GLOG_DIR}/include)

set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/blaze)

link_directories(
	  ${GLOG_DIR}/lib) 

#add_subdirectory(proto)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)

add_library(blaze STATIC ${SRC_LIST})
add_dependencies(blaze glog-download)
target_link_libraries(blaze blaze_message)
set_target_properties(blaze PROPERTIES
    PUBLIC_HEADER "${include_dir}/AccAgent.h;${include_dir}/Client.h;${include_dir}/Common.h;${include_dir}/Task.h;${include_dir}/TaskEnv.h")


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

# install options
install(TARGETS blaze
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/blaze)

install(TARGETS manager
    RUNTIME DESTINATION bin)

install(TARGETS admin
    RUNTIME DESTINATION bin)
