cmake_minimum_required(VERSION 3.25)

project(cpp-stellar-sdk
    VERSION 0.1.0
    DESCRIPTION "Stellar C++ SDK"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)


find_program(XDRC_EXECUTABLE xdrc REQUIRED)
find_library(XDRPP_LIBRARY NAMES xdrpp REQUIRED)
find_path(XDRPP_INCLUDE_DIR NAMES xdrpp/marshal.h REQUIRED)


add_library(xdrpp::xdrpp UNKNOWN IMPORTED)
set_target_properties(xdrpp::xdrpp PROPERTIES
    IMPORTED_LOCATION "${XDRPP_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${XDRPP_INCLUDE_DIR}"
)

add_subdirectory(xdr)

file(GLOB_RECURSE SDK_SOURCES "src/*.cpp")

add_library(cpp-stellar-sdk STATIC ${SDK_SOURCES})
add_library(stellar::sdk ALIAS cpp-stellar-sdk)

target_include_directories(cpp-stellar-sdk
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cpp-stellar-sdk
    PUBLIC
        stellar::xdr
        xdrpp::xdrpp
)

if(MSVC)
    target_compile_options(cpp-stellar-sdk PRIVATE /W4 /permissive-)
else()
    target_compile_options(cpp-stellar-sdk PRIVATE -Wall -Wextra -Wpedantic)
endif()