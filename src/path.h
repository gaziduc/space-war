#ifndef PATH_H
#define PATH_H

#include "init.h"

#define NUM_FIELDS_TITLE 1
#define NUM_FIELDS_ENEMY 4

enum path_line_type
{
    TITLE = 0,
    ENEMY,
};

struct enemy_path
{
    int pos_y;
    int speed_x;
    int health;
    Uint32 time_to_wait;
};

union path_line
{
    char title[128];
    struct enemy_path enemy_path;
};

struct path
{
    enum path_line_type type;
    union path_line line;
};

struct vector *load_paths(struct window *window, char *filename);

void execute_path_action(struct window *window);

#endif
