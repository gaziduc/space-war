#include "init.h"
#include "game.h"
#include "event.h"
#include "menu.h"
#include "utils.h"
#include "level.h"
#include "weapon.h"
#include "net.h"
#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>


static void render_selected_level_title(struct window *window, const char *s, Uint32 alpha, int score)
{
    SDL_Color orange = { .r = 255, .g = 127, .b = 39, .a = alpha };

    SDL_Texture *texture = get_text_texture(window, window->fonts->zero4b_30_small,
                                            s, orange);

    if (texture)
    {
        SDL_Rect pos = { .x = 1300, .y = 320, .w = 0, .h = 0 };
        SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
        pos.x -= pos.w / 2;

        resize_pos_for_resolution(window, &pos);

        SDL_RenderCopy(window->renderer, texture, NULL, &pos);
        SDL_DestroyTexture(texture);
    }


    SDL_Color yellow = { .r = 255, .g = 255, .b = 0, .a = alpha };

    // Render score
    char str[50] = { 0 };
    sprintf(str, window->txt[BEST_D], score);
    texture = get_text_texture(window, window->fonts->zero4b_30_small, str, yellow);

    if (texture)
    {
        SDL_Rect pos_score = { .x = 1300, .y = 420, .w = 0, .h = 0 };
        SDL_QueryTexture(texture, NULL, NULL, &pos_score.w, &pos_score.h);
        pos_score.x -= pos_score.w / 2;

        resize_pos_for_resolution(window, &pos_score);

        SDL_RenderCopy(window->renderer, texture, NULL, &pos_score);
        SDL_DestroyTexture(texture);
    }
}



static void render_level_difficulties(struct window *window, Uint32 begin,
                                      int level, int selected_difficulty)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };
    SDL_Color green = { .r = GREEN_R, .g = GREEN_G, .b = GREEN_B, .a = alpha };

    char *s_list[NUM_DIFFICULTIES + 1] = { window->txt[EASY__], window->txt[HARD__], window->txt[REALLY_HARD__], window->txt[BACK_3] };

    for (int i = 1; i <= NUM_DIFFICULTIES; i++)
    {
        int y = 360 + (i - 1) * 100;
        char s[100] = { 0 };

        if (window->save->progress[window->num_players - 1][level - 1] < i)
        {
            if (i != selected_difficulty)
                snprintf(s, strlen(s_list[i - 1]) - 1, "%s", s_list[i - 1]);
            else
                snprintf(s, strlen(s_list[i - 1]) - 1, "%s", s_list[i - 1]);
        }
        else
        {
            if (i != selected_difficulty)
                strcpy(s, s_list[i - 1]);
            else
                strcpy(s, s_list[i - 1]);
        }

        render_text(window, window->fonts->zero4b_30_small, s,
                    i != selected_difficulty ? blue : green, 150, y);
    }

    render_text(window, window->fonts->zero4b_30_small,
                s_list[NUM_DIFFICULTIES],
                selected_difficulty == NUM_DIFFICULTIES + 1 ? green : blue, 150, 360 + NUM_DIFFICULTIES * 100);

    SDL_Color white = { 195, 195, 195, alpha };

    switch (selected_difficulty)
    {
        case EASY:
            render_text(window, window->fonts->craft_large,
                        window->txt[EASY_CONDITIONS], white, 150, 880);
            break;

        case HARD:
            render_text(window, window->fonts->craft_large,
                        window->txt[HARD_CONDITIONS], white, 150, 880);
            break;

        case REALLY_HARD:
            render_text(window, window->fonts->craft_large,
                        window->txt[REALLY_HARD_CONDITIONS], white, 150, 880);
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
        sprintf(s, window->txt[ARCADE_MODE]);
    else
        sprintf(s, window->txt[MISSION_D_D___S], selected_level, window->num_players, str);

    SDL_Rect areas[NUM_DIFFICULTIES + 1];

    for (unsigned i = 0; i < NUM_DIFFICULTIES + 1; i++)
    {
        areas[i].x = 150;
        areas[i].y = 360 + i * 100;
        TTF_SizeText(window->fonts->zero4b_30_small, window->txt[EASY__ + i], &areas[i].w, &areas[i].h);
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
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
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);

        Uint32 alpha = SDL_GetTicks() - begin;

        if (alpha > TITLE_ALPHA_MAX)
            alpha = TITLE_ALPHA_MAX;
        else if (alpha == 0)
            alpha = 1;

        SDL_Color orange = { 255, 127, 39, alpha };

        render_text(window, window->fonts->zero4b_30_extra_small, s, orange, 150, 150);
        render_text(window, window->fonts->zero4b_30_extra_small, window->txt[CHOOSE_DIFFICULTY], orange, 150, 225);
        render_level_difficulties(window, begin, selected_level, selected_difficulty);
        render_controller_input_texts(window, begin, 1);
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

    SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };
    SDL_Color green = { .r = GREEN_R, .g = GREEN_G, .b = GREEN_B, .a = alpha };
    SDL_Color orange = { 255, 127, 39, alpha };
    SDL_Color grey = { 128, 128, 128, alpha };

    render_text(window, window->fonts->zero4b_30_small, window->txt[SELECT_MISSION], orange, 150, 150);

    for (int i = 1; i <= NUM_LEVELS + 1; i++)
    {
        char s[50] = { 0 };

        if (i != NUM_LEVELS + 1)
            sprintf(s, window->txt[MISSION_D_D_S], i, window->num_players, window->save->progress[window->num_players - 1][i - 1], "***");
        else
            sprintf(s, window->txt[ARCADE_MODE_S], window->save->progress[window->num_players - 1][i - 1], "***");


        int y = 290 + (i - 1) * 60;

        if (i != selected_level)
        {
            if (i == 1 || window->save->progress[window->num_players - 1][i - 2] > 0)
                render_text(window, window->fonts->zero4b_30_extra_small, s, blue, 150, y);
            else
                render_text(window, window->fonts->zero4b_30_extra_small, s, grey, 150, y);
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

    render_text(window, window->fonts->zero4b_30_extra_small,
                window->txt[BACK_2],
                selected_level == NUM_LEVELS + 2 ? green : blue,
                150, 290 + (NUM_LEVELS + 1) * 60);
}


void select_level(struct window *window)
{
    int escape = 0;
    unsigned selected_level = 0;
    Uint32 begin = SDL_GetTicks();

    char *s_list[NUM_LEVELS + 1] = { window->txt[THE_MILKY_WAY],
                                 window->txt[ANDROMEDA_GALAXY],
                                 window->txt[HYPERSPACE],
                                 window->txt[NEW_UNIVERSE],
                                 window->txt[SPATIAL_ARMY],
                                 window->txt[BOSSES],
                                 window->txt[NEW_PLANET],
                                 window->txt[SPACE_TUNNEL],
                                 window->txt[THE_END],
                                 window->txt[ARCADE_MODE]
                               };

    SDL_Rect areas[NUM_LEVELS + 2];
    for (unsigned i = 0; i < NUM_LEVELS + 2; i++)
    {
        areas[i].x = 150;
        areas[i].y = 290 + i * 60;
        char s[100] = { 0 };

        if (i < NUM_LEVELS)
            sprintf(s, window->txt[MISSION_D_D_S], i + 1, window->num_players, window->save->progress[window->num_players - 1][i], "***");
        else if (i == NUM_LEVELS)
            sprintf(s, window->txt[ARCADE_MODE_S], window->save->progress[window->num_players - 1][i], "***");
        else
            sprintf(s, window->txt[BACK_2]);

        TTF_SizeText(window->fonts->zero4b_30_extra_small, s, &areas[i].w, &areas[i].h);
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
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
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_level_texts(window, begin, selected_level, s_list);
        render_controller_input_texts(window, begin, 1);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}
