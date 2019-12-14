#include "event.h"
#include "init.h"
#include "utils.h"
#include "game.h"
#include "stars.h"
#include <SDL2/SDL.h>

static void handle_play_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_RETURN])
    {
        window->in->key[SDL_SCANCODE_RETURN] = 0;

        play_game(window);
    }
}


static void render_menu_texts(struct window *window)
{
    SDL_Rect pos = { .x = 150, .y = 150, .w = 0, .h = 0 };
    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = 224 };
    SDL_Color red = { .r = 255, .g = 0, .b = 0, .a = 224 };

    SDL_Texture *title = get_text_texture(window, window->fonts->zero4b_30, "SPACE WAR", blue);
    SDL_QueryTexture(title, NULL, NULL, &pos.w, &pos.h);
    SDL_RenderCopy(window->renderer, title, NULL, &pos);
    SDL_DestroyTexture(title);

    pos.y = 600;

    SDL_Texture *play = get_text_texture(window, window->fonts->zero4b_30_small, "Play: enter", red);
    SDL_QueryTexture(play, NULL, NULL, &pos.w, &pos.h);
    SDL_RenderCopy(window->renderer, play, NULL, &pos);
    SDL_DestroyTexture(play);

    pos.y = 700;

    SDL_Texture *quit = get_text_texture(window, window->fonts->zero4b_30_small, "Quit: escape", red);
    SDL_QueryTexture(quit, NULL, NULL, &pos.w, &pos.h);
    SDL_RenderCopy(window->renderer, quit, NULL, &pos);
    SDL_DestroyTexture(quit);
}



void menu(struct window *window)
{
    // Initialize the stars lib
    struct universe *u = NULL;
    new_universe(&u, window->w, window->h, 512, window);

    int escape = 0;

    while (!escape)
    {
        // Get and handle events
        update_events(window->in);
        handle_quit_event(window);
        handle_play_event(window);
        escape = handle_escape_event(window);

        // Display textures
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        for (;;)
        {
            struct return_point rp;
            int return_code = process_point(u, &rp);

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
            new_point(u, window);

        render_menu_texts(window);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}
