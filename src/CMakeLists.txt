add_subdirectory(dto)
add_subdirectory(handlers)
add_subdirectory(models)
add_subdirectory(repositories)
add_subdirectory(services)
add_subdirectory(utils)

add_library(pawspective_src INTERFACE)

target_link_libraries(pawspective_src INTERFACE 
    pawspective_dto
    pawspective_handlers
    pawspective_models
    pawspective_repositories
    pawspective_services
    pawspective_utils
)

target_include_directories(pawspective_src INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})