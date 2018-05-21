include_directories(
    "${MANAGER_DIR}/include"
    "${GLOG_DIR}/include" 
    "${GTEST_DIR}/include" )

link_directories(
    "${MANAGER_DIR}/lib"
	  "${GLOG_DIR}/lib"
	  "${GTEST_DIR}/lib")

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)
add_executable(test_app ${SRC_LIST})

target_link_libraries(test_app
    blaze
    glog
    gtest
    ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(tasks)

add_test(blaze_test test_app -v 3 -stderrthreshold=3)
