include(CheckLibraryExists)

function(scp_enable_xlib)
    message(STATUS "Xlib backend support enabled")

    set(SCP_ENABLE_X11 ON PARENT_SCOPE)
    set(SCP_ENABLE_XLIB ON PARENT_SCOPE)
    set(SCP_ENABLE_XCB OFF PARENT_SCOPE)
    set(SCP_ENABLE_WAYLAND OFF PARENT_SCOPE)
    set(SCP_ENABLE_LINUX OFF PARENT_SCOPE)
    set(SCP_ENABLE_AUTO OFF PARENT_SCOPE)

    set(SDL_X11 ON PARENT_SCOPE)
    set(SDL_WAYLAND OFF PARENT_SCOPE)
endfunction()

function(scp_enable_xcb)
    message(STATUS "xcb backend support enabled")

    set(SCP_ENABLE_X11 ON PARENT_SCOPE)
    set(SCP_ENABLE_XLIB OFF PARENT_SCOPE)
    set(SCP_ENABLE_XCB ON PARENT_SCOPE)
    set(SCP_ENABLE_WAYLAND OFF PARENT_SCOPE)
    set(SCP_ENABLE_LINUX OFF PARENT_SCOPE)
    set(SCP_ENABLE_AUTO OFF PARENT_SCOPE)

    set(SDL_X11 ON PARENT_SCOPE)
    set(SDL_WAYLAND OFF PARENT_SCOPE)
endfunction()

function(scp_enable_wayland)
    message(STATUS "Wayland backend support enabled")

    set(SCP_ENABLE_X11 OFF PARENT_SCOPE)
    set(SCP_ENABLE_XLIB OFF PARENT_SCOPE)
    set(SCP_ENABLE_XCB OFF PARENT_SCOPE)
    set(SCP_ENABLE_WAYLAND ON PARENT_SCOPE)
    set(SCP_ENABLE_LINUX OFF PARENT_SCOPE)
    set(SCP_ENABLE_AUTO OFF PARENT_SCOPE)

    set(SDL_X11 OFF PARENT_SCOPE)
    set(SDL_WAYLAND ON PARENT_SCOPE)
endfunction()

function(scp_enable_linux)
    message(STATUS "X11 and Wayland backend support enabled")

    set(SCP_ENABLE_X11 OFF PARENT_SCOPE)
    set(SCP_ENABLE_XLIB OFF PARENT_SCOPE)
    set(SCP_ENABLE_XCB OFF PARENT_SCOPE)
    set(SCP_ENABLE_WAYLAND OFF PARENT_SCOPE)
    set(SCP_ENABLE_LINUX ON PARENT_SCOPE)
    set(SCP_ENABLE_AUTO OFF PARENT_SCOPE)

    set(SDL_X11 ON PARENT_SCOPE)
    set(SDL_WAYLAND ON PARENT_SCOPE)
endfunction()

function(scp_detect_x11)
    message(STATUS "Checking for Xlib and xcb...")

    check_library_exists(xcb xcb_connect "" _SCP_HAS_XCB)
    check_library_exists(xcb-image xcb_image_get "" _SCP_HAS_XCB_IMAGE)
    check_library_exists(X11 XOpenDisplay "" _SCP_HAS_XLIB)

    set(SCP_HAS_XCB _SCP_HAS_XCB PARENT_SCOPE)
    set(SCP_HAS_XCB_IMAGE _SCP_HAS_XCB_IMAGE PARENT_SCOPE)
    set(SCP_HAS_XLIB _SCP_HAS_XLIB PARENT_SCOPE)
endfunction()

if(SCP_ENABLE_X11)
    scp_detect_x11()

    if(SCP_HAS_XCB AND SCP_HAS_XCB_IMAGE)
        scp_enable_xcb()
    elseif(SCP_HAS_XLIB AND NOT SCP_HAS_XCB AND NOT SCP_HAS_XCB_IMAGE)
        scp_enable_xlib()
    endif()
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

            if(SCP_HAS_XCB AND SCP_HAS_XCB_IMAGE)
                scp_enable_xcb()
            elseif(SCP_HAS_XLIB AND NOT SCP_HAS_XCB AND NOT SCP_HAS_XCB_IMAGE)
                scp_enable_xlib()
            endif()
        elseif(auto_backend MATCHES "wayland")
            scp_enable_wayland()
        endif()
    endif()
endif()
