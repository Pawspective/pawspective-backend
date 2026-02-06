set(DTO_SOURCES 
    src/dummy.cpp
)

add_library(pawspective_dto STATIC ${DTO_SOURCES})

target_include_directories(pawspective_dto PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_dto
    PUBLIC
)