#include "event.h"
#include "init.h"
#include "utils.h"
#include "game.h"
#include "stars.h"
#include "menu.h"
#include "level.h"
#include "setting.h"
#include <stdlib.h>
#include <SDL2/SDL.h>


static void render_menu_texts(struct window *window, Uint32 begin, int selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Rect pos = { .x = 150, .y = 150, .w = 0, .h = 0 };
    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };
    SDL_Color green = { .r = 0, .g = 255, .b = 0, .a = alpha };

    // Render title
    SDL_Texture *title = get_text_texture(window, window->fonts->zero4b_30, "SPACE WAR", blue);
    SDL_QueryTexture(title, NULL, NULL, &pos.w, &pos.h);
    SDL_RenderCopy(window->renderer, title, NULL, &pos);
    SDL_DestroyTexture(title);

    // Render items
    char *s_list[NUM_ITEMS] = { "-> PLAY", "-> SETTINGS", "-> QUIT" };

    for (int i = 1; i <= NUM_ITEMS; i++)
    {
        if (selected_item != i)
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1] + 3, blue,
                        150, 640 + (i - 1) * 100);
        else
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1], green,
                        150, 640 + (i - 1) * 100);

    }
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
    load_music(window, "data/hybris.ogg", 1);

    int escape = 0;
    int selected_item = 1;
    Uint32 begin = SDL_GetTicks();

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &selected_item, NUM_ITEMS);

        if (handle_play_event(window))
        {
            switch (selected_item)
            {
                case 1:
                    select_level(window);
                    break;
                case 2:
                    settings(window);
                    break;
                case 3:
                    escape = 1;
                    break;
            }

            begin = SDL_GetTicks();
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_menu_texts(window, begin, selected_item);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}
