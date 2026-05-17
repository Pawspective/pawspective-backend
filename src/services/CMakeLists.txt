set(SERVICES_SOURCES 
    src/city_service.cpp
    src/jwt_service.cpp
    src/pg_session_service.cpp
    src/exception.cpp
    src/user_service.cpp
    src/organization_service.cpp
    src/animal_service.cpp
    src/breed_service.cpp
    src/post_service.cpp
    src/review_service.cpp
    src/adopt_request_service.cpp
)

add_library(pawspective_services STATIC ${SERVICES_SOURCES})

target_include_directories(pawspective_services PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_services
    PUBLIC
    userver::core
    pawspective_dto
    pawspective_models
    pawspective_repositories
    pawspective_utils
)