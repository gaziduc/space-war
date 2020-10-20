#include <stdio.h>
#include <stddef.h>
#include "path.h"
#include "utils.h"
#include "vector.h"
#include "enemy.h"
#include "wave.h"
#include "object.h"
#include "string_vec.h"


static void go_to_next_line(size_t *index, char *s)
{
    while (s[*index] && s[*index] != '\n')
        (*index)++;

    if (s[*index])
        (*index)++;
}

static void replace_underscores(char *s)
{
    for (size_t i = 0; s[i]; i++)
        if (s[i] == '_')
            s[i] = ' ';
}

static int set_path_title(size_t *index, char *s, struct path *p)
{
    p->type = TITLE;
    int scan = sscanf(s + (*index), " %s\n", p->line.title);

    go_to_next_line(index, s);

    return scan;
}

static void set_object_type(struct window *window, const char *filename, size_t *index, char *s, struct path *p)
{
    p->type = OBJECT;

    char c = 0;
    int scan = sscanf(s + (*index), " %c\n", &c);

    if (scan != NUM_FIELDS_OBJECT)
        error(filename, "Could not load file because it is corrupted.", window->window, window->renderer);

    switch (c)
    {
        case 'H':
            p->line.type = HEALTH;
            break;

        case 'S':
            p->line.type = SHIELD;
            break;

        case 'P':
            p->line.type = PLANET;
            break;

        case 'G':
            p->line.type = GALAXY;
            break;

        case 'M':
            p->line.type = MISSILE_AROUND;
            break;

        default:
            error(filename, "Could not load file because it is corrupted.", window->window, window->renderer);
            break;
    }

    go_to_next_line(index, s);
}



struct vector *load_paths(struct window *window, char *filename)
{
    // Load file
    SDL_RWops *f = SDL_RWFromFile(filename, "r");
    if (!f)
        error(filename, "Could not load file.", window->window, window->renderer);

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

    struct vector *vector = vector_create(window);
    size_t index = 0;

    while (str->ptr[index])
    {
        // If line[0] == '#' or '\n', go to next line
        if (str->ptr[index] == '#' || str->ptr[index] == '\r' || str->ptr[index] == '\n')
        {
            while (str->ptr[index] && str->ptr[index] != '\n')
                index++;

            if (!str->ptr[index])
                break;

            index++;
            continue;
        }

        struct path p;

        if (str->ptr[index] == '$') // Wave title
        {
            index++;

            int scan = set_path_title(&index, str->ptr, &p);

            if (scan != NUM_FIELDS_TITLE)
                error(filename, "Could not load file because it is corrupted.", window->window, window->renderer);
            else
                replace_underscores(p.line.title);
        }
        else if (str->ptr[index] == '@') // objects
        {
            index++;

            set_object_type(window, filename, &index, str->ptr, &p);
        }
        else
        {
            // If line[0] wasn't a '#', '@', '\r', '\n' or a '$'
            p.type = ENEMY;

            int scan = sscanf(str->ptr + index, "%u %d %d %d %c\n", &p.line.enemy_path.time_to_wait,
                                              &p.line.enemy_path.pos_y,
                                              &p.line.enemy_path.speed_x,
                                              &p.line.enemy_path.health,
                                              &p.line.enemy_path.enemy_type);

            if (scan != NUM_FIELDS_ENEMY)
                error(filename, "Could not load file because it is corrupted.", window->window, window->renderer);

            go_to_next_line(&index, str->ptr);
        }

        vector_add_path(vector, &p, window);
    }

    free_string(str);

    return vector;
}


int execute_path_action(struct window *window)
{
    // If not EOF
    if (window->paths->index < window->paths->size)
    {
        enum path_line_type type = window->paths->data[window->paths->index].type;

        if (type == ENEMY)
            create_enemies(window);
        else if (type == OBJECT)
            create_object(window, window->paths->data[window->paths->index].line.type);
        else if (type == TITLE)
            render_wave_title(window);

        return 0;
    }
    else
        return 1;
}
