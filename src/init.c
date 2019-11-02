#include "utils.h"
#include "init.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_framerate.h>

static struct window *init_window(int width, int height)
{
    struct window *window = xmalloc(sizeof(struct window), NULL);

    window->window = SDL_CreateWindow("Space war",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      width,
                                      height,
                                      SDL_WINDOW_FULLSCREEN);

    if (!window->window)
        error("Could not create window", SDL_GetError(), NULL);

    window->w = width;
    window->h = height;

    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (!window->renderer)
        error("Could not create renderer", SDL_GetError(), NULL);

    return window;
}


static void load_textures(struct window *window)
{
    window->img = xmalloc(sizeof(struct textures), window->window);

    window->img->ship = load_texture("data/ship.bmp", window);
    window->img->bg = load_texture("data/background.jpg", window);
}


struct window *init_all(int width, int height)
{
    // Init SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        error("Could not load SDL2", SDL_GetError(), NULL);

    // Create window and renderer
    struct window *window = init_window(width, height);

    // Init SDL2_image
    int flags = IMG_INIT_JPG;
    int initted = IMG_Init(flags);
    if ((initted & flags) != flags)
        error("Could not load SDL2_image", IMG_GetError(), window->window);

    // Load textures
    load_textures(window);

    // Init inputs
    window->in = xcalloc(1, sizeof(struct input), window->window);

    // Init framerate manager
    window->fps = xmalloc(sizeof(FPSmanager), window->window);
    SDL_initFramerate(window->fps);
    if (SDL_setFramerate(window->fps, 60) == -1)
        error("Could not set framerate", "Could not set framerate to 60 Hz", window->window);

    return window;
}
