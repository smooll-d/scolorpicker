cmake_minimum_required(VERSION 3.31)

project(scolorpicker
    VERSION 2.0.0
    DESCRIPTION "smooll Color Picker"
    LANGUAGES CXX
)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

configure_file("${CMAKE_SOURCE_DIR}/cmake/version.hpp.in" "${CMAKE_SOURCE_DIR}/src/version.hpp")

include(cmake/setup_sdl.cmake)

file(GLOB_RECURSE exe_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${exe_src})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(${CMAKE_PROJECT_NAME} SDL3::SDL3 SDL3_ttf::SDL3_ttf X11)
