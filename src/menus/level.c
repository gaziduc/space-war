#include "init.h"
#include "game.h"
#include "event.h"
#include "menu.h"
#include "utils.h"
#include "level.h"
#include "weapon.h"
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>


static void render_selected_level_title(struct window *window, const char *s, Uint32 alpha, int score)
{
    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

    SDL_Texture *texture = get_text_texture(window, window->fonts->zero4b_30_small,
                                            s, orange);
    SDL_Rect pos = { .x = 1300, .y = 320, .w = 0, .h = 0 };
    SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
    pos.x -= pos.w / 2;

    resize_pos_for_resolution(window, &pos);

    SDL_RenderCopy(window->renderer, texture, NULL, &pos);
    SDL_DestroyTexture(texture);


    SDL_Color yellow = { .r = 255, .g = 255, .b = 0, .a = alpha };

    // Render score
    char str[50] = { 0 };
    sprintf(str, "Best: %d", score);
    texture = get_text_texture(window, window->fonts->zero4b_30_small, str, yellow);
    SDL_Rect pos_score = { .x = 1300, .y = 420, .w = 0, .h = 0 };
    SDL_QueryTexture(texture, NULL, NULL, &pos_score.w, &pos_score.h);
    pos_score.x -= pos_score.w / 2;

    resize_pos_for_resolution(window, &pos_score);

    SDL_RenderCopy(window->renderer, texture, NULL, &pos_score);
    SDL_DestroyTexture(texture);
}



static void render_level_difficulties(struct window *window, Uint32 begin,
                                      int level, int selected_difficulty)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { 0, 255, 255, alpha };
    SDL_Color green = { 0, 255, 0, alpha };

    char *s_list[NUM_DIFFICULTIES + 1] = { "-> Easy *", "-> Hard *", "-> Really Hard *", "-> Back" };

    for (int i = 1; i <= NUM_DIFFICULTIES; i++)
    {
        int y = 360 + (i - 1) * 80;
        char s[100] = { 0 };

        if (window->save->progress[window->num_players - 1][level - 1] < i)
        {
            if (i != selected_difficulty)
                snprintf(s, strlen(s_list[i - 1] + 3) - 1, "%s", s_list[i - 1] + 3);
            else
                snprintf(s, strlen(s_list[i - 1]) - 1, "%s", s_list[i - 1]);
        }
        else
        {
            if (i != selected_difficulty)
                strcpy(s, s_list[i - 1] + 3);
            else
                strcpy(s, s_list[i - 1]);
        }

        render_text(window, window->fonts->zero4b_30_small, s,
                    i != selected_difficulty ? blue : green, 150, y);
    }

    render_text(window, window->fonts->zero4b_30_small,
                selected_difficulty == NUM_DIFFICULTIES + 1 ? s_list[NUM_DIFFICULTIES] : s_list[NUM_DIFFICULTIES] + 3,
                selected_difficulty == NUM_DIFFICULTIES + 1 ? green : blue, 150, 360 + NUM_DIFFICULTIES * 80);

    SDL_Color white = { 255, 255, 255, alpha };

    switch (selected_difficulty)
    {
        case EASY:
            render_text(window, window->fonts->zero4b_30_extra_small,
                        "20 HP - 2 Bombs - 999+ Ammo - 0 Bonus", white, 150, 760);
            break;
        case HARD:
            render_text(window, window->fonts->zero4b_30_extra_small,
                        "7 HP - 1 Bomb - 999+ Ammo - 1000 Bonus", white, 150, 760);
            break;

        case REALLY_HARD:
            render_text(window, window->fonts->zero4b_30_extra_small,
                        "3 HP - 1 Bomb - 200 Ammo - 3000 Bonus", white, 150, 760);
            break;

        default:
            break;
    }
}


static void level_difficulty(struct window *window, int selected_level, const char *str)
{
    int escape = 0;
    unsigned selected_difficulty = 0;
    Uint32 begin = SDL_GetTicks();

    char s[50] = { 0 };

    if (selected_level == NUM_LEVELS + 1)
        sprintf(s, "Arcade Mode");
    else
        sprintf(s, "Mission %d.%d - %s", selected_level, window->num_players, str);

    SDL_Rect areas[NUM_DIFFICULTIES + 1];

    for (unsigned i = 0; i < NUM_DIFFICULTIES + 1; i++)
    {
        areas[i].x = 150;
        areas[i].y = 360 + i * 80;
        areas[i].w = 1400;
        areas[i].h = 80;
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);

        if (selected_difficulty > 0 && handle_play_event(window))
        {
            if (selected_difficulty <= NUM_DIFFICULTIES)
            {
                choose_weapons(window, selected_level, selected_difficulty, str);
                begin = SDL_GetTicks();
            }
            else if (selected_difficulty == NUM_DIFFICULTIES + 1)
                escape = 1;
        }

        handle_select_arrow_event(window, &selected_difficulty, NUM_DIFFICULTIES + 1, areas);
        escape = escape || handle_escape_event(window);

        // Display black bachground
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);

        Uint32 alpha = SDL_GetTicks() - begin;

        if (alpha > TITLE_ALPHA_MAX)
            alpha = TITLE_ALPHA_MAX;
        else if (alpha == 0)
            alpha = 1;

        SDL_Color orange = { 255, 128, 0, alpha };

        render_text(window, window->fonts->zero4b_30_small, s, orange, 150, 150);
        render_level_difficulties(window, begin, selected_level, selected_difficulty);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}


static void render_level_texts(struct window *window, Uint32 begin, int selected_level,
                               char **s_list)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };
    SDL_Color green = { .r = 0, .g = 255, .b = 0, .a = alpha };
    SDL_Color orange = { 255, 128, 0, alpha };
    SDL_Color grey = { 128, 128, 128, alpha };

    render_text(window, window->fonts->zero4b_30_small, "MISSION SELECT", orange, 150, 150);

    for (int i = 1; i <= NUM_LEVELS + 1; i++)
    {
        char s[50] = { 0 };

        if (i != NUM_LEVELS + 1)
            sprintf(s, "-> Mission %d.%d %.*s", i, window->num_players, window->save->progress[window->num_players - 1][i - 1], "***");
        else
            sprintf(s, "-> Arcade Mode %.*s", window->save->progress[window->num_players - 1][i - 1], "***");


        int y = 320 + (i - 1) * 60;

        if (i != selected_level)
        {
            if (i == 1 || window->save->progress[window->num_players - 1][i - 2] > 0)
                render_text(window, window->fonts->zero4b_30_extra_small, s + 3, blue, 150, y);
            else
                render_text(window, window->fonts->zero4b_30_extra_small, s + 3, grey, 150, y);
        }
        else
        {
            if (i == 1 || window->save->progress[window->num_players - 1][i - 2] > 0)
                render_text(window, window->fonts->zero4b_30_extra_small, s, green, 150, y);
            else
                render_text(window, window->fonts->zero4b_30_extra_small, s, grey, 150, y);
        }
    }

    if (selected_level > 0 && selected_level <= NUM_LEVELS + 1)
        render_selected_level_title(window, s_list[selected_level - 1],
                                    alpha, window->save->score[window->num_players - 1][selected_level - 1]);

    char *back = "-> Back";

    render_text(window, window->fonts->zero4b_30_extra_small,
                selected_level == NUM_LEVELS + 2 ? back : back + 3,
                selected_level == NUM_LEVELS + 2 ? green : blue,
                150, 320 + (NUM_LEVELS + 1) * 60);
}


void select_level(struct window *window)
{
    int escape = 0;
    unsigned selected_level = 0;
    Uint32 begin = SDL_GetTicks();

    char *s_list[NUM_LEVELS + 1] = { "The Milky Way",
                                 "Andromeda Galaxy",
                                 "Hyperspace",
                                 "New Universe",
                                 "Spatial Army",
                                 "Bosses",
                                 "New Planet",
                                 "Space Tunnel",
                                 "The End",
                                 "Arcade Mode",
                               };

    SDL_Rect areas[NUM_LEVELS + 2];
    for (unsigned i = 0; i < NUM_LEVELS + 2; i++)
    {
        areas[i].x = 150;
        areas[i].y = 320 + i * 60;
        areas[i].w = 600;
        areas[i].h = 60;
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);

        if (selected_level > 0 && handle_play_event(window))
        {
            if (selected_level <= NUM_LEVELS + 1)
            {
                if (selected_level == 1 || window->save->progress[window->num_players - 1][selected_level - 2] > 0)
                {
                    level_difficulty(window, selected_level, s_list[selected_level - 1]);
                    begin = SDL_GetTicks();
                }
            }
            else if (selected_level == NUM_LEVELS + 2)
                escape = 1;
        }

        handle_select_arrow_event(window, &selected_level, NUM_LEVELS + 2, areas);
        escape = escape || handle_escape_event(window);

        // Display black bachground
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_level_texts(window, begin, selected_level, s_list);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }

    if (window->is_lan)
    {
         char data[15] = { 0 };
         data[11] = 2;
         SDLNet_TCP_Send(window->client, data, sizeof(data));
    }
}
