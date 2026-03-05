set(DTO_SOURCES 
    src/user_dto.cpp
    src/user_register_dto.cpp
    src/user_update_dto.cpp
    src/organization_dto.cpp
    src/organization_register_dto.cpp
    src/organization_update_dto.cpp
    src/animal_dto.cpp
    src/breed_dto.cpp
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