#include_directories(
#    "${GLOG_DIR}/include" 
#    "${GTEST_DIR}/include" )
#
link_directories(
	  ${GTEST_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS="-fprofile-arcs -ftest-coverage") 

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)
add_executable(test_app ${SRC_LIST})
add_dependencies(test_app Google KSight)

target_link_libraries(test_app
    blaze
    gcov
    ${Google_LIBRARIES}
    ${KSight_LIBRARIES}
    ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(tasks)

add_test(blaze_test test_app -v 3 -stderrthreshold=3)
