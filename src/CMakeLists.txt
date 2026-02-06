add_subdirectory(handlers)

add_library(pawspective_src INTERFACE)

target_link_libraries(pawspective_src INTERFACE 
    pawspective_handlers
)

target_include_directories(pawspective_src INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})