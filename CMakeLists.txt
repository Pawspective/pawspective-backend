cmake_minimum_required(VERSION 3.12)
project(pawspective CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(DownloadUserver)

find_package(
    userver
    COMPONENTS core
    QUIET
)
if(NOT userver_FOUND)
    # Tries TRY_DIR first, falls back to downloading userver from GitHub using CPM.
    download_userver(TRY_DIR third_party/userver)
endif()

userver_setup_environment()

if(TARGET userver::core)
    foreach(prop 
        INTERFACE_COMPILE_DEFINITIONS 
        INTERFACE_COMPILE_OPTIONS
        COMPILE_DEFINITIONS
        COMPILE_OPTIONS)
        get_target_property(USERVER_PROPS userver::core ${prop})
        if(USERVER_PROPS AND NOT USERVER_PROPS STREQUAL "USERVER_PROPS-NOTFOUND")
            list(FILTER USERVER_PROPS EXCLUDE REGEX "USERVER_NAMESPACE")
            set_property(TARGET userver::core PROPERTY ${prop} ${USERVER_PROPS})
        endif()
    endforeach()
endif()


## Common sources
add_subdirectory(src)

get_directory_property(DIR_COMPILE_DEFS COMPILE_DEFINITIONS)
if(DIR_COMPILE_DEFS)
    list(FILTER DIR_COMPILE_DEFS EXCLUDE REGEX "USERVER_NAMESPACE")
    set_directory_properties(PROPERTIES COMPILE_DEFINITIONS "${DIR_COMPILE_DEFS}")
endif()

add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE pawspective_src)


## Unit Tests

## Benchmarks

# Functional testing
userver_testsuite_add_simple()

# Install
include(GNUInstallDirs)

if(DEFINED ENV{PREFIX})
    set(PREFIX_PATH "$ENV{PREFIX}")
else()
    set(PREFIX_PATH "$ENV{HOME}/.local")
endif()

file(TO_CMAKE_PATH "${PREFIX_PATH}" PREFIX_PATH)

set(CMAKE_INSTALL_PREFIX "${PREFIX_PATH}")
message(STATUS "Set install prefix: $ENV{PREFIX}")

file(GLOB CONFIGS_FILES ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.yaml ${CMAKE_CURRENT_SOURCE_DIR}/configs/*.json)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${PROJECT_NAME}
)
install(
    FILES ${CONFIGS_FILES}
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)