include_directories(
    ${ManagerIncludePath}
    ${ProtobufIncludePath})

# check xlnx platform
if(DEFINED ENV{XILINX_SDX})
  message(STATUS "XILINX_SDX is defined, enable xlnx_opencl")
  set(XILINX_SDX $ENV{XILINX_SDX})
  add_subdirectory(xlnx_opencl)
else()
  message(WARNING "skip xlnx_opencl compilation")
endif()
