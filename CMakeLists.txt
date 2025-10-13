cmake_minimum_required(VERSION 3.11)

project(scolorpicker
    VERSION 2.5.1
    DESCRIPTION "smooll's Color Picker"
    LANGUAGES CXX C
)

option(SCP_ENABLE_X11 "Enable X11 backend support" OFF)
option(SCP_ENABLE_XLIB "Enable Xlib backend support" OFF)
option(SCP_ENABLE_XCB "Enable XCB backend support" OFF)
option(SCP_ENABLE_WAYLAND "Enable Wayland backend support" OFF)
option(SCP_ENABLE_LINUX "Enable X11 and Wayland backend support" OFF)
option(SCP_ENABLE_AUTO "Enable automatically detected backend" ON)
option(SCP_FORCE_XLIB "Force Xlib backend. Used with SCP_ENABLE_LINUX" OFF)
option(SCP_FORCE_XCB "Force XCB backend. Used with SCP_ENABLE_LINUX" OFF)
option(SCP_BUILD_DEPENDENCIES "Force download and build of dependencies regardless of system availability" OFF)
option(SCP_BUILD_DOCUMENTATION "Compress documentation and install it if desired" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(FETCHCONTENT_QUIET OFF)

set(languages "en" "pl")

include(GNUInstallDirs)

set(SCP_DATA_DIRECTORY_DEV "${CMAKE_SOURCE_DIR}/data")
set(SCP_DATA_DIRECTORY_REL "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/scolorpicker")

include(FetchContent)
include(cmake/check_options.cmake)
include(cmake/setup_sdl.cmake)

configure_file("${CMAKE_SOURCE_DIR}/cmake/config.hpp.in" "${CMAKE_SOURCE_DIR}/src/config.hpp")

file(GLOB_RECURSE cli_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/CLI/*.cpp")
file(GLOB_RECURSE utils_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Utils/*.cpp")
file(GLOB_RECURSE color_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/Color/*.cpp")
file(GLOB_RECURSE ui_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/UI/*.cpp")
file(GLOB_RECURSE stb_image_src CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/stb_image/*.c")

set(exe_src "${CMAKE_SOURCE_DIR}/src/main.cpp")

list(APPEND exe_src ${cli_src} ${utils_src} ${color_src} ${screenshot_src} ${ui_src} ${stb_image_src})

add_executable(${CMAKE_PROJECT_NAME} ${exe_src})

target_compile_options(${CMAKE_PROJECT_NAME} BEFORE PUBLIC -Wall)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src"
    ${_SCP_SDBUSCPP_INCLUDE_DIRECTORY})

target_link_libraries(${CMAKE_PROJECT_NAME} SDL3::SDL3 ${SCP_SDBUSCPP} ${SCP_SYSTEM_LIBRARIES})

# Install executable
install(TARGETS "${CMAKE_PROJECT_NAME}"
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/data/messages"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/scolorpicker"
)

# Install all cursor files
file(GLOB cursors "${CMAKE_SOURCE_DIR}/data/cursor/cursor_*.bmp")
install(FILES ${cursors}
    DESTINATION "${CMAKE_INSTALL_DATADIR}/scolorpicker/cursor"
)

# Install desktop file
install(FILES "${CMAKE_SOURCE_DIR}/data/desktop/scolorpicker.desktop"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
)

# Install icon files
install(FILES "${CMAKE_SOURCE_DIR}/data/logo/logo_16x16.png"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/16x16/apps"
    RENAME "scolorpicker.png"
)
install(FILES "${CMAKE_SOURCE_DIR}/data/logo/logo_32x32.png"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps"
    RENAME "scolorpicker.png"
)
install(FILES "${CMAKE_SOURCE_DIR}/data/logo/logo_64x64.png"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps"
    RENAME "scolorpicker.png"
)
install(FILES "${CMAKE_SOURCE_DIR}/data/logo/logo_128x128.png"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps"
    RENAME "scolorpicker.png"
)

if(SCP_BUILD_DOCUMENTATION)
    include(cmake/localize.cmake)
endif()
