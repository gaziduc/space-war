#include <stdio.h>
#include <stddef.h>
#include "path.h"
#include "utils.h"
#include "vector.h"
#include "enemy.h"
#include "wave.h"
#include "object.h"


static void replace_underscores(char *s)
{
    for (size_t i = 0; s[i]; i++)
        if (s[i] == '_')
            s[i] = ' ';
}

static int set_path_title(FILE *f, struct path *p)
{
    p->type = TITLE;
    int scan = fscanf(f, " %s\n", p->line.title);
    return scan;
}

static void set_object_type(struct window *window, const char *filename, FILE *f, struct path *p)
{
    p->type = OBJECT;

    char c = 0;
    int scan = fscanf(f, " %c\n", &c);

    if (scan != NUM_FIELDS_OBJECT)
    {
        fclose(f);
        error(filename, "Could not load file because it is corrupted.", window->window);
    }

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

        default:
            error(filename, "Could not load file because it is corrupted.", window->window);
            break;
    }
}



struct vector *load_paths(struct window *window, char *filename)
{
    // Load file
    FILE *f = fopen(filename, "r");
    if (!f)
        error(filename, "Could not load file.", window->window);

    struct vector *vector = vector_create(window);
    int c = 0;

    while ((c = fgetc(f)) != EOF)
    {
        // If line[0] == '#' or '\n', go to next line
        if (c == '#' || c == '\n')
        {
            while (c != EOF && c != '\n')
                c = fgetc(f);

            if (c == EOF)
                break;

            continue;
        }

        struct path p;

        if (c == '$') // Wave title
        {
            int scan = set_path_title(f, &p);

            if (scan != NUM_FIELDS_TITLE)
            {
                fclose(f);
                error(filename, "Could not load file because it is corrupted.", window->window);
            }
            else
                replace_underscores(p.line.title);
        }
        else if (c == '@') // objects
        {
            set_object_type(window, filename, f, &p);
        }
        else
        {
            // If line[0] wasn't a '#', '@', '/', '\n' or a '$', re-read it as part of a number
            fseek(f, -1, SEEK_CUR);

            p.type = ENEMY;

            int scan = fscanf(f, "%u %d %d %d %c\n", &p.line.enemy_path.time_to_wait,
                                              &p.line.enemy_path.pos_y,
                                              &p.line.enemy_path.speed_x,
                                              &p.line.enemy_path.health,
                                              &p.line.enemy_path.enemy_type);

            if (scan != NUM_FIELDS_ENEMY)
            {
                fclose(f);
                error(filename, "Could not load file because it is corrupted.", window->window);
            }
        }

        vector_add_path(vector, &p, window);
    }

    // Close file
    fclose(f);

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
