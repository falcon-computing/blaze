file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/../../common/*.proto")
PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})

set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}
    CACHE INTERNAL "Path to generated protobuf files.")

add_library(blaze_message STATIC ${ProtoSources} ${ProtoHeaders})

target_include_directories(blaze_message PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(blaze_message PROPERTIES
    LINKER_LANGUAGE CXX
    COMPILE_FLAGS "-fPIC")
target_link_libraries(blaze_message ${PROTOBUF_LIBRARY})
