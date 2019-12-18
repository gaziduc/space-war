#include "utils.h"
#include "init.h"
#include "pixel.h"
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

SDL_Texture *load_texture(const char *path, struct window *window)
{
    SDL_Surface *surface = IMG_Load(path);

    if (!surface)
        error("Could not load images", IMG_GetError(), window->window);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(window->renderer, surface);

    if (!texture)
        error("Could not load images", SDL_GetError(), window->window);

    SDL_FreeSurface(surface);

    return texture;
}


struct collision_texture *load_texture_collision(const char *path, struct window *window)
{
    SDL_Surface *surface = IMG_Load(path);

    if (!surface)
        error("Could not load images", IMG_GetError(), window->window);

    struct collision_texture *collision = xmalloc(sizeof(struct collision_texture), window->window);

    collision->w = surface->w;
    collision->h = surface->h;
    collision->collision = xmalloc(surface->w * surface->h * sizeof(short), window->window);

    SDL_LockSurface(surface);

    for (int i = 0; i < surface->w; i++)
    {
        for (int j = 0; j < surface->h; j++)
        {
            Uint32 pixel = get_pixel(surface, i, j);
            Uint8 r = 0;
            Uint8 g = 0;
            Uint8 b = 0;
            Uint8 a = 0;
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            collision->collision[j * surface->w + i] = a >= 128;
        }
    }

    SDL_UnlockSurface(surface);

    collision->texture = SDL_CreateTextureFromSurface(window->renderer, surface);

    if (!collision->texture)
        error("Could not load images", SDL_GetError(), window->window);

    SDL_FreeSurface(surface);

    return collision;
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


void *xrealloc(void *ptr, size_t size, SDL_Window *window)
{
    void *p = realloc(ptr, size);

    if (!p)
        error("realloc failed", "Memory allocation failed.", window);

    return p;
}


SDL_Texture *get_text_texture(struct window *window, TTF_Font *font,
                              const char *text, SDL_Color fg)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, fg);

    if (!surface)
        error("Could not create text surface", TTF_GetError(), window->window);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(window->renderer, surface);

    if (!texture)
        error("Could not create text texture", SDL_GetError(), window->window);

    SDL_FreeSurface(surface);
    return texture;
}


void render_text(struct window *window, TTF_Font *font, const char *text, SDL_Color fg,
                 int x, int y)
{
    SDL_Texture *texture = get_text_texture(window, font, text, fg);

    int w = 0;
    int h = 0;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    if (x == POS_CENTERED)
        x = window->w / 2 - w / 2;

    if (y == POS_CENTERED)
        y = window->h / 2 - h / 2;

    SDL_Rect pos_dst = { .x = x, .y = y, .w = w, .h = h };
    SDL_RenderCopy(window->renderer, texture, NULL, &pos_dst);

    SDL_DestroyTexture(texture);
}


TTF_Font *load_font(struct window *window, const char *filename,
                    int pt_size)
{
    TTF_Font *font = TTF_OpenFont(filename, pt_size);

    if (!font)
        error("Could not load font", TTF_GetError(), window->window);

    return font;
}


Mix_Chunk *load_sound(struct window *window, const char *filename)
{
    Mix_Chunk *sound = Mix_LoadWAV(filename);

    if (!sound)
        error("Could not load sound", Mix_GetError(), window->window);

    return sound;
}
