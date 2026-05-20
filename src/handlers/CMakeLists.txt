set(HANDLERS_SOURCES 
    src/health_handler.cpp
    src/auth_me_handler.cpp
    src/auth_checker.cpp
    
    src/auth_refresh_handler.cpp
    src/user_registration_handler.cpp
    src/user_update_handler.cpp
    src/auth_login_handler.cpp
    src/auth_logout_handler.cpp
    src/org_registration_handler.cpp
    src/animal_registration_handler.cpp
    src/org_update_handler.cpp
    src/org_get_handler.cpp
    src/org_search_handler.cpp
    src/city_list_handler.cpp
    src/animal_update_handler.cpp
    src/animal_filters_handler.cpp
    src/org_animals_handler.cpp
    src/animal_get_handler.cpp
    src/animal_list_handler.cpp
    src/breed_list_handler.cpp
    src/animal_delete_handler.cpp
    src/org_delete_handler.cpp
    src/user_delete_handler.cpp
    src/posts_get_handler.cpp
)

add_library(pawspective_handlers STATIC ${HANDLERS_SOURCES})

target_include_directories(pawspective_handlers PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_handlers
    PUBLIC
    userver::core
    pawspective_dto
    pawspective_components
    pawspective_services
    pawspective_utils
)