#include "init.h"
#include "game.h"
#include "event.h"
#include "menu.h"
#include "utils.h"
#include "level.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>

static int handle_play_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_RETURN])
    {
        window->in->key[SDL_SCANCODE_RETURN] = 0;
        return 1;
    }

    return 0;
}


static void handle_arrow_event(struct window *window, int *selected_level)
{
    if (window->in->key[SDL_SCANCODE_UP])
    {
        window->in->key[SDL_SCANCODE_UP] = 0;

        if (*selected_level > 1)
            (*selected_level)--;
    }
    if (window->in->key[SDL_SCANCODE_DOWN])
    {
        window->in->key[SDL_SCANCODE_DOWN] = 0;

        if ( *selected_level < NUM_LEVELS)
            (*selected_level)++;
    }
}


static void render_selected_level_title(struct window *window, const char *s, Uint32 alpha)
{
    SDL_Color orange = { .r = 255, .g = 128, .b = 0, .a = alpha };

    SDL_Texture *texture = get_text_texture(window, window->fonts->zero4b_30_small,
                                            s, orange);

    SDL_Rect pos = { .x = 1300, .y = 150, .w = 0, .h = 0 };
    SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
    pos.x -= pos.w / 2;

    SDL_RenderCopy(window->renderer, texture, NULL, &pos);
    SDL_DestroyTexture(texture);
}


static void render_level_texts(struct window *window, Uint32 begin, int selected_level)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };
    SDL_Color green = { .r = 0, .g = 255, .b = 0, .a = alpha };

    for (int i = 1; i <= NUM_LEVELS; i++)
    {
        char s[50] = { 0 };
        sprintf(s, "-> Mission %d", i);

        int y = 150 + (i - 1) * 80;

        if (i != selected_level)
            render_text(window, window->fonts->zero4b_30_small, s + 3, blue, 150, y);
        else
            render_text(window, window->fonts->zero4b_30_small, s, green, 150, y);
    }

    switch (selected_level)
    {
        case 1:
            render_selected_level_title(window, "The Milky Way", alpha);
            break;
        case 2:
            render_selected_level_title(window, "Andromeda Galaxy", alpha);
            break;

        default:
            error("Unknown selected level", "Unknown selected level number", window->window);
            break;
    }
}


void select_level(struct window *window)
{
    int escape = 0;
    int selected_level = 1;
    Uint32 begin = SDL_GetTicks();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in);
        handle_quit_event(window, 0);

        if (handle_play_event(window))
        {
            play_game(window, selected_level);
            begin = SDL_GetTicks();
        }

        handle_arrow_event(window, &selected_level);
        escape = handle_escape_event(window);

        // Display black bachground
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_level_texts(window, begin, selected_level);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}
