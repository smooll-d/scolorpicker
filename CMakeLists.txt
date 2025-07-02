cmake_minimum_required(VERSION 3.31)

project(scolorpicker
    VERSION 2.0.0
    DESCRIPTION "smooll Color Picker"
    LANGUAGES CXX
)

option(SCP_ENABLE_X11 "Enable X11 backend support" OFF)
option(SCP_ENABLE_XLIB "Enable Xlib backend support" OFF)
option(SCP_ENABLE_XCB "Enable xcb backend support" OFF)
option(SCP_ENABLE_WAYLAND "Enable Wayland backend support" OFF)
option(SCP_ENABLE_LINUX "Enable X11 and Wayland backend support" OFF)
option(SCP_ENABLE_AUTO "Enable automatically detected backend" ON)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

include(FetchContent)
include(cmake/check_options.cmake)
include(cmake/setup_sdl.cmake)

configure_file("${CMAKE_SOURCE_DIR}/cmake/config.hpp.in" "${CMAKE_SOURCE_DIR}/src/config.hpp")

message(STATUS "")
message(STATUS "|--------------------------------|")
message(STATUS "|   SCOLORPICKER CONFIGURATION   |")
message(STATUS "|--------------------------------|")
message(STATUS "|    SCP_ENABLE_X11     ${SCP_ENABLE_X11}\t\t|")
message(STATUS "|    SCP_ENABLE_XLIB    ${SCP_ENABLE_XLIB}\t\t|")
message(STATUS "|    SCP_ENABLE_XCB     ${SCP_ENABLE_XCB}\t\t|")
message(STATUS "|    SCP_ENABLE_WAYLAND ${SCP_ENABLE_WAYLAND}\t\t|")
message(STATUS "|    SCP_ENABLE_LINUX   ${SCP_ENABLE_LINUX}\t\t|")
message(STATUS "|    SCP_ENABLE_AUTO    ${SCP_ENABLE_AUTO}\t\t|")
message(STATUS "|--------------------------------|")
message(STATUS "")

file(GLOB_RECURSE cli_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/CLI/*.cpp")
file(GLOB_RECURSE utils_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Utils/*.cpp")

set(exe_src "${CMAKE_SOURCE_DIR}/src/main.cpp")

list(APPEND exe_src ${cli_src} ${utils_src} ${screenshot_src})

add_executable(${CMAKE_PROJECT_NAME} ${exe_src})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src" ${_SCP_SDBUSCPP_INCLUDE_DIRECTORY})

target_link_libraries(${CMAKE_PROJECT_NAME} SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf ${SCP_SYSTEM_LIBRARIES})
