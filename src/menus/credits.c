#include "credits.h"
#include "init.h"
#include "event.h"
#include "utils.h"
#include "menu.h"
#include <string.h>
#include <SDL2/SDL.h>


static int render_screen(struct window *window, char screen[][CREDITS_COLS], int is_in_menu)
{
    static SDL_Color green = { GREEN_R, GREEN_G, GREEN_B, TITLE_ALPHA_MAX };

    // Handle events
    update_events(window->in, window, 0);
    handle_quit_event(window, 0);
    if (handle_play_event(window) || handle_escape_event(window))
        return 1;


    // Clear screen
    SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
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
                SDL_Texture *texture = get_text_texture(window, window->fonts->craft_large, screen[i] + j, green);

                if (!texture)
                    break;

                int w = 0;
                int h = 0;
                SDL_QueryTexture(texture, NULL, NULL, &w, &h);
                SDL_FRect pos_dst = { .x = LEFT_PADDING + j * CREDITS_CHAR_W,
                                      .y = UP_PADDING + i * CREDITS_CHAR_H,
                                      .w = w,
                                      .h = h
                                    };

                resize_pos_for_resolution_float(window, &pos_dst);

                SDL_RenderCopyF(window->renderer, texture, NULL, &pos_dst);

                SDL_DestroyTexture(texture);
                break;
            }
        }
    }

    SDL_RenderPresent(window->renderer);
    frame_delay(window->fps);

    return 0;
}



int add_letters(struct window *window, char screen[][CREDITS_COLS], char *letters,
                        size_t *line, size_t *col, Uint32 time_between_two_letters,
                        Uint32 time_end, int is_in_menu)
{
    size_t i = 0;

    while (i < strlen(letters))
    {
        Uint32 curr = SDL_GetTicks();
        while (SDL_GetTicks() - curr < time_between_two_letters)
        {
            if (render_screen(window, screen, is_in_menu))
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
        if (render_screen(window, screen, is_in_menu))
            return 1;
    }

    return 0;
}


void credits(struct window *window)
{
    char screen[CREDITS_LINES][CREDITS_COLS] = { 0 };
    size_t line = 0;
    size_t col = 0;

    ADD_LETTERS(window, screen, window->txt[CREDITS_1], &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "\n\n\n", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, window->txt[CREDITS_2], &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "\n\n\n", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, window->txt[CREDITS_3], &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "\n\n", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "SDL2", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "\n", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "SDL2_IMAGE", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "\n", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "SDL2_TTF", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "\n", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "SDL2_MIXER", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "\n", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "SDL2_GFX", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, "\n\n\n\n\n\n\n", &line, &col, 50, 0, 1)
    ADD_LETTERS(window, screen, window->txt[CREDITS_4], &line, &col, 50, 42000, 1)
}
