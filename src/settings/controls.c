#include "init.h"
#include "menu.h"
#include "utils.h"
#include "event.h"
#include "controls.h"
#include "setting.h"
#include <stdio.h>
#include <SDL2/SDL.h>

static void render_controls(struct window *window, Uint32 begin, int selected_item, char *names[NUM_CONTROLS])
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

    render_text(window, window->fonts->zero4b_30_small, "Keyboard Controls...", orange, 150, 150);

    char s[64] = { 0 };

    for (enum control i = 0; i < NUM_CONTROLS; i++)
    {
        sprintf(s, "-> %s: %s", names[i], SDL_GetScancodeName(window->settings->controls[i]));
        render_text(window, window->fonts->zero4b_30_small, selected_item - 1 == (int) i ? s : s + 3, selected_item - 1 == (int) i ? green : white, 150, 300 + i * 80);
    }

    char *str = "-> Reset inputs";
    render_text(window, window->fonts->zero4b_30_small, selected_item == (int) NUM_CONTROLS + 1 ? str : str + 3, selected_item == (int) NUM_CONTROLS + 1 ? green : blue, 150, 360 + NUM_CONTROLS * 80);
}


void controls(struct window *window)
{
    int escape = 0;
    int selected_item = 1;
    Uint32 begin = SDL_GetTicks();
    char *names[NUM_CONTROLS] = { "Up", "Left", "Down", "Right", "Shoot", "Bomb" };

    while (!escape)
    {
        update_events(window->in, window);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &selected_item, NUM_CONTROLS + 1);
        escape = handle_escape_event(window);
        if (handle_play_event(window))
        {
            if (selected_item == NUM_CONTROLS + 1) // if user want to reset inputs
            {
                reset_controls(window);
                write_settings(window);
            }
            else
            {
                SDL_Scancode temp = get_key(window, selected_item, names);
                if (temp != SDL_SCANCODE_ESCAPE)
                    window->settings->controls[selected_item - 1] = temp;
                write_settings(window);
                begin = SDL_GetTicks();
            }
        }

        // Display black background
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_controls(window, begin, selected_item, names);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}


SDL_Scancode get_key(struct window *window, int selected_item, char *names[NUM_CONTROLS])
{
    SDL_Event event;
    Uint32 begin = SDL_GetTicks();


    char s1[128] = { 0 };
    sprintf(s1, "Enter the key for %s...", names[selected_item - 1]);

    char s2[128] = { 0 };
    sprintf(s2, "Current key: %s", SDL_GetScancodeName(window->settings->controls[selected_item - 1]));

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
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
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

