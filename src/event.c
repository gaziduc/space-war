#include "event.h"
#include "init.h"
#include "free.h"
#include <SDL2/SDL.h>

void update_events(struct input *in)
{
    in->quit = 0;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        // Quit event
        case SDL_QUIT:
            in->quit = 1;
            break;

        // Keyboard events
        case SDL_KEYDOWN:
            in->key[event.key.keysym.scancode] = 1;
            break;
        case SDL_KEYUP:
            in->key[event.key.keysym.scancode] = 0;
            break;

        // Controller events
        case SDL_CONTROLLERBUTTONDOWN:
            in->c.c_button[event.cbutton.button] = 1;
            break;

        case SDL_CONTROLLERBUTTONUP:
            in->c.c_button[event.cbutton.button] = 0;
            break;

        default:
            break;
        }
    }
}

void handle_quit_event(struct window *window, int is_in_level)
{
    if (window->in->quit)
    {
        free_all(window, is_in_level);
        exit(EXIT_SUCCESS);
    }
}

int handle_escape_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_ESCAPE]
        || window->in->c.c_button[SDL_CONTROLLER_BUTTON_BACK])
    {
        window->in->key[SDL_SCANCODE_ESCAPE] = 0;
        window->in->c.c_button[SDL_CONTROLLER_BUTTON_BACK] = 0;

        return 1;
    }

    return 0;
}
