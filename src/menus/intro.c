#include "init.h"
#include "utils.h"
#include "event.h"
#include "credits.h"
#include "menu.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>


static void render_controller(struct window *window, Uint8 alpha)
{
    SDL_Rect controller_pos;
    init_position(POS_CENTERED, 775, window->img->controller, &controller_pos);
    resize_pos_for_resolution(window, &controller_pos);
    SDL_SetTextureAlphaMod(window->img->controller, alpha);
    SDL_RenderCopy(window->renderer, window->img->controller, NULL, &controller_pos);
}

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

        SDL_Rect pos = { .x = DEFAULT_W / 2 - 300, .y = 665, .w = 600, .h = 45 };
        resize_pos_for_resolution(window, &pos);
        SDL_SetRenderDrawColor(window->renderer, 90, 230, 29, i == 0 ? 1 : i);
        SDL_RenderFillRect(window->renderer, &pos);
        SDL_SetRenderDrawColor(window->renderer, 225, 225, 225,  i == 0 ? 1 : i);
        SDL_RenderDrawRect(window->renderer, &pos);


        render_text(window, window->fonts->craft_large, "100 %", white, POS_CENTERED, 673);
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

        render_text(window, window->fonts->craft_large, s[1], white,
                    POS_CENTERED, 575);

        render_text(window, window->fonts->craft_large, s[2], white,
                    POS_CENTERED, 725);

        render_controller(window, i);

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

        render_text(window, window->fonts->craft_large, s[1], white,
                POS_CENTERED, 575);

        render_text(window, window->fonts->craft_large, s[2], white,
                POS_CENTERED, 725);

        render_controller(window, 195);
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

        render_text(window, window->fonts->craft_large, s[1], white,
                POS_CENTERED, 575);

        render_text(window, window->fonts->craft_large, s[2], white,
                POS_CENTERED, 725);

        render_controller(window, i);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }


    load_music(window, "data/intro.ogg", 0);


    char screen[CREDITS_LINES][CREDITS_COLS] = { 0 };

    size_t line = 0;
    size_t col = 0;

    ADD_LETTERS(window, screen, window->txt[INTRO_1], &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, "\n\n", &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, window->txt[INTRO_2], &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, "\n", &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, window->txt[INTRO_3], &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, "\n", &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, window->txt[INTRO_4], &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, "\n\n", &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, window->txt[INTRO_5], &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, "\n", &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, window->txt[INTRO_6], &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, "\n", &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, window->txt[INTRO_7], &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n", &line, &col, 60, 0, 0);
    ADD_LETTERS(window, screen, window->txt[INTRO_8], &line, &col, 60, 42000, 0);
}
