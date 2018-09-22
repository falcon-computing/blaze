aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)

set(include_prefix "blaze/xlnx_opencl")
set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/${include_prefix})

set(PlatformXilinxIncludePath ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL "Path to platform include files.")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${XILINX_SDX}/runtime/include/1_2)

link_directories(
    ${XILINX_SDX}/runtime/lib/x86_64)

add_library(xlnx_opencl SHARED ${SRC_LIST})
add_dependencies(xlnx_opencl 
    blaze
    blaze_message
    Google
    KSight)

target_link_libraries(xlnx_opencl
    xilinxopencl)

set_target_properties(xlnx_opencl PROPERTIES
    PUBLIC_HEADER "${include_dir}/OpenCLEnv.h;${include_dir}/OpenCLCommon.h")

install(TARGETS xlnx_opencl
    LIBRARY DESTINATION platforms
    PUBLIC_HEADER DESTINATION include/${include_prefix})
