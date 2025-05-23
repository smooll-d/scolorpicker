include(FetchContent)

set(BUILD_EXAMPLE OFF)
set(BUILD_CSHARP OFF)

FetchContent_Declare(
    screen_capture_lite
    GIT_REPOSITORY https://github.com/smasherprog/screen_capture_lite.git
    GIT_TAG        v.91
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(screen_capture_lite)
