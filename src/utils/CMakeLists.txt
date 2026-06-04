set(UTILS_SOURCES
    src/hasher.cpp
    src/exception.cpp
    src/validator.cpp
    src/error_response.cpp
    src/sql_builder.cpp
    src/s3_upload_client.cpp
)

add_library(pawspective_utils STATIC ${UTILS_SOURCES})

target_include_directories(pawspective_utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(pawspective_utils
    PUBLIC
    userver::core
    userver::postgresql
    userver::s3api
    ${ARGON2_LIBRARIES}
)

if(BUILD_TESTING)
    add_executable(pawspective_utils_tests
        tests/hasher_test.cpp
        tests/validator_test.cpp
        tests/sql_builder_test.cpp
        tests/s3_upload_client_test.cpp
    )

    target_link_libraries(pawspective_utils_tests
        PRIVATE
        pawspective_utils
        userver::utest
        userver-s3api-utest
    )

    add_test(NAME pawspective_utils_tests COMMAND pawspective_utils_tests)
endif()
