include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ProtobufIncludePath})

set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/blaze)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)

set(ManagerIncludePath ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL "Path to manager include")

add_library(blaze STATIC ${SRC_LIST})
target_include_directories(blaze PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_dependencies(blaze Google FalconLM)
target_link_libraries(blaze blaze_message)

set(HEADER_FILES
    ${include_dir}/AccAgent.h
    ${include_dir}/BaseClient.h
    ${include_dir}/Client.h
    ${include_dir}/Common.h
    ${include_dir}/ReserveClient.h
    ${include_dir}/Task.h
    ${include_dir}/TaskEnv.h)

set_target_properties(blaze PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

add_executable(manager ${CMAKE_CURRENT_SOURCE_DIR}/src/executables/main.cpp)
add_dependencies(manager Google FalconLM)

target_link_libraries(manager
    blaze
    ${Google_LIBRARIES}
    ${FalconLM_LIBRARIES}
    ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(admin ${CMAKE_CURRENT_SOURCE_DIR}/src/executables/admin.cpp)
add_dependencies(admin Google FalconLM)
target_link_libraries(admin
    blaze
    ${Google_LIBRARIES}
    ${CMAKE_DL_LIBS} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(test)

# install options
install(TARGETS blaze
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/blaze)

install(TARGETS manager
    RUNTIME DESTINATION bin)

install(TARGETS admin
    RUNTIME DESTINATION bin)
