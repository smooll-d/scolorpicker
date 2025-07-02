set(SDBUSCPP_INSTALL OFF)
set(BUILD_SHARED_LIBS ON)

FetchContent_Declare(
    sdbus-c++
    GIT_REPOSITORY https://github.com/Kistler-Group/sdbus-cpp.git
    GIT_TAG v2.1.0
    GIT_SHALLOW ON
    GIT_PROGRESS ON
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(sdbus-c++)
