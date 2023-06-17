cmake_minimum_required(VERSION 3.26)

project(scolorpicker
    VERSION 1.2.0
    DESCRIPTION "smooll Color Picker for X11"
    LANGUAGES C
)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

configure_file(input/CMake_Config.h.in ${CMAKE_SOURCE_DIR}/include/CMake/CMake_Config.h)

set(exe_src
    src/SCP_Clipboard.c
    src/SCP_CLI.c
    src/SCP.c
    src/SCP_Main.c
)

add_executable(${PROJECT_NAME} ${exe_src})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        X11
        Xext
)
