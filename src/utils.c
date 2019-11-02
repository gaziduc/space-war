#include "utils.h"
#include "init.h"
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Texture *load_texture(const char *path, struct window *window)
{
    SDL_Surface *surface = IMG_Load(path);

    if (!surface)
        error("Could not load images", SDL_GetError(), window->window);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(window->renderer, surface);

    if (!texture)
        error("Could not load images", SDL_GetError(), window->window);

    SDL_FreeSurface(surface);
    return texture;
}


void init_position(int x, int y, struct window *window, SDL_Texture *texture,
                   SDL_Rect *pos)
{
    SDL_QueryTexture(texture, NULL, NULL, &pos->w, &pos->h);

    if (x == POS_CENTERED)
        pos->x = window->w / 2 - pos->w / 2;
    else
        pos->x = x;

    if (y == POS_CENTERED)
        pos->y = window->h / 2 - pos->h / 2;
    else
        pos->y = y;
}


void error(const char *title, const char *text, SDL_Window *window)
{
    if (window)
        SDL_DestroyWindow(window);

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL);
    exit(EXIT_FAILURE);
}


void *xmalloc(size_t size, SDL_Window *window)
{
    void *ptr = malloc(size);

    if (!ptr)
        error("malloc failed", "Memory allocation failed.", window);

    return ptr;
}

void *xcalloc(size_t nmenb, size_t size, SDL_Window *window)
{
    void *ptr = calloc(nmenb, size);

    if (!ptr)
        error("calloc failed", "Memory allocation failed.", window);

    return ptr;
}
