set(UTILS_SOURCES 
    src/dummy.cpp
)

add_library(pawspective_utils STATIC ${UTILS_SOURCES})

target_include_directories(pawspective_utils PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_utils
    PUBLIC
)