function(scp_enable_x11)
    message(STATUS "X11 backend support enabled")

    set(SCP_ENABLE_X11 ON PARENT_SCOPE)
    set(SCP_ENABLE_WAYLAND OFF PARENT_SCOPE)
    set(SCP_ENABLE_LINUX OFF PARENT_SCOPE)
    set(SCP_ENABLE_AUTO OFF PARENT_SCOPE)

    set(SDL_X11 ON PARENT_SCOPE)
    set(SDL_WAYLAND OFF PARENT_SCOPE)
endfunction()

function(scp_enable_wayland)
    message(STATUS "Wayland backend support enabled")

    set(SCP_ENABLE_X11 OFF PARENT_SCOPE)
    set(SCP_ENABLE_WAYLAND ON PARENT_SCOPE)
    set(SCP_ENABLE_LINUX OFF PARENT_SCOPE)
    set(SCP_ENABLE_AUTO OFF PARENT_SCOPE)

    set(SDL_X11 OFF PARENT_SCOPE)
    set(SDL_WAYLAND ON PARENT_SCOPE)
endfunction()

function(scp_enable_linux)
    message(STATUS "X11 and Wayland backend support enabled")

    set(SCP_ENABLE_X11 OFF PARENT_SCOPE)
    set(SCP_ENABLE_WAYLAND OFF PARENT_SCOPE)
    set(SCP_ENABLE_LINUX ON PARENT_SCOPE)
    set(SCP_ENABLE_AUTO OFF PARENT_SCOPE)

    set(SDL_X11 ON PARENT_SCOPE)
    set(SDL_WAYLAND ON PARENT_SCOPE)
endfunction()

if(SCP_ENABLE_X11)
    scp_enable_x11()
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
            scp_enable_x11()
        elseif(auto_backend MATCHES "wayland")
            scp_enable_wayland()
        endif()
    endif()
endif()
