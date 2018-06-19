get_filename_component(DestID ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_library(${DestID} SHARED ${DestID}.cpp)
