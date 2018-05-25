aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
		"${XILINX_SDX}/runtime/include/1_2")

link_directories(
    ${XILINX_SDX}/runtime/lib/x86_64)

add_library(xlnx_opencl SHARED ${SRC_LIST})
add_dependencies(xlnx_opencl 
    blaze)

target_link_libraries(xlnx_opencl
    xilinxopencl)
