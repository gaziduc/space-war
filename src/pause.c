#include "event.h"
#include "pause.h"
#include "setting.h"
#include "menu.h"
#include "utils.h"
#include <SDL2/SDL.h>


static void render_pause_texts(struct window *window, Uint32 begin, int selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = 0, .g = 255, .b = 255, .a = alpha };
    SDL_Color green = { .r = 0, .g = 255, .b = 0, .a = alpha };

    // Render title
    render_text(window, window->fonts->zero4b_30, "PAUSE", blue, 150, 150);

    // Render items
    char *s_list[NUM_CHOICES_PAUSE] = { "-> RESUME", "-> SETTINGS", "-> ESCAPE" };

    for (int i = 1; i <= NUM_CHOICES_PAUSE; i++)
    {
        if (selected_item != i)
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1] + 3, blue,
                        150, 670 + (i - 1) * 100);
        else
            render_text(window, window->fonts->zero4b_30_small, s_list[i - 1], green,
                        150, 670 + (i - 1) * 100);
    }
}


int pause(struct window *window)
{
    // take a screenshot of background
    SDL_Surface *sshot = SDL_CreateRGBSurface(0, window->w, window->h, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(window->renderer, NULL, sshot->format->format, sshot->pixels, sshot->pitch);
    SDL_Texture *bg = SDL_CreateTextureFromSurface(window->renderer, sshot);
    SDL_FreeSurface(sshot);


    int escape = 0;
    int selected = 1;
    Uint32 first_begin = SDL_GetTicks();
    Uint32 begin = first_begin;

    while (!escape)
    {
        // Handle events
        update_events(window->in, window);
        handle_quit_event(window, 1);
        handle_select_arrow_event(window, &selected, NUM_CHOICES_PAUSE);
        if (handle_escape_event(window))
            escape = 1;

        if (handle_play_event(window))
        {
            if (selected == 1)
                escape = 1;
            if (selected == 2)
            {
                settings(window);
                begin = SDL_GetTicks();
            }
            if (selected == 3)
                escape = 2;
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Copy screenshot on background and add transparency
        SDL_RenderCopy(window->renderer, bg, NULL, NULL);
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 160);
        SDL_RenderFillRect(window->renderer, NULL);


        // Render menu options
        render_pause_texts(window, begin, selected);

        // Show screen
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }

    Uint32 delay = SDL_GetTicks() - first_begin;

    window->last_enemy_time += delay;
    window->last_shot_time += delay;
    window->wave_title_time += delay;

    SDL_DestroyTexture(bg);
    return escape - 1;
}
