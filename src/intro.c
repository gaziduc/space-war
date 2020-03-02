#include "intro.h"
#include "init.h"
#include "event.h"
#include "utils.h"
#include <string.h>
#include <SDL2/SDL.h>


static int render_screen(struct window *window, char screen[][INTRO_COLS],
                          size_t line, size_t col, Uint32 begin, Uint32 curr)
{
    static SDL_Color green = { 0, 255, 0, 255 };

    // handle events
    update_events(window->in, window);
    handle_quit_event(window, 0);
    if (handle_play_event(window))
        return 1;


    // print text
    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    SDL_RenderClear(window->renderer);

    for (int i = 0; i < INTRO_LINES; i++)
    {
        for (int j = 0; j < INTRO_COLS; j++)
        {
            if (screen[i][j])
            {
                char text[2] = { screen[i][j], '\0' };
                render_text(window, window->fonts->pixel, text, green,
                            j * INTRO_CHAR_W + 2, i * INTRO_CHAR_H + 3);
            }
        }
    }

    // print cursor
    Uint32 alpha = (curr - begin) % 1000;
    if (alpha < 500)
    {
        if (alpha > 250)
            alpha = -alpha + 500;

        SDL_SetRenderDrawColor(window->renderer, 0, alpha, 0, alpha);
        SDL_Rect pos = { .x = 1 + col * INTRO_CHAR_W, .y = 2 + line * INTRO_CHAR_H,
                         .w = INTRO_CHAR_W - 1, .h = INTRO_CHAR_H - 3};
        SDL_RenderFillRect(window->renderer, &pos);
    }

    SDL_RenderPresent(window->renderer);
    SDL_framerateDelay(window->fps);
    return 0;
}


void intro(struct window *window)
{
    char screen[INTRO_LINES][INTRO_COLS] = { 0 };

    load_music(window, "data/intro.ogg", 0);
    Uint32 begin = SDL_GetTicks();

    char s[] = "GAME MADE BY DAVID GHIASSI (GAZI)...";

    size_t line = 15;
    size_t col = 35;

    Uint32 temp = begin;

    while (col - 35 < strlen(s))
    {
        Uint32 curr = SDL_GetTicks();

        if (curr - temp >= 100)
        {
            screen[line][col] = s[col - 35];
            col++;
            temp += 100;
        }

        if (render_screen(window, screen, line, col, begin, curr))
            return;
    }


    while (col >= 35)
    {
        Uint32 curr = SDL_GetTicks();

        if (curr - temp >= 100)
        {
            screen[line][col] = 0;
            col--;
            temp += 100;
        }

        if (render_screen(window, screen, line, col, begin, curr))
            return;
    }
}
