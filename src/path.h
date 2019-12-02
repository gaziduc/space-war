#ifndef PATH_H
#define PATH_H

#include "init.h"
#define NUM_FIELDS 3

struct path
{
    Uint32 time_to_wait;
    int pos_y;
    int speed_x;
};

struct vector *load_paths(struct window *window, char *filename);

#endif
