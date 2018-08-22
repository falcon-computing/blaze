if(DEFINED ENV{XILINX_SDX})
set(XILINX_SDX $ENV{XILINX_SDX})

include_directories(
    ${PlatformXilinxIncludePath}
    ${XILINX_SDX}/runtime/include/1_2)

link_directories(
    ${XILINX_SDX}/runtime/lib/x86_64)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

add_library(PairHMMTask SHARED ${SRC_LIST})
add_dependencies(PairHMMTask Google)
target_link_libraries(PairHMMTask
    blaze
    blaze_message
    xilinxopencl)

install(TARGETS PairHMMTask
    LIBRARY DESTINATION examples)

else()
  message(STATUS "skip examples/PairHMMTask which depends on Xilinx")
endif()
