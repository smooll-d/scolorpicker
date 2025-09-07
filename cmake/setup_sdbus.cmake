find_package(sdbus-c++ CONFIG)

if (NOT TARGET SDBusCpp::sdbus-c++ OR SCP_BUILD_DEPENDENCIES)
    set(SCP_SDBUSCPP sdbus-c++ CACHE INTERNAL "SCP_SDBUSCPP")
    set(SDBUSCPP_INSTALL OFF)

    FetchContent_Declare(
        sdbus-c++
        GIT_REPOSITORY https://github.com/Kistler-Group/sdbus-cpp.git
        GIT_TAG v2.1.0
        GIT_SHALLOW ON
        GIT_PROGRESS ON
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(sdbus-c++)
else()
    set(SCP_SDBUSCPP SDBusCpp::sdbus-c++ CACHE INTERNAL "SCP_SDBUSCPP")
endif()