#include "init.h"
#include "utils.h"
#include "event.h"
#include "game.h"
#include "menu.h"
#include "net.h"
#include <stdio.h>
#include <string.h>

static void render_ok(struct window *window, int selected_level,
                      int selected_difficulty, Uint32 begin, const char *mission_name,
                      unsigned selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color white = { 195, 195, 195, alpha };
    SDL_Color orange = { 255, 127, 39, alpha };
    SDL_Color green = { .r = GREEN_R, .g = GREEN_G, .b = GREEN_B, .a = alpha };
    SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };

    render_text(window, window->fonts->zero4b_30_small, window->txt[SUMMARY], orange, 150, 150);

    char str[128] = { 0 };
    sprintf(str, window->txt[MODE_S], window->is_lan ? window->txt[TWO_PLAYERS_NETWORK] :
                                      window->num_players == 2 ? window->txt[TWO_PLAYERS_LOCAL] : window->txt[ONE_PLAYER]);

    render_text(window, window->fonts->zero4b_30_extra_small, str,
                white, 150, 300);

    if (selected_level != NUM_LEVELS + 1)
        sprintf(str, window->txt[MISSION_D_D___S], selected_level, window->num_players, mission_name);
    else
        sprintf(str, window->txt[ARCADE_MODE]);

    render_text(window, window->fonts->zero4b_30_extra_small, str,
                white, 150, 370);

    snprintf(str, strlen(window->txt[EASY__ + selected_difficulty - 1]) - 1, "%s", window->txt[EASY__+ selected_difficulty - 1]);
    str[strlen(window->txt[EASY__ + selected_difficulty - 1]) - 1] = '\0';

    render_text(window, window->fonts->zero4b_30_extra_small, str, white, 150, 440);

    render_text(window, window->fonts->zero4b_30_extra_small, window->txt[WEAPON],
                white, 150, 510);

    SDL_Rect pos = { .x = 425, .y = 523, .w = 0, .h = 0 };
    SDL_QueryTexture(window->img->shot[window->weapon]->texture, NULL, NULL, &pos.w, &pos.h);

    resize_pos_for_resolution(window, &pos);

    SDL_RenderCopy(window->renderer, window->img->shot[window->weapon]->texture,
                   NULL, &pos);

    render_text(window, window->fonts->zero4b_30_small, window->txt[START_MISSION],
                selected_item == 1 ? green : blue, 150, 750);

    render_text(window, window->fonts->zero4b_30_small, window->txt[BACK_5],
                selected_item == 2 ? green : blue, 150, 850);
}



void waiting_screen(struct window *window, Uint32 start_mission_ticks)
{
    SDL_Color white = { 255, 255, 255, 255 };

    while (SDL_GetTicks() < start_mission_ticks)
    {
        // Display black bachground
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);

        char s[8] = { 0 };
        sprintf(s, "%d", (start_mission_ticks - SDL_GetTicks()) / 1000 + 1);
        render_text(window, window->fonts->zero4b_30, s, white, POS_CENTERED, POS_CENTERED);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);
    }
}


int ready(struct window *window, int selected_level, int selected_difficulty, const char *str)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();
    unsigned selected_item = 0;
    SDL_Rect areas[2];

    for (unsigned i = 0; i < 2; i++)
    {
        areas[i].x = 150;
        areas[i].y = 750 + i * 100;
        TTF_SizeText(window->fonts->zero4b_30_small, window->txt[START_MISSION + i], &areas[i].w, &areas[i].h);
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &selected_item, 2, areas);

        if (selected_item > 0 && handle_play_event(window))
        {
            if (selected_item == 1)
            {
                if (window->is_lan)
                {
                    struct msg msg = { .type =  LEVEL_MSG };
                    msg.content.lvl.level_num = selected_level;
                    msg.content.lvl.level_difficulty = selected_difficulty;
                    msg.content.lvl.weapon = window->weapon;
                    Uint32 ticks = SDL_GetTicks() + 3000;
                    msg.content.lvl.start_mission_ticks = ticks; // Start mission in 3 sec
                    send_msg(window, &msg);

                    waiting_screen(window, ticks);

                }

                play_game(window, selected_level, selected_difficulty);
                return 1;
            }
            else
                escape = 1;
        }

        escape = escape || handle_escape_event(window);

        // Display black bachground
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_ok(window, selected_level, selected_difficulty, begin, str, selected_item);
        render_controller_input_texts(window, begin, 1);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);
    }

    return 0;
}
