#include "utils.h"
#include "level.h"
#include "init.h"
#include "save.h"
#include "utils.h"
#include <stdio.h>
#include <SDL2/SDL.h>


void read_save(struct window *window)
{
    // Allocate memory for progress
    window->save = xmalloc(sizeof(struct save), window->window, window->renderer);

    // Open file in binary mode for reading
    SDL_RWops *f = SDL_RWFromFile("save.bin", "rb");

    if (!f) // if could not open file, e.g. the file does not exists
    {
        // Set progress to 0
        for (int i = 0; i < MAX_PLAYERS; i++)
        {
            for (int j = 0; j < NUM_LEVELS + 1; j++)
            {
                window->save->progress[i][j] = 0;
                window->save->score[i][j] = 0;
            }
        }

        return;
    }

    // Read progress
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        for (int j = 0; j < NUM_LEVELS + 1; j++)
        {
            SDL_RWread(f, &window->save->progress[i][j], sizeof(window->save->progress[i][j]), 1);
            SDL_RWread(f, &window->save->score[i][j], sizeof(window->save->score[i][j]), 1);
        }
    }

    SDL_RWclose(f);
}


void write_save(struct window *window, struct save *save)
{
    SDL_RWops *f = SDL_RWFromFile("save.bin", "wb");
    if (!f)
        error("save.bin", "Could not open/create save file", window->window, window->renderer);

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        for (int j = 0; j < NUM_LEVELS + 1; j++)
        {
            SDL_RWwrite(f, &save->progress[i][j], sizeof(save->progress[i][j]), 1);
            SDL_RWwrite(f, &save->score[i][j], sizeof(save->score[i][j]), 1);
        }
    }

    SDL_RWclose(f);
}
