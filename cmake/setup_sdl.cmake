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
    GIT_TAG        release-3.2.14
    GIT_SHALLOW    ON
    GIT_PROGRESS   ON
)
FetchContent_MakeAvailable(SDL3)

# Disabled for this project
# FetchContent_Declare(
#     SDL_mixer
#     GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
#     GIT_TAG        main
#     GIT_SHALLOW    ON
#     GIT_PROGRESS   ON
# )
# FetchContent_MakeAvailable(SDL_mixer)

set(SDLIMAGE_SAMPLES OFF)

FetchContent_Declare(
    SDL_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG        release-3.2.4
    GIT_SHALLOW    ON
    GIT_PROGRESS   ON
)
FetchContent_MakeAvailable(SDL_image)

set(SDLTTF_SAMPLES OFF)

FetchContent_Declare(
    SDL_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG        release-3.2.2
    GIT_SHALLOW    ON
    GIT_PROGRESS   ON
)
FetchContent_MakeAvailable(SDL_ttf)
