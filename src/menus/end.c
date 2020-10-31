#include "event.h"
#include "utils.h"
#include "menu.h"
#include "save.h"
#include "net.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>


static void render_success_texts(struct window *window, Uint32 begin, int is_best, unsigned selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color green = { .r = 0, .g = 255, .b = 0, .a = alpha };
    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };

    render_text(window, window->fonts->zero4b_30, window->txt[SUCCESS], green,
                150, 150);

    // Print score
    char s[50] = { 0 };
    sprintf(s, window->txt[SCORE_D_2], window->score);

    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30_extra_small, s, orange,
                150, 400);

    // Health Bonus
    int health_bonus = 0;

    for (unsigned i = 0; i < window->num_players; i++)
        health_bonus += window->player[i].health;

    sprintf(s, window->txt[HEALTH_BONUS_D], health_bonus);

    render_text(window, window->fonts->zero4b_30_extra_small, s, orange,
                150, 450);

    // Bombs Bonus
    int bombs_bonus = window->num_bombs * 100;
    sprintf(s, window->txt[BOMBS_BONUS_D], bombs_bonus);

    render_text(window, window->fonts->zero4b_30_extra_small, s, orange,
                150, 500);

    // Difficulty bonus
    sprintf(s, window->txt[DIFFICULTY_BONUS_D], window->bonus);
    render_text(window, window->fonts->zero4b_30_extra_small, s, orange,
                150, 550);

    // Total
    sprintf(s, window->txt[TOTAL_D], window->score + health_bonus + bombs_bonus + window->bonus);

    render_text(window, window->fonts->zero4b_30_small, s, orange,
                150, 620);

    if (is_best)
    {
        SDL_Color yellow = { .r = 255, .g = 255, .b = 0, .a = alpha };
        render_text(window, window->fonts->zero4b_30_small, window->txt[NEW_BEST], yellow, 1000, 620);
    }


    // Enter to continue
    if (!window->is_lan || window->server)
        render_text(window, window->fonts->zero4b_30_small, window->txt[CONTINUE],
                    selected_item == 1 ? green : blue, 150, 810);
    else
        render_text(window, window->fonts->zero4b_30_small, window->txt[WAITING_FOR_THE_SERVER],
                    orange, 150, 810);
}



void success(struct window *window, const int level_num, const int difficulty)
{
    if (window->save->progress[window->num_players - 1][level_num - 1] < difficulty)
        window->save->progress[window->num_players - 1][level_num - 1] = difficulty;

    int final_score = window->score + window->num_bombs * 100 + window->bonus;
    for (unsigned i = 0; i < window->num_players; i++)
        final_score += window->player[i].health;

    int is_best = 0;

    if (window->save->score[window->num_players - 1][level_num - 1] < final_score)
    {
        window->save->score[window->num_players - 1][level_num - 1] = final_score;
        is_best = 1;
    }

    // Save
    write_save(window, window->save);

    Uint32 begin = SDL_GetTicks();
    int escape = 0;
    unsigned selected = 0;
    SDL_Rect areas[1];
    areas[0].x = 150;
    areas[0].y = 810;
    TTF_SizeText(window->fonts->zero4b_30_small, window->txt[CONTINUE], &areas[0].w, &areas[0].h);

    load_music(window, "data/success.ogg", 1);

    while (!escape)
    {
        update_events(window->in, window, 0);
        handle_quit_event(window, 1);

        if (!window->is_lan || window->server)
        {
            escape = selected > 0 && handle_play_event(window);
            handle_select_arrow_event(window, &selected, 1, areas);

            if (escape && window->server)
            {
                struct msg msg = { .type = MENU_MSG };
                send_msg(window, &msg);
            }
        }
        else
        {
            handle_messages(window, "M");

            if (window->restart > 0)
                break;
        }

        SDL_RenderClear(window->renderer);

        // Display black bg
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        render_stars(window);
        render_success_texts(window, begin, is_best, selected);

        SDL_RenderPresent(window->renderer);

        SDL_framerateDelay(window->fps);
    }

    window->restart = 0;
}


static void render_failure_texts(struct window *window, Uint32 begin, int selected, int is_best)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color red = { .r = 255, .g = 0, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30, window->txt[FAILURE], red,
                150, 150);

    char s[50] = { 0 };
    sprintf(s, window->txt[SCORE_D_2], window->score);

    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30_small, s, orange,
                150, 450);

    if (is_best)
    {
        SDL_Color yellow = { .r = 255, .g = 255, .b = 0, .a = alpha };
        render_text(window, window->fonts->zero4b_30_small, window->txt[NEW_BEST], yellow, 150, 550);
    }



    if (!window->is_lan || window->server)
    {
        char *s_list[2] = { window->txt[RETRY], window->txt[BACK_9] };
        SDL_Color blue = { 0, 255, 255, alpha };
        SDL_Color green = { 0, 255, 0, alpha };


        for (int i = 1; i <= 2; i++)
        {
            if (selected == i)
                render_text(window, window->fonts->zero4b_30_small, s_list[i - 1],
                            green, 150, 730 + (i - 1) * 100);
            else
                render_text(window, window->fonts->zero4b_30_small, s_list[i - 1],
                            blue, 150, 730 + (i - 1) * 100);
        }
    }
    else
        render_text(window, window->fonts->zero4b_30_small, window->txt[WAITING_FOR_THE_SERVER],
                            orange, 150, 730);
}


int failure(struct window *window, int level_num)
{
    Uint32 begin = SDL_GetTicks();
    int escape = 0;
    unsigned selected = 0;
    int is_best = 0;

    if (window->save->score[window->num_players - 1][level_num - 1] < window->score)
    {
        window->save->score[window->num_players - 1][level_num - 1] = window->score;
        is_best = 1;
    }

    load_music(window, "data/failure.ogg", 1);

    SDL_Rect areas[2];
    for (unsigned i = 0; i < 2; i++)
    {
        areas[i].x = 150;
        areas[i].y = 730 + i * 100;
        TTF_SizeText(window->fonts->zero4b_30_small, window->txt[RETRY + i], &areas[i].w, &areas[i].h);
    }

    while (!escape)
    {
        update_events(window->in, window, 0);
        handle_quit_event(window, 1);
        if (!window->is_lan || window->server)
        {
            escape = handle_escape_event(window);
            if (selected > 0 && handle_play_event(window))
            {
                if (selected == 2)
                {
                    if (window->is_lan)
                    {
                        struct msg msg = { .type = MENU_MSG };
                        send_msg(window, &msg);
                    }

                    escape = 1;
                }
                else if (window->is_lan)
                {
                    struct msg msg = { .type = RESTART_MSG };
                    send_msg(window, &msg);
                }

                break;
            }

            handle_select_arrow_event(window, &selected, 2, areas);
        }
        else
        {
            handle_messages(window, "RM");

            if (window->restart > 0)
            {
                if (window->restart == 2)
                    escape = 1;
                break;
            }
        }


        SDL_RenderClear(window->renderer);

        // Display black bg
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        render_stars(window);
        render_failure_texts(window, begin, selected, is_best);
        SDL_RenderPresent(window->renderer);

        SDL_framerateDelay(window->fps);
    }

    window->restart = 0;

    return escape;
}

