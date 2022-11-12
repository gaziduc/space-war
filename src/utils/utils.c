#include "utils.h"
#include "init.h"
#include "pixel.h"
#include "framerate.h"
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


SDL_Texture *load_texture(const char *path, struct window *window)
{
    render_loading_screen(window);

    SDL_Surface *surface = IMG_Load(path);

    if (!surface)
        error("Could not load images", IMG_GetError(), window->window, window->renderer);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(window->renderer, surface);

    if (!texture)
        error("Could not load images", SDL_GetError(), window->window, window->renderer);

    SDL_FreeSurface(surface);

    return texture;
}


void frame_delay(FPSmanager *fps)
{
    framerateDelay(fps);
}


struct collision_texture *get_texture_collision(SDL_Surface *surface, struct window *window)
{
    struct collision_texture *collision = xmalloc(sizeof(struct collision_texture), window->window, window->renderer);

    collision->w = surface->w;
    collision->h = surface->h;
    collision->collision = xmalloc(surface->w * surface->h * sizeof(char), window->window, window->renderer);

    if (SDL_MUSTLOCK(surface))
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

    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);

    collision->texture = SDL_CreateTextureFromSurface(window->renderer, surface);

    if (!collision->texture)
        error("Could not load images", SDL_GetError(), window->window, window->renderer);

    return collision;
}


struct collision_texture *load_texture_collision(const char *path, struct window *window)
{
    render_loading_screen(window);

    SDL_Surface *surface = IMG_Load(path);

    if (!surface)
        error("Could not load images", IMG_GetError(), window->window, window->renderer);

    struct collision_texture *result = get_texture_collision(surface, window);

    SDL_FreeSurface(surface);

    return result;
}

void init_position(int x, int y, SDL_Texture *texture, SDL_Rect *pos)
{
    SDL_QueryTexture(texture, NULL, NULL, &pos->w, &pos->h);

    if (x == POS_CENTERED)
        pos->x = DEFAULT_W / 2 - pos->w / 2;
    else
        pos->x = x;

    if (y == POS_CENTERED)
        pos->y = DEFAULT_H / 2 - pos->h / 2;
    else
        pos->y = y;
}

void init_position_float(int x, int y, SDL_Texture *texture, SDL_FRect *pos)
{
    int w = 0;
    int h = 0;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    pos->w = w;
    pos->h = h;

    if (x == POS_CENTERED)
        pos->x = DEFAULT_W / 2 - pos->w / 2;
    else
        pos->x = x;

    if (y == POS_CENTERED)
        pos->y = DEFAULT_H / 2 - pos->h / 2;
    else
        pos->y = y;
}

void error(const char *title, const char *text, SDL_Window *window, SDL_Renderer *renderer)
{
    // Destroy renderer BEFORE window
    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (window)
        SDL_DestroyWindow(window);

    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL);
    exit(EXIT_FAILURE);
}


void *xmalloc(size_t size, SDL_Window *window, SDL_Renderer *renderer)
{
    void *ptr = malloc(size);

    if (!ptr)
        error("malloc failed", "Memory allocation failed.", window, renderer);

    return ptr;
}

void *xcalloc(size_t nmenb, size_t size, SDL_Window *window, SDL_Renderer *renderer)
{
    void *ptr = calloc(nmenb, size);

    if (!ptr)
        error("calloc failed", "Memory allocation failed.", window, renderer);

    return ptr;
}


void *xrealloc(void *ptr, size_t size, SDL_Window *window, SDL_Renderer *renderer)
{
    void *p = realloc(ptr, size);

    if (!p)
        error("realloc failed", "Memory allocation failed.", window, renderer);

    return p;
}


SDL_Texture *get_text_texture(struct window *window, TTF_Font *font,
                              const char *text, SDL_Color fg)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, fg);

    if (!surface)
        error("Could not create text surface", TTF_GetError(), window->window, window->renderer);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(window->renderer, surface);

    // Texture can be NULL if SDL_CreateTextureFromSurface fails

    SDL_FreeSurface(surface);
    return texture;
}


void render_text(struct window *window, TTF_Font *font, const char *text, SDL_Color fg,
                 int x, int y)
{
    SDL_Texture *texture = get_text_texture(window, font, text, fg);

    if (!texture)
        return;

    int w = 0;
    int h = 0;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);

    if (x == POS_CENTERED)
        x = DEFAULT_W / 2 - w / 2;

    if (y == POS_CENTERED)
        y = DEFAULT_H / 2 - h / 2;

    SDL_Rect pos_dst = { .x = x, .y = y, .w = w, .h = h };

    resize_pos_for_resolution(window, &pos_dst);

    SDL_RenderCopy(window->renderer, texture, NULL, &pos_dst);

    SDL_DestroyTexture(texture);
}


TTF_Font *load_font(struct window *window, const char *filename,
                    int pt_size)
{
    TTF_Font *font = TTF_OpenFont(filename, pt_size);

    if (!font)
        error("Could not load font", TTF_GetError(), window->window, window->renderer);

    return font;
}


Mix_Chunk *load_sound(struct window *window, const char *filename)
{
    render_loading_screen(window);

    Mix_Chunk *sound = Mix_LoadWAV(filename);

    if (!sound)
        error("Could not load sound", Mix_GetError(), window->window, window->renderer);

    return sound;
}


void resize_pos_for_resolution(struct window *window, SDL_Rect *pos)
{
     pos->x = (pos->x * window->w) / DEFAULT_W;
     pos->y = (pos->y * window->h) / DEFAULT_H;
     pos->w = (pos->w * window->w) / DEFAULT_W;
     pos->h = (pos->h * window->h) / DEFAULT_H;
}

void resize_pos_for_resolution_float(struct window *window, SDL_FRect *pos)
{
    pos->x = (pos->x * window->w) / DEFAULT_W;
    pos->y = (pos->y * window->h) / DEFAULT_H;
    pos->w = (pos->w * window->w) / DEFAULT_W;
    pos->h = (pos->h * window->h) / DEFAULT_H;
}
