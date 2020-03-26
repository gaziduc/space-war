#ifndef STARS_H
#define STARS_H

#include "init.h"

#define OPACITY_MAX 8

struct points
{
    int x;
    int y;
    int z;
    struct points *next;
};

struct universe
{
    int depth;
    struct points* points;
    struct points* iterator;
};

void new_universe(struct universe **u, int depth, struct window *window);

void new_point(struct universe *universe, struct window *window);

struct return_point
{
    int x;
    int y;
    int opacity;
};

void free_universe(struct universe *u);

int process_point(struct universe *u, struct return_point *rp, struct window *window);

#endif /* !STARS_H */
