set(HANDLERS_SOURCES 
    src/health_handler.cpp
    
)

add_library(pawspective_handlers STATIC ${HANDLERS_SOURCES})

target_include_directories(pawspective_handlers PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_handlers
    PUBLIC
    userver::core
)