#include "init.h"
#include "event.h"
#include "utils.h"
#include "lobby.h"
#include "game.h"
#include "menu.h"
#include "net.h"
#include "ready.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>


void lobby(struct window *window)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        int res = handle_messages(window, "LZT");
        if (!res)
            escape = 1;
        else if (res == 2)
        {
            waiting_screen(window, SDL_GetTicks() + window->start_mission_ticks - window->client_time);
            play_game(window);
        }
            

        // Process/Draw all the things
        render_stars(window);
        Uint32 alpha = SDL_GetTicks() - begin;

        if (alpha > TITLE_ALPHA_MAX)
            alpha = TITLE_ALPHA_MAX;
        else if (alpha == 0)
            alpha = 1;

        SDL_Color orange = { 255, 127, 39, alpha };
        render_text(window, window->fonts->zero4b_30_small, window->txt[WAITING_FOR_THE_SERVER],
                    orange, 150, 150);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);

        if (window->client == NULL)
            break;
    }
}
