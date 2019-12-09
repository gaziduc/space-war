#include <stdio.h>
#include "path.h"
#include "utils.h"
#include "vector.h"

struct vector *load_paths(struct window *window, char *filename)
{
    // Load file
    FILE *f = fopen(filename, "r");
    if (!f)
        error(filename, "Could not load file.", window->window);

    struct vector *vector = vector_create(window);
    int scan = NUM_FIELDS;

    while (scan == NUM_FIELDS)
    {
        struct path p = { .time_to_wait = 0, .pos_y = -1, .speed_x = 1, .health = 2 };

        int c = fgetc(f);

        // While line[0] == '#' or '\n', go to next line
        while (c == '#' || c == '\n')
        {
            int end_of_line = c == '\n';
            int end_of_file = 0;

            while (!end_of_line && !end_of_file)
            {
                c = fgetc(f);
                end_of_line = c == '\n';
                end_of_file = c == EOF;
            }

            c = fgetc(f);
        }

        // If line[0] wasn't a '#', re-read it as part of first number
        fseek(f, -1, SEEK_CUR);

        scan = fscanf(f, "%u %d %d %d\n", &p.time_to_wait, &p.pos_y, &p.speed_x, &p.health);
        if (scan != NUM_FIELDS)
            break;

        vector_add_path(vector, &p, window);
    }

    if (scan != EOF)
        error(filename, "Could not load file because it is corrupted.", window->window);

    // Close file
    fclose(f);

    return vector;
}
