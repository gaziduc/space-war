#include "event.h"
#include "init.h"
#include "utils.h"
#include "game.h"
#include "stars.h"
#include "menu.h"
#include "level.h"
#include <SDL2/SDL.h>


static int handle_play_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_RETURN])
    {
        window->in->key[SDL_SCANCODE_RETURN] = 0;

        select_level(window);

        return 1;
    }

    return 0;
}


static void render_menu_texts(struct window *window, Uint32 begin)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Rect pos = { .x = 150, .y = 150, .w = 0, .h = 0 };
    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };
    SDL_Color red = { .r = 255, .g = 0, .b = 0, .a = alpha };

    SDL_Texture *title = get_text_texture(window, window->fonts->zero4b_30, "SPACE WAR", blue);
    SDL_QueryTexture(title, NULL, NULL, &pos.w, &pos.h);
    SDL_RenderCopy(window->renderer, title, NULL, &pos);
    SDL_DestroyTexture(title);

    pos.y = 740;

    SDL_Texture *play = get_text_texture(window, window->fonts->zero4b_30_small, "PLAY: ENTER", red);
    SDL_QueryTexture(play, NULL, NULL, &pos.w, &pos.h);
    SDL_RenderCopy(window->renderer, play, NULL, &pos);
    SDL_DestroyTexture(play);

    pos.y = 840;

    SDL_Texture *quit = get_text_texture(window, window->fonts->zero4b_30_small, "QUIT: ESCAPE", red);
    SDL_QueryTexture(quit, NULL, NULL, &pos.w, &pos.h);
    SDL_RenderCopy(window->renderer, quit, NULL, &pos);
    SDL_DestroyTexture(quit);
}


void render_stars(struct window *window)
{
    for (;;)
    {
        struct return_point rp;
        int return_code = process_point(window->universe, &rp);

        if (return_code == 0)
            break;

        if (return_code == 1)
        {
            SDL_SetRenderDrawColor(window->renderer,
                                   255 * rp.opacity / OPACITY_MAX,
                                   255 * rp.opacity / OPACITY_MAX,
                                   255 * rp.opacity / OPACITY_MAX,
                                   255 * rp.opacity / OPACITY_MAX);

            SDL_RenderDrawPoint(window->renderer, rp.x + window->w / 2 - 1,
                                                  -rp.y + window->h / 2 - 1);
        }
    }

    for (int c = 0; c < rand() % 128; c++)
        new_point(window->universe, window);

}


void menu(struct window *window)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in);
        handle_quit_event(window);
        if (handle_play_event(window))
            begin = SDL_GetTicks();
        escape = handle_escape_event(window);

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_menu_texts(window, begin);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}
