include_directories(
    ${ManagerIncludePath}
    ${ProtobufIncludePath})

function(compile_task proj_name)
  add_library(${proj_name} SHARED ${proj_name}.cpp)
  add_dependencies(${proj_name} Google KSight)
  target_link_libraries(${proj_name}
    blaze
    blaze_message)
#install(TARGETS ${proj_name}
#LIBRARY DESTINATION examples)
endfunction(compile_task)

function(compile_xlnx_task proj_name)
  if(XilinxOCL_FOUND)
    include_directories(
        ${PlatformXilinxIncludePath}
        ${XilinxOCL_INCLUDE_DIRS})
    link_directories(${XilinxOCL_LIBRARY_DIRS})

    compile_task(${proj_name})

    target_link_libraries(${proj_name} ${XilinxOCL_LIBRARIES})
  else()
    message(STATUS "skip examples/${proj_name} which depends on Xilinx")
  endif()
endfunction(compile_xlnx_task)

add_subdirectory(arrayTest)
add_subdirectory(pi)
