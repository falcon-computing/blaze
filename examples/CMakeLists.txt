include_directories(
    ${ManagerIncludePath}
    ${ProtobufIncludePath})

function(compile_task proj_name)
  add_library(${proj_name} SHARED ${proj_name}.cpp)
  add_dependencies(${proj_name} Google KSight)
  target_link_libraries(${proj_name}
    blaze
    blaze_message)
  install(TARGETS ${proj_name}
    LIBRARY DESTINATION examples)
endfunction(compile_task)

function(compile_xlnx_task proj_name)
  if(DEFINED ENV{XILINX_SDX})
    set(XILINX_SDX $ENV{XILINX_SDX})
    include_directories(
        ${PlatformXilinxIncludePath}
        ${XILINX_SDX}/runtime/include/1_2)
    link_directories(
        ${XILINX_SDX}/runtime/lib/x86_64)

    compile_task(${proj_name})

    target_link_libraries(${proj_name} xilinxopencl)
  else()
    message(STATUS "skip examples/${proj_name} which depends on Xilinx")
  endif()
endfunction(compile_xlnx_task)

add_subdirectory(arrayTest)
add_subdirectory(bwa-sw)
add_subdirectory(pi)
add_subdirectory(pairhmm)
