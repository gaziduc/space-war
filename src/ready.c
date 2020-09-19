#include "init.h"
#include "utils.h"
#include "event.h"
#include "game.h"
#include "menu.h"

static void render_ok(struct window *window, int selected_level,
                      int selected_difficulty, Uint32 begin)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color white = { 255, 255, 255, alpha };
    SDL_Color orange = { 255, 128, 0, alpha };
    SDL_Color purple = { 192, 0, 192, alpha };

    render_text(window, window->fonts->zero4b_30_small, "SUMMARY", orange, 150, 150);

    char str[128] = { 0 };
    sprintf(str, "Mode: %d Player%s %s", window->num_players,
                                   window->num_players == 2 ? "s" : "",
                                   window->is_lan ? "(LAN)" :
                                   window->num_players == 2 ? "(Local)" : "");

    render_text(window, window->fonts->zero4b_30_extra_small, str,
                white, 150, 300);

    sprintf(str, "Mission: %d.%d", selected_level, window->num_players);
    render_text(window, window->fonts->zero4b_30_extra_small, str,
                white, 150, 370);

    switch (selected_difficulty)
    {
        case 0:
            render_text(window, window->fonts->zero4b_30_extra_small, "Difficulty: Easy", white, 150, 440);
            break;

        case 1:
            render_text(window, window->fonts->zero4b_30_extra_small, "Difficulty: Hard", white, 150, 440);
            break;

        case 2:
            render_text(window, window->fonts->zero4b_30_extra_small, "Difficulty: Really Hard", white, 150, 440);
            break;

        default:
            break;
    }

    render_text(window, window->fonts->zero4b_30_extra_small, "Weapon:",
                white, 150, 510);

    SDL_Rect pos = { .x = 400, .y = 523, .w = 0, .h = 0 };
    SDL_QueryTexture(window->img->shot[window->weapon]->texture, NULL, NULL, &pos.w, &pos.h);

    resize_pos_for_resolution(window, &pos);

    SDL_RenderCopy(window->renderer, window->img->shot[window->weapon]->texture,
                   NULL, &pos);

    render_text(window, window->fonts->zero4b_30_small, "Press enter!",
                purple, 150, 750);
}



void ready(struct window *window, int selected_level, int selected_difficulty)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);

        if (handle_play_event(window))
        {
            if (window->is_lan)
            {
                char data[4] = { selected_level, selected_difficulty, window->weapon, 0 };
                SDLNet_TCP_Send(window->client, data, sizeof(data));
            }

            play_game(window, selected_level, selected_difficulty);
            return;
        }

        escape = handle_escape_event(window);

        // Display black bachground
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_ok(window, selected_level, selected_difficulty, begin);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }

}
