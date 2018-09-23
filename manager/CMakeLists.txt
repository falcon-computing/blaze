include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${ProtobufIncludePath})

set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/blaze)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SRC_LIST)

set(ManagerIncludePath ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL "Path to manager include")

add_library(blaze STATIC ${SRC_LIST})
target_include_directories(blaze PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_dependencies(blaze Google FalconLM KSight)
target_link_libraries(blaze blaze_message)

set(HEADER_FILES
    ${include_dir}/BaseClient.h
    ${include_dir}/Client.h
    ${include_dir}/Common.h
    ${include_dir}/ReserveClient.h
    ${include_dir}/Task.h
    ${include_dir}/TaskEnv.h)

set_target_properties(blaze PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

add_executable(nam ${CMAKE_CURRENT_SOURCE_DIR}/src/executables/main.cpp)
add_dependencies(nam Google FalconLM KSight)

target_link_libraries(nam
    blaze
    ${Google_LIBRARIES}
    ${FalconLM_LIBRARIES}
    ${KSight_LIBRARIES}
    ${CMAKE_DL_LIBS} ${ZLIB_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(admin ${CMAKE_CURRENT_SOURCE_DIR}/src/executables/admin.cpp)
add_dependencies(admin Google FalconLM KSight)
target_link_libraries(admin
    blaze
    ${Google_LIBRARIES}
    ${FalconLM_LIBRARIES}
    ${KSight_LIBRARIES}
    ${CMAKE_DL_LIBS} ${ZLIB_LIBRARIES} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(test)

# install options
install(TARGETS blaze
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include/blaze)

install(TARGETS nam 
    RUNTIME DESTINATION bin)

#install(TARGETS admin
#    RUNTIME DESTINATION bin)
