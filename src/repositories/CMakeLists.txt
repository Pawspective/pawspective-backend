set(REPOSITORIES_SOURCES 
    src/user_repository.cpp
    src/organization_repository.cpp
    src/city_repository.cpp
    src/animal_repository.cpp
    src/breed_repository.cpp
)

add_library(pawspective_repositories STATIC ${REPOSITORIES_SOURCES})

target_include_directories(pawspective_repositories PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_repositories
    PUBLIC
    userver::core
    pawspective_models
    pawspective_utils
)