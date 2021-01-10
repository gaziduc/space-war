#ifndef PATH_H
#define PATH_H

#include "init.h"

#define NUM_FIELDS_TITLE 1
#define NUM_FIELDS_OBJECT 1
#define NUM_FIELDS_ENEMY 5
#define NUM_FIELDS_TEXT 1

enum path_line_type
{
    TITLE = 0,
    ENEMY,
    OBJECT,
    PERSISTENT_TEXT,
};

struct enemy_path
{
    int pos_y;
    int speed_x;
    int health;
    Uint32 time_to_wait;
    char enemy_type;
};

union path_line
{
    char title[128];
    struct enemy_path enemy_path;
    enum object_type type;
    int tutorial_num;
};

struct path
{
    enum path_line_type type;
    union path_line line;
};

struct vector *load_paths(struct window *window, char *filename);

int execute_path_action(struct window *window);

#endif
