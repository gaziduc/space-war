#ifndef PATH_H
#define PATH_H

#include "init.h"
#define NUM_FIELDS 4

struct path
{
    Uint32 time_to_wait;
    int pos_y;
    int speed_x;
    int health;
};

struct vector *load_paths(struct window *window, char *filename);

#endif
