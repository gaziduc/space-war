#include "init.h"
#include "game.h"
#include "event.h"
#include "menu.h"
#include "utils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>

static int handle_play_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_RETURN])
    {
        window->in->key[SDL_SCANCODE_RETURN] = 0;

        play_game(window);

        return 1;
    }

    return 0;
}


static void render_galaxy(struct window *window)
{
    (void) window;

    return;
}


static void render_level_texts(struct window *window, Uint32 begin)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };

    render_text(window, window->fonts->zero4b_30_small, "Mission 1.1", blue, POS_CENTERED, 900);
}


void select_level(struct window *window)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in);
        handle_quit_event(window);
        if (handle_play_event(window))
            begin = SDL_GetTicks();
        escape = handle_escape_event(window);

        // Display black bachground
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_galaxy(window);
        render_level_texts(window, begin);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}
