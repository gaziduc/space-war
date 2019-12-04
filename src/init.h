#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL_ttf.h>

struct textures
{
    SDL_Texture *ship;
    SDL_Texture *shot;
    SDL_Texture *bg;
    SDL_Texture *bg2;
    SDL_Texture *enemy;
    SDL_Texture *explosion;
    SDL_Texture *trail;
    SDL_Texture *enemy_shot;
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
    unsigned long framecount;
    int speed_x;
    struct list *next;
};

enum list_type
{
    MY_SHOTS_LIST = 0,
    ENEMY_LIST,
    EXPLOSION_LIST,
    ENEMY_SHOT_LIST,
    NUM_LISTS
};

struct vector
{
    struct path *data;
    size_t size;
    size_t capacity;
    size_t index;
};

struct fonts
{
    TTF_Font *pixel;
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
    Uint32 last_shot_time;
    struct vector *paths;
    Uint32 last_enemy_time;
    int health;
    struct fonts *fonts;
    int score;
};

struct window *init_all(int width, int height);

#endif /* !INIT_H */
