#include "event.h"
#include "pause.h"
#include "setting.h"
#include "menu.h"
#include "utils.h"
#include "help.h"
#include <SDL2/SDL.h>


static void render_pause_texts(struct window *window, Uint32 begin, int selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };
    SDL_Color green = { .r = GREEN_R, .g = GREEN_G, .b = GREEN_B, .a = alpha };
    SDL_Color orange = { 255, 127, 39, alpha };

    // Render title
    render_text(window, window->fonts->zero4b_30, window->txt[PAUSE], orange, 150, 150);

    // Render items
    char *s_list[NUM_CHOICES_PAUSE] = { window->txt[RESUME], window->txt[SETTINGS_2], window->txt[HELP_2], window->txt[ESCAPE] };

    for (int i = 1; i <= NUM_CHOICES_PAUSE; i++)
    {
        render_text(window, window->fonts->zero4b_30_small, s_list[i - 1],
                    selected_item != i ? blue : green,
                    150, 570 + (i - 1) * 100);
    }
}


int pause(struct window *window)
{
    SDL_Texture *bg = NULL;

    if (!window->in->focus_lost)
    {
        // take a screenshot of background
        SDL_Surface *sshot = SDL_CreateRGBSurface(0, window->w, window->h, 32, 0, 0, 0, 0);
        SDL_RenderReadPixels(window->renderer, NULL, sshot->format->format, sshot->pixels, sshot->pitch);
        bg = SDL_CreateTextureFromSurface(window->renderer, sshot);
        SDL_FreeSurface(sshot);
    }

    int escape = 0;
    unsigned selected = 0;
    Uint32 first_begin = SDL_GetTicks();
    Uint32 begin = first_begin;
    SDL_Rect areas[NUM_CHOICES_PAUSE];

    for (unsigned i = 0; i < NUM_CHOICES_PAUSE; i++)
    {
        areas[i].x = 150;
        areas[i].y = 570 + i * 100;
        TTF_SizeText(window->fonts->zero4b_30_small, window->txt[RESUME + i], &areas[i].w, &areas[i].h);
    }

    while (!escape)
    {
        // Handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 1);
        handle_select_arrow_event(window, &selected, NUM_CHOICES_PAUSE, areas);
        if (handle_escape_event(window))
            escape = 1;

        if (selected > 0 && handle_play_event(window))
        {
            if (selected == 1)
                escape = 1;
            else if (selected == 2)
            {
                settings(window);
                begin = SDL_GetTicks();
            }
            else if (selected == 3)
            {
                help(window);
                begin = SDL_GetTicks();
            }
            else if (selected == 4)
                escape = 2;
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Copy screenshot on background and add transparency
        if (bg)
            SDL_RenderCopy(window->renderer, bg, NULL, NULL);

        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 160);
        SDL_RenderFillRect(window->renderer, NULL);

        // Render menu options
        render_pause_texts(window, begin, selected);

        // Show screen
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        frame_delay(window->fps);
    }

    delay_times(window, first_begin);
    SDL_ShowCursor(SDL_DISABLE);

    SDL_DestroyTexture(bg);
    return escape - 1;
}


void delay_times(struct window *window, Uint32 begin)
{
    Uint32 delay = SDL_GetTicks() - begin;

    window->last_enemy_time += delay;
    window->wave_title_time += delay;

    for (unsigned i = 0; i < window->num_players; i++)
        window->player[i].last_shot_time += delay;

    window->mission_start_time += delay;
    window->combo_time += delay;
    window->last_combo_time += delay;
}
