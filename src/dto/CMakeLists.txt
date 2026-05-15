set(DTO_SOURCES 
    src/user_dto.cpp
    src/user_register_dto.cpp
    src/user_update_dto.cpp
    src/organization_dto.cpp
    src/organization_register_dto.cpp
    src/organization_update_dto.cpp
    src/city_dto.cpp
    src/animal_dto.cpp
    src/animal_register_dto.cpp
    src/animal_update_dto.cpp
    src/breed_dto.cpp
    src/animal_filter_dto.cpp
    src/animal_list_dto.cpp
    src/organization_list_dto.cpp
    src/review_create_dto.cpp
    src/review_dto.cpp
    src/review_list_dto.cpp
    src/review_update_dto.cpp
    src/post_dto.cpp
    src/post_create_dto.cpp
    src/post_update_dto.cpp
    src/post_list_dto.cpp
)

add_library(pawspective_dto STATIC ${DTO_SOURCES})

target_include_directories(pawspective_dto PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_dto
    PUBLIC
    userver::core
    pawspective_models
)