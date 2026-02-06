set(SERVICES_SOURCES 
    src/dummy.cpp
)

add_library(pawspective_services STATIC ${SERVICES_SOURCES})

target_include_directories(pawspective_services PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_services
    PUBLIC

)