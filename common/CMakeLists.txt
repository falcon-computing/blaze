file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})

set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}
    CACHE INTERNAL "Path to generated protobuf files.")

add_library(blaze_message STATIC ${ProtoSources} ${ProtoHeaders})

target_include_directories(blaze_message PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(blaze_message ${PROTOBUF_LIBRARY})
set_target_properties(blaze_message PROPERTIES
    PUBLIC_HEADER "${ProtoHeaders}")

install(TARGETS blaze_message
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/blaze)
