include_directories(
    ${ManagerIncludePath}
    ${ProtobufIncludePath})

# check xlnx platform
set(BUILD_PLATFORM_MODULE false)
if(XilinxOCL_FOUND)
  message(STATUS "enable xlnx_opencl")
  add_subdirectory(xlnx_opencl)
  set(BUILD_PLATFORM_MODULE true)
endif()

if(IntelOCL_FOUND)
  message(STATUS "enable altr_opencl")
  add_subdirectory(altr_opencl)
  set(BUILD_PLATFORM_MODULE true)
endif()

if(NOT BUILD_PLATFORM_MODULE)
  message(STATUS "skip platforms/xlnx_opencl compilation")
endif()
