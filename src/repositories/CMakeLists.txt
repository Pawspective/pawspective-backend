set(REPOSITORIES_SOURCES 
    src/dummy.cpp
)

add_library(pawspective_repositories STATIC ${REPOSITORIES_SOURCES})

target_include_directories(pawspective_repositories PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_repositories
    PUBLIC
)