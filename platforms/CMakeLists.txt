include_directories(
    ${ManagerIncludePath}
    ${ProtobufIncludePath})

# check xlnx platform
set(BUILD_PLATFORM_MODULE false)
if(DEFINED ENV{XILINX_SDX})
  message(STATUS "XILINX_SDX is defined, enable xlnx_opencl")
  set(XILINX_SDX $ENV{XILINX_SDX})
  add_subdirectory(xlnx_opencl)
  set(BUILD_PLATFORM_MODULE true)
endif()

if(DEFINED ENV{ALTERAOCLSDKROOT})
  message(STATUS "ALTERAOCLSDKROOT is defined, enable altr_opencl")
  set(ALTERAOCLSDKROOT $ENV{ALTERAOCLSDKROOT})
  find_package( IntelAltera REQUIRED )
  add_subdirectory(altr_opencl)
  set(BUILD_PLATFORM_MODULE true)
endif()

if(NOT BUILD_PLATFORM_MODULE)
  message(STATUS "skip platforms/xlnx_opencl compilation")
endif()
