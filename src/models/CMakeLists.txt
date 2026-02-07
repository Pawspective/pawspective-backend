set(MODELS_SOURCES 
    src/user.cpp
)

add_library(pawspective_models STATIC ${MODELS_SOURCES})

target_include_directories(pawspective_models PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_models
    PUBLIC
    userver::core
)