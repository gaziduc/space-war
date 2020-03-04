#include "credits.h"
#include "init.h"
#include "event.h"
#include "utils.h"
#include "menu.h"
#include <string.h>
#include <SDL2/SDL.h>


static int render_screen(struct window *window, char screen[][CREDITS_COLS],
                          size_t line, size_t col, Uint32 begin)
{
    static SDL_Color green = { 0, 255, 0, 255 };

    // Handle events
    update_events(window->in, window);
    handle_quit_event(window, 0);
    if (handle_play_event(window) || handle_escape_event(window))
        return 1;


    // Clear screen
    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    SDL_RenderClear(window->renderer);

    // Render stars
    render_stars(window);

    for (int i = 0; i < CREDITS_LINES; i++)
    {
        for (int j = 0; j < CREDITS_COLS; j++)
        {
            if (screen[i][j])
            {
                char text[2] = { screen[i][j], '\0' };

                render_text(window, window->fonts->pixel, text, green,
                            LEFT_PADDING + j * CREDITS_CHAR_W,
                            UP_PADDING + i * CREDITS_CHAR_H);
            }
        }
    }

    // Print cursor
    Uint32 alpha = (SDL_GetTicks() - begin) % 1000;

    if (alpha <= 511)
    {
        if (alpha > 255)
            alpha = -alpha + 511;

        SDL_SetRenderDrawColor(window->renderer, 0, alpha, 0, alpha);

        SDL_Rect pos = { .x = LEFT_PADDING + col * CREDITS_CHAR_W,
                         .y = UP_PADDING + line * CREDITS_CHAR_H,
                         .w = CREDITS_CHAR_W,
                         .h = CREDITS_CHAR_H };
        SDL_RenderFillRect(window->renderer, &pos);
    }

    SDL_RenderPresent(window->renderer);
    SDL_framerateDelay(window->fps);

    return 0;
}



static int add_letters(struct window *window, char screen[][CREDITS_COLS], char *letters,
                        size_t *line, size_t *col, Uint32 time_between_two_letters,
                        Uint32 time_end, Uint32 begin)
{
    size_t i = 0;

    while (i < strlen(letters))
    {
        Uint32 curr = SDL_GetTicks();
        while (SDL_GetTicks() - curr < time_between_two_letters)
        {
            if (render_screen(window, screen, *line, *col, begin))
                return 1;
        }

        if (letters[i] != '\n')
        {
            screen[*line][*col] = letters[i];
            (*col)++;
        }
        else
        {
            (*line)++;
            *col = 0;
        }

        i++;
    }

    Uint32 curr = SDL_GetTicks();

    while (SDL_GetTicks() - curr < time_end)
    {
        if (render_screen(window, screen, *line, *col, begin))
            return 1;
    }

    return 0;
}


void credits(struct window *window)
{
    char screen[CREDITS_LINES][CREDITS_COLS] = { 0 };
    Uint32 begin = SDL_GetTicks();

    char *msg = "GAME CREATED BY DAVID \"GAZI\" GHIASSI\n\n\n\nPROGRAMMED IN C LANGUAGE\n\n\n\n";
    char *h_line = "*------------------*\n";

    size_t line = 0;
    size_t col = 0;

    if (add_letters(window, screen, msg, &line, &col, 50, 0, begin))
        return;

    if (add_letters(window, screen, h_line, &line, &col, 50, 0, begin))
        return;

    if (add_letters(window, screen, "|  LIBRARIES USED  |\n", &line, &col, 50, 0, begin))
        return;

    if (add_letters(window, screen, h_line, &line, &col, 50, 0, begin))
        return;

    if (add_letters(window, screen, "|       SDL2       |\n", &line, &col, 50, 0, begin))
        return;

    if (add_letters(window, screen, "|    SDL2_IMAGE    |\n", &line, &col, 50, 0, begin))
        return;

    if (add_letters(window, screen, "|     SDL2_TTF     |\n", &line, &col, 50, 0, begin))
        return;

    if (add_letters(window, screen, "|    SDL2_MIXER    |\n", &line, &col, 50, 0, begin))
        return;

    if (add_letters(window, screen, "|     SDL2_GFX     |\n", &line, &col, 50, 0, begin))
        return;

    if (add_letters(window, screen, h_line, &line, &col, 50, 0, begin))
        return;

    if (add_letters(window, screen, "\n\n\n\nTHANKS FOR PLAYING!", &line, &col, 50, 42000, begin))
        return;
}
