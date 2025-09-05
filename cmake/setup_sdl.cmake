find_package(SDL3 CONFIG)

if (NOT TARGET SDL3::SDL3)
    set(SDL_TESTS OFF)
    set(SDL_EXAMPLES OFF)
    set(SDL_SHARED ON)
    set(SDL_STATIC OFF)
    set(SDL_INSTALL_DOCS OFF)
    set(SDL_INSTALL_TESTS OFF)
    set(SDL_TEST_LIBRARY OFF)
    set(SDL_TESTS OFF)
    set(SDL_DISABLE_INSTALL ON)

    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG        release-3.2.22
        GIT_SHALLOW    ON
        GIT_PROGRESS   ON
    )
    FetchContent_MakeAvailable(SDL3)
endif()

find_package(SDL3_image CONFIG)

if (NOT TARGET SDL3_image::SDL3_image)
    set(SDLIMAGE_SAMPLES OFF)

    FetchContent_Declare(
        SDL_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG        release-3.2.4
        GIT_SHALLOW    ON
        GIT_PROGRESS   ON
    )
    FetchContent_MakeAvailable(SDL_image)
endif()

# Disabled for this project
# find_package(SDL3_mixer CONFIG)

# if (NOT TARGET SDL3_mixer::SDL3_mixer)
    # FetchContent_Declare(
    #     SDL_mixer
    #     GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    #     GIT_TAG        main
    #     GIT_SHALLOW    ON
    #     GIT_PROGRESS   ON
    # )
    # FetchContent_MakeAvailable(SDL_mixer)
# endif()

# Disabled for this project
# find_package(SDL3_ttf CONFIG)

# if (NOT TARGET SDL3_ttf::SDL3_ttf)
    # set(SDLTTF_SAMPLES OFF)
    #
    # FetchContent_Declare(
    #     SDL_ttf
    #     GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    #     GIT_TAG        release-3.2.2
    #     GIT_SHALLOW    ON
    #     GIT_PROGRESS   ON
    # )
    # FetchContent_MakeAvailable(SDL_ttf)
# endif()