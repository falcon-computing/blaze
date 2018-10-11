aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)

set(include_prefix "blaze/altr_opencl")
set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/${include_prefix})

set(PlatformAlteraIncludePath ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL "Path to platform include files.")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${IntelAltera_INCLUDE_DIRS})

link_directories(
    ${IntelAltera_LIBRARY_DIRS})

add_library(altr_opencl SHARED ${SRC_LIST})
add_dependencies(altr_opencl 
    blaze
    blaze_message
    Google
    KSight)

target_link_libraries(altr_opencl
    ${IntelAltera_LIBRARIES})

set_target_properties(altr_opencl PROPERTIES
    PUBLIC_HEADER "${include_dir}/OpenCLEnv.h;${include_dir}/OpenCLCommon.h")

install(TARGETS altr_opencl
    LIBRARY DESTINATION platforms
    PUBLIC_HEADER DESTINATION include/${include_prefix})
