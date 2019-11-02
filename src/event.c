#include "event.h"
#include "init.h"
#include "free.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>

void update_events(struct input *in)
{
    in->quit = 0;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            in->quit = 1;
            break;
        case SDL_KEYDOWN:
            in->key[event.key.keysym.scancode] = 1;
            break;
        case SDL_KEYUP:
            in->key[event.key.keysym.scancode] = 0;
            break;
        default:
            break;
        }
    }
}

void handle_quit_event(struct window *window)
{
    if (window->in->quit || window->in->key[SDL_SCANCODE_ESCAPE])
    {
        free_all(window);
        exit(EXIT_SUCCESS);
    }
}
