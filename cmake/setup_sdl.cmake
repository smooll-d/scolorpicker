# SDL flags
set(SDL_TESTS OFF)
set(SDL_EXAMPLES OFF)
set(SDL_SHARED ON)
set(SDL_STATIC OFF)
set(SDL_INSTALL_DOCS OFF)
set(SDL_INSTALL_TESTS OFF)
set(SDL_TEST_LIBRARY OFF)
set(SDL_TESTS OFF)
set(SDL_DISABLE_INSTALL ON)

# SDL_image flags
set(SDLIMAGE_SAMPLES OFF)

# SDL_ttf flags
set(SDLTTF_SAMPLES OFF)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-3.2.14
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(SDL3)

# Disabled for this project
# FetchContent_Declare(
#     SDL_mixer
#     GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
#     GIT_TAG        main
#     GIT_SHALLOW    TRUE
#     GIT_PROGRESS   TRUE
#     OVERRIDE_FIND_PACKAGE
# )
# FetchContent_MakeAvailable(SDL_mixer)

# FetchContent_Declare(
#     SDL_image
#     GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
#     GIT_TAG        release-3.2.4
#     GIT_SHALLOW    TRUE
#     GIT_PROGRESS   TRUE
#     OVERRIDE_FIND_PACKAGE
# )
# FetchContent_MakeAvailable(SDL_image)

FetchContent_Declare(
    SDL_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG        release-3.2.2
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(SDL_ttf)
