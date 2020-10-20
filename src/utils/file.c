#include "utils.h"
#include <SDL2/SDL.h>

struct string_vec* dump_file_in_string(char *filename, struct window *window)
{
    // Load file
    SDL_RWops *f = SDL_RWFromFile(filename, "r");
    if (!f)
        return NULL;

    // Dump file content into a string
    struct string_vec *str = create_string(window);
    char buffer[1025] = { 0 };
    size_t read_bytes = 0;

    do
    {
        read_bytes = SDL_RWread(f, buffer, sizeof(char), 1024);
        add_string(window, str, buffer);
    } while (read_bytes == 1024);

    // Close file
    SDL_RWclose(f);

    return str;
}


void go_to_next_line(size_t *index, char *s)
{
    while (s[*index] && s[*index] != '\n')
        (*index)++;

    if (s[*index])
        (*index)++;
}

