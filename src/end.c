#include "event.h"
#include "utils.h"
#include "menu.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>


static void render_success_texts(struct window *window, Uint32 begin)
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
    sprintf(s, "Health Bonus: %d", window->health);

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
    sprintf(s, "TOTAL: %d", window->score + window->health + bombs_bonus + window->bonus);

    render_text(window, window->fonts->zero4b_30_small, s, orange,
                150, 620);

    // Enter to continue
    render_text(window, window->fonts->zero4b_30_small, "-> CONTINUE",
                green, 150, 810);
}



void success(struct window *window)
{
    Uint32 begin = SDL_GetTicks();
    int escape = 0;
    int selected = 1;

    load_music(window, "data/success.ogg", 1);

    while (!escape)
    {
        update_events(window->in, window);
        escape = handle_play_event(window);
        handle_select_arrow_event(window, &selected, 1);

        SDL_RenderClear(window->renderer);

        // Display black bg
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        render_stars(window);
        render_success_texts(window, begin);
        SDL_RenderPresent(window->renderer);

        SDL_framerateDelay(window->fps);
    }
}


static void render_failure_texts(struct window *window, Uint32 begin, int selected)
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


    char *s_list[2] = { "-> RETRY", "-> BACK" };
    SDL_Color blue = { 0, 255, 255, alpha };
    SDL_Color green = { 0, 255, 0, alpha };


    for (int i = 1; i <= 2; i++)
    {
        if (selected == i)
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1],
                        green, 150, 730 + (i - 1) * 100);
        else
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1] + 3,
                        blue, 150, 730 + (i - 1) * 100);
    }
}


int failure(struct window *window)
{
    Uint32 begin = SDL_GetTicks();
    int escape = 0;
    int selected = 1;

    load_music(window, "data/failure.ogg", 1);

    while (!escape)
    {
        update_events(window->in, window);
        escape = handle_escape_event(window);
        if (handle_play_event(window))
            escape = 1;
        handle_select_arrow_event(window, &selected, 2);

        SDL_RenderClear(window->renderer);

        // Display black bg
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        render_stars(window);
        render_failure_texts(window, begin, selected);
        SDL_RenderPresent(window->renderer);

        SDL_framerateDelay(window->fps);
    }

    if (selected == 1)
        escape = 0;

    return escape;
}

