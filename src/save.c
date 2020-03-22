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
        for (int i = 0; i < NUM_LEVELS + 1; i++)
        {
            window->save->progress[i] = 0;
            window->save->score[i] = 0;
        }

        return;
    }

    // Read progress
    for (int i = 0; i < NUM_LEVELS + 1; i++)
    {
        fread(&window->save->progress[i], sizeof(window->save->progress[i]), 1, f);
        fread(&window->save->score[i], sizeof(window->save->score[i]), 1, f);
    }

    fclose(f);
}


void write_save(struct window *window, struct save *save)
{
    FILE *f = fopen("save.bin", "wb");
    if (!f)
        error("save.bin", "Could not open/create save file", window->window);

    for (int i = 0; i < NUM_LEVELS + 1; i++)
    {
        fwrite(&save->progress[i], sizeof(save->progress[i]), 1, f);
        fwrite(&save->score[i], sizeof(save->score[i]), 1, f);
    }

    fclose(f);
}
