cmake_minimum_required(VERSION 3.31)

project(scolorpicker
    VERSION 2.0.18012025.0
    DESCRIPTION "smooll Color Picker"
    LANGUAGES CXX
)

add_subdirectory(vendor/SDL EXCLUDE_FROM_ALL)

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
