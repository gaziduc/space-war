#include "event.h"
#include "utils.h"
#include "menu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>


static void render_success_texts(struct window *window, Uint32 begin)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color green = { .r = 0, .g = 192, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30, "SUCCESS", green,
                POS_CENTERED, window->h / 2 - 250);

    char s[50] = { 0 };
    sprintf(s, "SCORE: %d", window->score);

    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30_small, s, orange,
                POS_CENTERED, window->h / 2);

    render_text(window, window->fonts->zero4b_30_extra_small, "CONTINUE: ENTER",
                green, POS_CENTERED, window->h / 2 + 180);
}


void success(struct window *window)
{
    Uint32 begin = SDL_GetTicks();
    int escape = 0;

    while (!escape)
    {
        update_events(window->in);
        escape = handle_escape_event(window);

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


static void render_failure_texts(struct window *window, Uint32 begin)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color red = { .r = 192, .g = 0, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30, "FAILURE", red,
                POS_CENTERED, window->h / 2 - 250);

    char s[50] = { 0 };
    sprintf(s, "SCORE: %d", window->score);

    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

    render_text(window, window->fonts->zero4b_30_small, s, orange,
                POS_CENTERED, window->h / 2);

    render_text(window, window->fonts->zero4b_30_extra_small, "RETRY: ENTER", red,
                POS_CENTERED, window->h / 2 + 160);

    render_text(window, window->fonts->zero4b_30_extra_small, "BACK: ESCAPE", red,
                POS_CENTERED, window->h / 2 + 220);
}


static int handle_retry_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_RETURN])
    {
        window->in->key[SDL_SCANCODE_RETURN] = 0;
        return 1;
    }

    return 0;
}


int failure(struct window *window)
{
    Uint32 begin = SDL_GetTicks();
    int escape = 0;
    int retry = 0;

    while (!escape && !retry)
    {
        update_events(window->in);
        escape = handle_escape_event(window);
        retry = handle_retry_event(window);

        SDL_RenderClear(window->renderer);

        // Display black bg
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        render_stars(window);
        render_failure_texts(window, begin);
        SDL_RenderPresent(window->renderer);

        SDL_framerateDelay(window->fps);
    }

    return escape;
}

