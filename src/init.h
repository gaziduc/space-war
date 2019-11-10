#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>

struct textures
{
    SDL_Texture *ship;
    SDL_Texture *shot;
    SDL_Texture *bg;
    SDL_Texture *enemy;
    SDL_Texture *explosion;
    SDL_Texture *trail;
};

struct input
{
    char key[SDL_NUM_SCANCODES];
    char quit;
};

struct list
{
    SDL_Rect pos_src;
    SDL_Rect pos_dst;
    struct list *next;
};

enum list_type
{
    SHOTS_LIST = 0,
    ENEMY_LIST,
    EXPLOSION_LIST,
    NUM_LISTS
};

struct window
{
    SDL_Window *window;
    int w;
    int h;
    SDL_Renderer *renderer;
    struct textures *img;
    struct input *in;
    FPSmanager *fps;
    struct list *list[NUM_LISTS];
};

struct window *init_all(int width, int height);

#endif /* !INIT_H */
