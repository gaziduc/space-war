#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>

struct textures
{
    SDL_Texture *ship;
    SDL_Texture *bg;
};

struct input
{
    char key[SDL_NUM_SCANCODES];
    char quit;
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
};

struct window *init_all(int width, int height);

#endif /* !INIT_H */
