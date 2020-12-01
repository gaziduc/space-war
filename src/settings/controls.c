#include "init.h"
#include "menu.h"
#include "utils.h"
#include "event.h"
#include "controls.h"
#include "setting.h"
#include <stdio.h>
#include <SDL2/SDL.h>

static void render_controls(struct window *window, Uint32 begin, int selected_item)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color white = { 255, 255, 255, alpha };
    SDL_Color green = { 0, 255, 0, alpha };
    SDL_Color orange = { 255, 128, 0, alpha };
    SDL_Color blue = { 0, 255, 255, alpha };

    render_text(window, window->fonts->zero4b_30_small, window->txt[KEYBOARD_CONTROLS], orange, 150, 150);

    char s[64] = { 0 };

    for (enum control i = 0; i < NUM_CONTROLS; i++)
    {
        sprintf(s, "%s: %s", window->txt[UP_TXT + i], SDL_GetScancodeName(window->settings->controls[i]));
        render_text(window, window->fonts->zero4b_30_small, s, selected_item - 1 == (int) i ? green : white, 150, 280 + i * 75);
    }

    render_text(window, window->fonts->zero4b_30_small, window->txt[RESET_INPUTS], selected_item == (int) NUM_CONTROLS + 1 ? green : blue, 150, 310 + NUM_CONTROLS * 80);
    render_text(window, window->fonts->zero4b_30_small, window->txt[BACK_8], selected_item == (int) NUM_CONTROLS + 2 ? green : blue, 150, 310 + (NUM_CONTROLS + 1) * 80);
}


void controls(struct window *window)
{
    int escape = 0;
    unsigned selected_item = 0;
    Uint32 begin = SDL_GetTicks();
    SDL_Rect areas[NUM_CONTROLS + 2];

    while (!escape)
    {
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);

        for (unsigned i = 0; i < NUM_CONTROLS; i++)
        {
            areas[i].x = 150;
            areas[i].y = 280 + i * 80;

            char s[64] = { 0 };
            sprintf(s, "%s: %s", window->txt[UP_TXT + i], SDL_GetScancodeName(window->settings->controls[i]));
            TTF_SizeText(window->fonts->zero4b_30_small, s, &areas[i].w, &areas[i].h);
        }

        for (unsigned i = 0; i < 2; i++)
        {
            areas[NUM_CONTROLS + i].x = 150;
            areas[NUM_CONTROLS + i].y = 310 + (NUM_CONTROLS + i) * 80;
            TTF_SizeText(window->fonts->zero4b_30_small, window->txt[RESET_INPUTS + i], &areas[NUM_CONTROLS + i].w, &areas[NUM_CONTROLS + i].h);
        }

        handle_select_arrow_event(window, &selected_item, NUM_CONTROLS + 2, areas);

        if (selected_item > 0 && handle_play_event(window))
        {
            if (selected_item == NUM_CONTROLS + 1) // if user want to reset inputs
            {
                reset_controls(window);
                write_settings(window);
            }
            else if (selected_item == NUM_CONTROLS + 2)
                escape = 1;
            else
            {
                SDL_Scancode temp = get_key(window, selected_item);
                if (temp != SDL_SCANCODE_ESCAPE)
                    window->settings->controls[selected_item - 1] = temp;
                write_settings(window);
                begin = SDL_GetTicks();
            }
        }

        escape = escape || handle_escape_event(window);

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_controls(window, begin, selected_item);
        render_controller_input_texts(window, begin, 1);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}


SDL_Scancode get_key(struct window *window, int selected_item)
{
    SDL_Event event;
    Uint32 begin = SDL_GetTicks();

    char s1[128] = { 0 };
    sprintf(s1, window->txt[ENTER_KEY], window->txt[UP_TXT + selected_item - 1]);

    char s2[128] = { 0 };
    sprintf(s2, window->txt[CURRENT_KEY], SDL_GetScancodeName(window->settings->controls[selected_item - 1]));

    while (1)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    return event.key.keysym.scancode;
            }
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);

        Uint32 alpha = SDL_GetTicks() - begin;
        if (alpha > TITLE_ALPHA_MAX)
            alpha = TITLE_ALPHA_MAX;
        else if (alpha == 0)
            alpha = 1;

        SDL_Color orange = { 255, 128, 0, alpha };
        SDL_Color white = { 255, 255, 255, alpha };

        render_text(window, window->fonts->zero4b_30_small, s1, orange, 150, 150);
        render_text(window, window->fonts->pixel_large, s2, orange, 150, 350);
        render_text(window, window->fonts->zero4b_30_small, "Escape to cancel...", white, 150, 850);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}



void reset_controls(struct window *window)
{
    window->settings->controls[UP] = SDL_SCANCODE_UP;
    window->settings->controls[LEFT] = SDL_SCANCODE_LEFT;
    window->settings->controls[DOWN] = SDL_SCANCODE_DOWN;
    window->settings->controls[RIGHT] = SDL_SCANCODE_RIGHT;
    window->settings->controls[SHOOT] = SDL_SCANCODE_SPACE;
    window->settings->controls[BOMB] = SDL_SCANCODE_B;
}

