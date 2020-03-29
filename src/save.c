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
    window->save = xmalloc(sizeof(struct save), window->window);

    // Open file in binary mode for reading
    FILE *f = fopen("save.bin", "rb");

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
            fread(&window->save->progress[i][j], sizeof(window->save->progress[i][j]), 1, f);
            fread(&window->save->score[i][j], sizeof(window->save->score[i][j]), 1, f);
        }
    }

    fclose(f);
}


void write_save(struct window *window, struct save *save)
{
    FILE *f = fopen("save.bin", "wb");
    if (!f)
        error("save.bin", "Could not open/create save file", window->window);

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        for (int j = 0; j < NUM_LEVELS + 1; j++)
        {
            fwrite(&save->progress[i][j], sizeof(save->progress[i][j]), 1, f);
            fwrite(&save->score[i][j], sizeof(save->score[i][j]), 1, f);
        }
    }

    fclose(f);
}
