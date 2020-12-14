#include "init.h"
#include "utils.h"
#include "event.h"
#include "credits.h"
#include "menu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>


void intro(struct window *window)
{
    char *s[3] = { "SPACE WAR",
                   window->txt[MADE_BY_DAVID],
                   window->txt[COMPATIBLE_WITH_CONTROLLERS]
                 };

    int escape = 0;


    for (int i = 195; i >= 0; i -= 6)
    {
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);

        // Black screen
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        SDL_Color white = { i, i, i, i == 0 ? 1 : i };
        SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = TITLE_ALPHA_MAX };

        render_text(window, window->fonts->zero4b_30, s[0], blue,
                    POS_CENTERED, 200);

        render_text(window, window->fonts->pixel_large, "100 %", white, POS_CENTERED, 675);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }



    for (int i = 0; i < 195; i += 6)
    {
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);

        // Black screen
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        SDL_Color white = { i, i, i, i == 0 ? 1 : i };
        SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = TITLE_ALPHA_MAX };

        render_text(window, window->fonts->zero4b_30, s[0], blue,
                    POS_CENTERED, 200);

        render_text(window, window->fonts->pixel_large, s[1], white,
                    POS_CENTERED, 600);

        render_text(window, window->fonts->pixel_large, s[2], white,
                    POS_CENTERED, 750);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }


    for (int i = 0; i < 100; i++)
    {
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);

        escape = handle_escape_event(window) || handle_play_event(window);
        if (escape)
            break;

        // Black screen
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        SDL_Color white = { 195, 195, 195, 255 };
        SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = TITLE_ALPHA_MAX };

        render_text(window, window->fonts->zero4b_30, s[0], blue,
                POS_CENTERED, 200);

        render_text(window, window->fonts->pixel_large, s[1], white,
                POS_CENTERED, 600);

        render_text(window, window->fonts->pixel_large, s[2], white,
                POS_CENTERED, 750);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }

    for (int i = 195; i >= 0; i -= 6)
    {
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);

        // Black screen
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        SDL_Color white = { i, i, i, i == 0 ? 1 : i };
        SDL_Color blue = { BLUE_R, BLUE_G, BLUE_B, i == 0 ? 1 : i };

        render_text(window, window->fonts->zero4b_30, s[0], blue,
                POS_CENTERED, 200);

        render_text(window, window->fonts->pixel_large, s[1], white,
                POS_CENTERED, 600);

        render_text(window, window->fonts->pixel_large, s[2], white,
                POS_CENTERED, 750);

        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }


    load_music(window, "data/intro.ogg", 0);


    char screen[CREDITS_LINES][CREDITS_COLS] = { 0 };
    Uint32 begin = SDL_GetTicks();

    size_t line = 0;
    size_t col = 0;

    ADD_LETTERS(window, screen, "12th of March 2061:\n\n", &line, &col, 60, 0, begin, 0);
    ADD_LETTERS(window, screen, "On the 11th, the Earth has been destroyed by\n", &line, &col, 60, 0, begin, 0);
    ADD_LETTERS(window, screen, "a giant nuclear explosion. As an astronaut,\n", &line, &col, 60, 0, begin, 0);
    ADD_LETTERS(window, screen, "I was not on the Earth this day.\n\n", &line, &col, 60, 0, begin, 0);
    ADD_LETTERS(window, screen, "In this ship are the last humans alive.\n", &line, &col, 60, 0, begin, 0);
    ADD_LETTERS(window, screen, "Since there is no more Earth, we don't know\n", &line, &col, 60, 0, begin, 0);
    ADD_LETTERS(window, screen, "where to go...\n\n", &line, &col, 60, 0, begin, 0);
    ADD_LETTERS(window, screen, "\n\n\n\n\n\n\n\n\n\n\n", &line, &col, 60, 0, begin, 0);
    ADD_LETTERS(window, screen, "Press ENTER to play", &line, &col, 60, 42000, begin, 0);
}
