#include "init.h"
#include "event.h"
#include "utils.h"
#include "lobby.h"
#include "game.h"
#include "menu.h"
#include "net.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

int selected;
int quit;

static void reset_global_vars()
{
    selected = 0;
    quit = 0;
}

void lobby(struct window *window)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();
    reset_global_vars();

    SDL_CreateThread(waiting_thread_client, "waiting_thread_client", window);

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);
        escape = handle_escape_event(window);

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        if (selected)
        {
            if (quit)
                escape = 1;
            else
            {
                play_game(window, window->state.level_num, window->state.level_difficulty);
                selected = 0;
            }
        }

        // Process/Draw all the things
        render_stars(window);
        Uint32 alpha = SDL_GetTicks() - begin;

        if (alpha > TITLE_ALPHA_MAX)
            alpha = TITLE_ALPHA_MAX;
        else if (alpha == 0)
            alpha = 1;

        SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };
        render_text(window, window->fonts->zero4b_30_small, "WAITING FOR THE SERVER...",
                    orange, 150, 150);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}


int waiting_thread_client(void *data)
{
    struct window *window = data;

    do
    {
        recv_state(window, &window->state);

        if (window->state.state == 1) // If server plays a level
            selected = 1;

    } while (window->state.state != 2); // while (!quit)

    selected = 1;
    quit = 1;

    return 0;
}
