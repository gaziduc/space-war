#include "event.h"
#include "utils.h"
#include "menu.h"
#include "save.h"
#include "net.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>


static void render_success_texts(struct window *window, Uint32 begin, int is_best)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color green = { .r = 0, .g = 255, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30, "SUCCESS", green,
                150, 150);

    // Print score
    char s[50] = { 0 };
    sprintf(s, "Score: %d", window->score);

    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30_extra_small, s, orange,
                150, 400);

    // Health Bonus
    int health_bonus = 0;

    for (unsigned i = 0; i < window->num_players; i++)
        health_bonus += window->player[i].health;

    sprintf(s, "Health Bonus: %d", health_bonus);

    render_text(window, window->fonts->zero4b_30_extra_small, s, orange,
                150, 450);

    // Bombs Bonus
    int bombs_bonus = window->num_bombs * 100;
    sprintf(s, "Bombs Bonus: %d", bombs_bonus);

    render_text(window, window->fonts->zero4b_30_extra_small, s, orange,
                150, 500);

    // Difficulty bonus
    sprintf(s, "Difficulty Bonus: %d", window->bonus);
    render_text(window, window->fonts->zero4b_30_extra_small, s, orange,
                150, 550);

    // Total
    sprintf(s, "TOTAL: %d", window->score + health_bonus + bombs_bonus + window->bonus);

    render_text(window, window->fonts->zero4b_30_small, s, orange,
                150, 620);

    if (is_best)
    {
        SDL_Color yellow = { .r = 255, .g = 255, .b = 0, .a = alpha };
        render_text(window, window->fonts->zero4b_30_small, "NEW BEST!", yellow, 1000, 620);
    }


    // Enter to continue
    if (!window->is_lan || window->server)
        render_text(window, window->fonts->zero4b_30_small, "CONTINUE",
                    green, 150, 810);
    else
        render_text(window, window->fonts->zero4b_30_small, "WAITING FOR THE SERVER...",
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
    SDL_Rect areas[] = { { .x = 150, .y = 810, .w = 1620, .h = 150 } };

    load_music(window, "data/success.ogg", 1);

    while (!escape)
    {
        update_events(window->in, window);
        handle_quit_event(window, 1);

        if (!window->is_lan || window->server)
        {
            escape = handle_play_event(window);
            handle_select_arrow_event(window, &selected, 1, areas);

            if (window->is_lan && window->server)
                send_state(&window->player[0], window, 0, 0, 0, 3, level_num, difficulty);
        }
        else
        {
            if (window->state.state == 0)
                return;
        }

        SDL_RenderClear(window->renderer);

        // Display black bg
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        render_stars(window);
        render_success_texts(window, begin, is_best);

        SDL_RenderPresent(window->renderer);

        SDL_framerateDelay(window->fps);
    }
}


static void render_failure_texts(struct window *window, Uint32 begin, int selected, int is_best)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color red = { .r = 255, .g = 0, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30, "FAILURE", red,
                150, 150);

    char s[50] = { 0 };
    sprintf(s, "SCORE: %d", window->score);

    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30_small, s, orange,
                150, 450);

    if (is_best)
    {
        SDL_Color yellow = { .r = 255, .g = 255, .b = 0, .a = alpha };
        render_text(window, window->fonts->zero4b_30_small, "NEW BEST!", yellow, 150, 550);
    }



    if (!window->is_lan || window->server)
    {
        char *s_list[2] = { "RETRY", "BACK" };
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
        render_text(window, window->fonts->zero4b_30_small, "WAITING FOR THE SERVER...",
                            orange, 150, 730);
}


int failure(struct window *window, int level_num, int level_difficulty)
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

    SDL_Rect areas[] = { { .x = 150, .y = 730, .w = 620, .h = 100 },
                         { .x = 150, .y = 830, .w = 620, .h = 100 }
                     };

    while (!escape)
    {
        update_events(window->in, window);
        handle_quit_event(window, 1);
        if (!window->is_lan || window->server)
        {
            escape = handle_escape_event(window);
            if (selected > 0 && handle_play_event(window))
            {
                if (selected == 2)
                    escape = 1;
                break;
            }

            handle_select_arrow_event(window, &selected, 2, areas);

            if (window->is_lan && window->server)
                send_state(&window->player[0], window, 0, 0, 0, 3, level_num, level_difficulty);
        }
        else
        {
            if (window->state.state == 0)
                return 1;
            else if (window->state.state == 1)
                return 0;
        }



        SDL_RenderClear(window->renderer);

        // Display black bg
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        render_stars(window);
        render_failure_texts(window, begin, selected, is_best);
        SDL_RenderPresent(window->renderer);

        SDL_framerateDelay(window->fps);
    }

    return escape;
}

