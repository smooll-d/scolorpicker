cmake_minimum_required(VERSION 3.31)

project(scolorpicker
    VERSION 2.0.18012025.0
    DESCRIPTION "smooll Color Picker"
    LANGUAGES CXX
)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-3.2.0
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(sdl3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

configure_file(input/metadata.hpp.in ${CMAKE_SOURCE_DIR}/include/SCP/metadata.hpp)

file(GLOB_RECURSE exe_src "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${exe_src})

target_compile_options(${CMAKE_PROJECT_NAME} BEFORE PRIVATE -g)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE
            SDL3::SDL3
            X11
    )
endif()
