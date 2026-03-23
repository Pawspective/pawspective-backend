set(MODELS_SOURCES 
    src/user.cpp
    src/jwt_secrets.cpp
    src/organization.cpp
    src/city.cpp
    src/animal.cpp
    src/animal_enums.cpp
    src/breed.cpp
    src/animal_filters.cpp
)

add_library(pawspective_models STATIC ${MODELS_SOURCES})

target_include_directories(pawspective_models PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_models
    PUBLIC
    userver::core
    pawspective_dto
)