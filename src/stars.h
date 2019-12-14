#ifndef STARS_H
#define STARS_H

#include "init.h"

#define OPACITY_MAX 8

struct universe
{
    int width;
    int height;
    int depth;
    struct points* points;
    struct points* iterator;
};

void new_universe(struct universe **u, int width, int height, int depth, struct window *window);

void new_point(struct universe *universe, struct window *window);

struct return_point
{
    int x;
    int y;
    int opacity;
};

int process_point(struct universe *u, struct return_point *rp);

#endif /* !STARS_H */
