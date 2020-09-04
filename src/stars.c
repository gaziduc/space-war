#include "stars.h"
#include "utils.h"
#include "init.h"
#include <stdlib.h>


void new_universe(struct universe **u, int depth, struct window *window)
{
    *u = xmalloc(sizeof(struct universe), window->window, window->renderer);
    (*u)->depth = depth;
    (*u)->iterator = NULL;
    (*u)->points = NULL;
}

void new_point(struct universe *u, struct window *window)
{
    struct points *p_ptr = xmalloc(sizeof(struct points), window->window, window->renderer);

    p_ptr->x = (rand() % window->w - window->w / 2) * u->depth;
    p_ptr->y = (rand() % window->h - window->h / 2) * u->depth;
    p_ptr->z = u->depth;
    p_ptr->size = (rand() % 2) + 1;

    p_ptr->next = u->points;
    u->points = p_ptr;
}

static void kill_point(struct universe *universe, struct points* to_kill)
{
    struct points *p_ptr = universe->points;
    struct points *last_ptr = NULL;

    while (p_ptr)
    {
        if (p_ptr == to_kill)
        {
            if (last_ptr == NULL)
                universe->points = p_ptr->next;
            else
                last_ptr->next = p_ptr->next;

            free(p_ptr);
            break;
        }
        else
        {
            last_ptr = p_ptr;
            p_ptr = p_ptr->next;
        }
    }
}


void free_universe(struct universe *u)
{
    struct points *p = u->points;

    while (p)
    {
        struct points *to_delete = p;
        p = p->next;
        free(to_delete);
    }

    free(u);
}


int process_point(struct universe *u, struct return_point *rp, struct window *window)
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

        if (abs(x) >= window->w / 2 || abs(y) >= window->h / 2)
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

            rp->x = x;
            rp->y = y;
            rp->opacity = m;
            rp->size = u->iterator->size;

            u->iterator = u->iterator->next;

            if (u->iterator == NULL)
                return 0;
            else
                return 1;
        }
    }
}
