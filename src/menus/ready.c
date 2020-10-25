#include "init.h"
#include "utils.h"
#include "event.h"
#include "game.h"
#include "menu.h"
#include <stdio.h>

static void render_ok(struct window *window, int selected_level,
                      int selected_difficulty, Uint32 begin, const char *mission_name)
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
                                   window->is_lan ? "(Network)" :
                                   window->num_players == 2 ? "(Local)" : "");

    render_text(window, window->fonts->zero4b_30_extra_small, str,
                white, 150, 300);

    if (selected_level != NUM_LEVELS + 1)
        sprintf(str, "Mission: %d.%d - %s", selected_level, window->num_players, mission_name);
    else
        sprintf(str, "Mission: Arcade Mode");

    render_text(window, window->fonts->zero4b_30_extra_small, str,
                white, 150, 370);

    switch (selected_difficulty)
    {
        case 1:
            render_text(window, window->fonts->zero4b_30_extra_small, "Difficulty: Easy", white, 150, 440);
            break;

        case 2:
            render_text(window, window->fonts->zero4b_30_extra_small, "Difficulty: Hard", white, 150, 440);
            break;

        case 3:
            render_text(window, window->fonts->zero4b_30_extra_small, "Difficulty: Really Hard", white, 150, 440);
            break;

        default:
            error("Unknown difficulty", "Unknown difficulty level", window->window, window->renderer);
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



int ready(struct window *window, int selected_level, int selected_difficulty, const char *str)
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
            play_game(window, selected_level, selected_difficulty);
            return 1;
        }

        escape = handle_escape_event(window);

        // Display black bachground
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_ok(window, selected_level, selected_difficulty, begin, str);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }

    return 0;
}
