set(COMPONENTS_SOURCES 
    src/jwt_component.cpp
    src/pg_session_component.cpp
    src/city_repository_component.cpp
    src/city_service_component.cpp
    src/user_repository_component.cpp
    src/user_service_component.cpp
    src/organization_repository_component.cpp
    src/organization_service_component.cpp
    src/animal_repository_component.cpp
    src/breed_repository_component.cpp
    src/animal_service_component.cpp
    src/breed_service_component.cpp
    src/post_repository_component.cpp
    src/review_repository_component.cpp
    src/review_service_component.cpp
    src/adopt_request_repository_component.cpp
)

add_library(pawspective_components STATIC ${COMPONENTS_SOURCES})

target_include_directories(pawspective_components PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_components
    PUBLIC
    userver::core
    pawspective_services
    pawspective_models
)