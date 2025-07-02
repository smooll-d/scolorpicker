include(CheckLibraryExists)

function(scp_enable_xlib)
    message(STATUS "Xlib backend support enabled")

    set(SCP_ENABLE_X11 ON CACHE INTERNAL "SCP_ENABLE_X11")
    set(SCP_ENABLE_XLIB ON CACHE INTERNAL "SCP_ENABLE_XLIB")
    set(SCP_ENABLE_XCB OFF CACHE INTERNAL "SCP_ENABLE_XCB")
    set(SCP_ENABLE_WAYLAND OFF CACHE INTERNAL "SCP_ENABLE_WAYLAND")
    set(SCP_ENABLE_LINUX OFF CACHE INTERNAL "SCP_ENABLE_LINUX")
    set(SCP_ENABLE_AUTO OFF CACHE INTERNAL "SCP_ENABLE_AUTO")

    list(APPEND _scp_sys_xlib X11)
    set(_SCP_SYSTEM_LIBRARIES_XLIB ${_scp_sys_xlib} CACHE INTERNAL "_SCP_SYSTEM_LIBRARIES_XLIB")

    set(screenshot_src
        "${CMAKE_SOURCE_DIR}/src/Screenshot/Screenshot.cpp"
        "${CMAKE_SOURCE_DIR}/src/Screenshot/Implementations/X11/Screenshot_X11.cpp"
        CACHE INTERNAL "screenshot_src")

    set(SDL_X11 ON CACHE INTERNAL "SDL_X11")
    set(SDL_WAYLAND OFF CACHE INTERNAL "SDL_WAYLAND")
endfunction()

function(scp_enable_xcb)
    message(STATUS "xcb backend support enabled")

    set(SCP_ENABLE_X11 ON CACHE INTERNAL "SCP_ENABLE_X11")
    set(SCP_ENABLE_XLIB OFF CACHE INTERNAL "SCP_ENABLE_XLIB")
    set(SCP_ENABLE_XCB ON CACHE INTERNAL "SCP_ENABLE_XCB")
    set(SCP_ENABLE_WAYLAND OFF CACHE INTERNAL "SCP_ENABLE_WAYLAND")
    set(SCP_ENABLE_LINUX OFF CACHE INTERNAL "SCP_ENABLE_LINUX")
    set(SCP_ENABLE_AUTO OFF CACHE INTERNAL "SCP_ENABLE_AUTO")

    list(APPEND _scp_sys_xcb xcb xcb-image)
    set(_SCP_SYSTEM_LIBRARIES_XCB ${_scp_sys_xcb} CACHE INTERNAL "_SCP_SYSTEM_LIBRARIES_XCB")

    set(screenshot_src
        "${CMAKE_SOURCE_DIR}/src/Screenshot/Screenshot.cpp"
        "${CMAKE_SOURCE_DIR}/src/Screenshot/Implementations/X11/Screenshot_X11.cpp"
        CACHE INTERNAL "screenshot_src")

    set(SDL_X11 ON CACHE INTERNAL "SDL_X11")
    set(SDL_WAYLAND OFF CACHE INTERNAL "SDL_WAYLAND")
endfunction()

function(scp_enable_wayland)
    message(STATUS "Wayland backend support enabled")

    include(cmake/setup_sdbus.cmake)

    set(SCP_ENABLE_X11 OFF CACHE INTERNAL "SCP_ENABLE_X11")
    set(SCP_ENABLE_XLIB OFF CACHE INTERNAL "SCP_ENABLE_XLIB")
    set(SCP_ENABLE_XCB OFF CACHE INTERNAL "SCP_ENABLE_XCB")
    set(SCP_ENABLE_WAYLAND ON CACHE INTERNAL "SCP_ENABLE_WAYLAND")
    set(SCP_ENABLE_LINUX OFF CACHE INTERNAL "SCP_ENABLE_LINUX")
    set(SCP_ENABLE_AUTO OFF CACHE INTERNAL "SCP_ENABLE_AUTO")

    #TODO: Add Wayland libraries
    list(APPEND _scp_sys_wayland sdbus-c++)
    set(_SCP_SDBUSCPP_INCLUDE_DIRECTORY "${sdbus-c++_SOURCE_DIR}/include" CACHE INTERNAL "_SCP_SDBUSCPP_INCLUDE_DIRECTORY")
    set(_SCP_SYSTEM_LIBRARIES_WAYLAND ${_scp_sys_wayland} CACHE INTERNAL "_SCP_SYSTEM_LIBRARIES_WAYLAND")

    set(screenshot_src
        "${CMAKE_SOURCE_DIR}/src/Screenshot/Screenshot.cpp"
        "${CMAKE_SOURCE_DIR}/src/Screenshot/Implementations/Wayland/Screenshot_Wayland.cpp"
        CACHE INTERNAL "screenshot_src")

    set(SDL_X11 OFF CACHE INTERNAL "SDL_X11")
    set(SDL_WAYLAND ON CACHE INTERNAL "SDL_WAYLAND")
endfunction()

function(scp_enable_linux)
    message(STATUS "X11 and Wayland backend support enabled")

    scp_enable_wayland()
    scp_detect_x11()

    set(SCP_ENABLE_X11 OFF CACHE INTERNAL "SCP_ENABLE_X11")
    set(SCP_ENABLE_XLIB ${SCP_ENABLE_XLIB} CACHE INTERNAL "SCP_ENABLE_XLIB")
    set(SCP_ENABLE_XCB ${SCP_ENABLE_XCB} CACHE INTERNAL "SCP_ENABLE_XCB")
    set(SCP_ENABLE_WAYLAND OFF CACHE INTERNAL "SCP_ENABLE_WAYLAND")
    set(SCP_ENABLE_LINUX ON CACHE INTERNAL "SCP_ENABLE_LINUX")
    set(SCP_ENABLE_AUTO OFF CACHE INTERNAL "SCP_ENABLE_AUTO")

    set(screenshot_src
        "${CMAKE_SOURCE_DIR}/src/Screenshot/Screenshot.cpp"
        "${CMAKE_SOURCE_DIR}/src/Screenshot/Implementations/X11/Screenshot_X11.cpp"
        "${CMAKE_SOURCE_DIR}/src/Screenshot/Implementations/Wayland/Screenshot_Wayland.cpp"
        CACHE INTERNAL "screenshot_src")

    set(SDL_X11 ON CACHE INTERNAL "SDL_X11")
    set(SDL_WAYLAND ON CACHE INTERNAL "SDL_WAYLAND")
endfunction()

function(scp_detect_x11)
    message(STATUS "Checking for Xlib and xcb...")

    check_library_exists(xcb xcb_connect "" _SCP_HAS_XCB)
    check_library_exists(xcb-image xcb_image_get "" _SCP_HAS_XCB_IMAGE)
    check_library_exists(X11 XOpenDisplay "" _SCP_HAS_XLIB)

    set(SCP_HAS_XCB _SCP_HAS_XCB CACHE INTERNAL "SCP_HAS_XCB")
    set(SCP_HAS_XCB_IMAGE _SCP_HAS_XCB_IMAGE CACHE INTERNAL "SCP_HAS_XCB_IMAGE")
    set(SCP_HAS_XLIB _SCP_HAS_XLIB CACHE INTERNAL "SCP_HAS_XLIB")

    if(SCP_HAS_XCB AND SCP_HAS_XCB_IMAGE)
        scp_enable_xcb()
    elseif(SCP_HAS_XLIB AND NOT SCP_HAS_XCB AND NOT SCP_HAS_XCB_IMAGE)
        scp_enable_xlib()
    endif()
endfunction()

if(SCP_ENABLE_X11)
    scp_detect_x11()
elseif(SCP_ENABLE_XLIB)
    scp_enable_xlib()
elseif(SCP_ENABLE_XCB)
    scp_enable_xcb()
elseif(SCP_ENABLE_WAYLAND)
    scp_enable_wayland()
elseif(SCP_ENABLE_LINUX)
    scp_enable_linux()
elseif(SCP_ENABLE_AUTO)
    message(STATUS "Detecting backend...")

    if (LINUX)
        execute_process(
            COMMAND bash -c "echo $XDG_SESSION_TYPE"
            OUTPUT_VARIABLE auto_backend
        )

        if(auto_backend MATCHES "x11")
            scp_detect_x11()
        elseif(auto_backend MATCHES "wayland")
            scp_enable_wayland()
        endif()
    endif()
endif()

set(SCP_SYSTEM_LIBRARIES ${_SCP_SYSTEM_LIBRARIES_XLIB} ${_SCP_SYSTEM_LIBRARIES_XCB} ${_SCP_SYSTEM_LIBRARIES_WAYLAND} CACHE INTERNAL "SCP_SYSTEM_LIBRARIES")
