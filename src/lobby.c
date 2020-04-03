#include "init.h"
#include "event.h"
#include "utils.h"
#include "lobby.h"
#include "game.h"
#include "menu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

int selected;
int quit;
int level_num;
int level_difficulty;

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

    SDL_CreateThread(waiting_thread, "waiting_thread", window);

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);
        escape = handle_escape_event(window);

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        if (selected)
        {
            if (quit)
                escape = 1;
            else
            {
                play_game(window, level_num, level_difficulty);
                selected = 0;
                SDL_CreateThread(waiting_thread, "waiting_thread", window);
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


int waiting_thread(void *data)
{
    struct window *window = data;
    char data_received[3] = { 0 };

    SDLNet_TCP_Recv(window->client, data_received, sizeof(data_received));

    level_num = data_received[0];
    level_difficulty = data_received[1];
    quit = data_received[2];

    selected = 1;

    return 0;
}
