#include "init.h"
#include "event.h"
#include "utils.h"
#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>

static void handle_arrow_event(struct window *window, SDL_Rect *pos)
{
    // Move ship
    if (window->in->key[SDL_SCANCODE_UP])
        pos->y -= SHIP_SPEED;

    if (window->in->key[SDL_SCANCODE_DOWN])
        pos->y += SHIP_SPEED;

    if (window->in->key[SDL_SCANCODE_LEFT])
        pos->x -= SHIP_SPEED;

    if (window->in->key[SDL_SCANCODE_RIGHT])
        pos->x += SHIP_SPEED;

    // Prevent out of bounds
    if (pos->y < 0)
        pos->y = 0;
    else if (pos->y > window->h - pos->h)
        pos->y = window->h - pos->h;

    if (pos->x < 0)
        pos->x = 0;
    else if (pos->x > window->w - pos->w)
        pos->x = window->w - pos->w;
}

void play_game(struct window *window)
{
    SDL_Rect pos;
    init_position(120, POS_CENTERED, window, window->img->ship, &pos);
    SDL_Rect pos_fs = { .x = 0, .y = 0, .w = window->w, .h = window->h };

    int escape = 0;

    while (!escape)
    {
        // Get and handle events
        update_events(window->in);
        handle_quit_event(window);
        handle_arrow_event(window, &pos);

        // Display textures
        SDL_RenderClear(window->renderer);
        SDL_RenderCopy(window->renderer, window->img->bg, NULL, &pos_fs);
        SDL_RenderCopy(window->renderer, window->img->ship, NULL, &pos);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}
