#include "credits.h"
#include "init.h"
#include "event.h"
#include "utils.h"
#include "menu.h"
#include <string.h>
#include <SDL2/SDL.h>


static int render_screen(struct window *window, char screen[][CREDITS_COLS],
                          size_t line, size_t col, Uint32 begin, int is_in_menu)
{
    static SDL_Color green = { 0, 255, 0, TITLE_ALPHA_MAX };

    // Handle events
    update_events(window->in, window);
    handle_quit_event(window, 0);
    if (handle_play_event(window) || handle_escape_event(window))
        return 1;


    // Clear screen
    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    SDL_RenderClear(window->renderer);

    // Render stars if we are in a menu
    if (is_in_menu)
        render_stars(window);

    // Render letters
    for (int i = 0; i < CREDITS_LINES; i++)
    {
        for (int j = 0; j < CREDITS_COLS; j++)
        {
            if (screen[i][j])
            {
                render_text(window, window->fonts->pixel_large, screen[i] + j, green,
                            LEFT_PADDING + j * CREDITS_CHAR_W,
                            UP_PADDING + i * CREDITS_CHAR_H);
                break;
            }
        }
    }

    // Render cursor
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

        resize_pos_for_resolution(window, &pos);

        SDL_RenderFillRect(window->renderer, &pos);
    }

    SDL_RenderPresent(window->renderer);
    SDL_framerateDelay(window->fps);

    return 0;
}



int add_letters(struct window *window, char screen[][CREDITS_COLS], char *letters,
                        size_t *line, size_t *col, Uint32 time_between_two_letters,
                        Uint32 time_end, Uint32 begin, int is_in_menu)
{
    size_t i = 0;

    while (i < strlen(letters))
    {
        Uint32 curr = SDL_GetTicks();
        while (SDL_GetTicks() - curr < time_between_two_letters)
        {
            if (render_screen(window, screen, *line, *col, begin, is_in_menu))
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
        if (render_screen(window, screen, *line, *col, begin, is_in_menu))
            return 1;
    }

    return 0;
}


void credits(struct window *window)
{
    char screen[CREDITS_LINES][CREDITS_COLS] = { 0 };
    Uint32 begin = SDL_GetTicks();

    char *msg = "GAME CREATED BY DAVID \"GAZI\" GHIASSI\n\n\nPROGRAMMED IN C LANGUAGE\n\n\n";
    char *h_line = "+------------------+\n";

    size_t line = 0;
    size_t col = 0;

    ADD_LETTERS(window, screen, msg, &line, &col, 50, 0, begin, 1);
    ADD_LETTERS(window, screen, h_line, &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, "|  LIBRARIES USED  |\n", &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, h_line, &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, "|       SDL2       |\n", &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, "|    SDL2_IMAGE    |\n", &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, "|     SDL2_TTF     |\n", &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, "|    SDL2_MIXER    |\n", &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, "|     SDL2_GFX     |\n", &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, "|     SDL2_NET     |\n", &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, "|     LIBCURL      |\n", &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, h_line, &line, &col, 50, 0, begin, 1)
    ADD_LETTERS(window, screen, "\n\n\nTHANKS FOR PLAYING!", &line, &col, 50, 42000, begin, 1)
}