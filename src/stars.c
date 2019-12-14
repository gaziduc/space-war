#include "stars.h"
#include "utils.h"
#include "init.h"
#include <stdlib.h>


struct points
{
    int x;
    int y;
    int z;
    struct points *next;
};

void new_universe(struct universe **u, int width, int height, int depth, struct window *window)
{
    *u = xmalloc(sizeof(struct universe), window->window);

    (*u)->width = width;
    (*u)->height = height;
    (*u)->depth = depth;

    (*u)->iterator = NULL;
    (*u)->points = NULL;
}

void new_point(struct universe *u, struct window *window)
{
    struct points *p_ptr = xmalloc(sizeof(struct points), window->window);

    p_ptr->x = (rand() % u->width - u->width / 2) * u->depth;
    p_ptr->y = (rand() % u->height - u->height / 2) * u->depth;
    p_ptr->z = u->depth;

    p_ptr->next = u->points;
    u->points = p_ptr;

    return;
}

static void kill_point(struct universe *universe, struct points* to_kill)
{
    struct points *p_ptr, *last_ptr = NULL;

    for (p_ptr = universe->points; p_ptr != NULL; p_ptr = p_ptr->next)
    {
        if (p_ptr == to_kill)
        {
            if (last_ptr == NULL)
                universe->points = p_ptr->next;
            else
                last_ptr->next = p_ptr->next;

            free(p_ptr);
        }
        else
            last_ptr = p_ptr;
    }
}

int process_point(struct universe *u, struct return_point *rp)
{
    if (u->iterator == NULL)
    {
        if (u->points == NULL)
            return 0;
        else
            u->iterator = u->points;
    }

    if (u->iterator->z == 0)
    {
        // Delete point that has reached us
        struct points *tmp = u->iterator;
        u->iterator = u->iterator->next;
        kill_point(u, tmp);
        return -1;
    }
    else
    {
        // Plot the point
        int x = u->iterator->x / u->iterator->z;
        int y = u->iterator->y / u->iterator->z;

        if (abs(x) >= u->width / 2 || abs(y) >= u->height / 2)
        {
            // Delete point that is off screen
            struct points *tmp = u->iterator;
            u->iterator = u->iterator->next;
            kill_point(u, tmp);
            if (u->iterator == NULL)
                return 0;
            else
                return -1;
        }
        else
        {
            int m = OPACITY_MAX * ((u->depth - u->iterator->z) * 4) / u->depth;

            if (m > OPACITY_MAX)
                m = OPACITY_MAX;

            u->iterator->z--;
            u->iterator = u->iterator->next;
            rp->x = x;
            rp->y = y;
            rp->opacity = m;

            if (u->iterator == NULL)
                return 0;
            else
                return 1;
        }
    }
}
