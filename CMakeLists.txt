cmake_minimum_required(VERSION 3.11)

project(scolorpicker
    VERSION 2.0.0
    DESCRIPTION "smooll's Color Picker"
    LANGUAGES CXX
)

option(SCP_ENABLE_X11 "Enable X11 backend support" OFF)
option(SCP_ENABLE_XLIB "Enable Xlib backend support" OFF)
option(SCP_ENABLE_XCB "Enable XCB backend support" OFF)
option(SCP_ENABLE_WAYLAND "Enable Wayland backend support" OFF)
option(SCP_ENABLE_LINUX "Enable X11 and Wayland backend support" OFF)
option(SCP_ENABLE_AUTO "Enable automatically detected backend" ON)
option(SCP_FORCE_XLIB "Force Xlib backend. Used with SCP_ENABLE_LINUX" OFF)
option(SCP_FORCE_XCB "Force XCB backend. Used with SCP_ENABLE_LINUX" OFF)

set(CMAKE_BUILD_TYPE "RelWithDebInfo")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(FETCHCONTENT_QUIET OFF)

set(SCP_DATA_DIRECTORY_DEV "${CMAKE_SOURCE_DIR}/data")
set(SCP_DATA_DIRECTORY_REL "/usr/share/scolorpicker")

include(FetchContent)
include(cmake/check_options.cmake)
include(cmake/setup_sdl.cmake)

configure_file("${CMAKE_SOURCE_DIR}/cmake/config.hpp.in" "${CMAKE_SOURCE_DIR}/src/config.hpp")

file(GLOB_RECURSE cli_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/CLI/*.cpp")
file(GLOB_RECURSE utils_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Utils/*.cpp")
file(GLOB_RECURSE color_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Color/*.cpp")

set(exe_src "${CMAKE_SOURCE_DIR}/src/main.cpp")

list(APPEND exe_src ${cli_src} ${utils_src} ${color_src} ${screenshot_src})

add_executable(${CMAKE_PROJECT_NAME} ${exe_src})

target_compile_options(${CMAKE_PROJECT_NAME} BEFORE PUBLIC -Wall)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src" ${_SCP_SDBUSCPP_INCLUDE_DIRECTORY})

target_link_libraries(${CMAKE_PROJECT_NAME} SDL3::SDL3 SDL3_image::SDL3_image ${SCP_SDBUSCPP} ${SCP_SYSTEM_LIBRARIES})