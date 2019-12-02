#include "vector.h"
#include "utils.h"
#include "init.h"
#include <stdlib.h>


struct vector *vector_create(struct window *window)
{
    struct vector *vector = xmalloc(sizeof(struct vector), window->window);

    vector->size = 0;
    vector->capacity = 4;
    vector->index = 0;
    vector->data = xcalloc(vector->capacity, sizeof(struct path), window->window);

    return vector;
}


static struct path *vector_double_capacity(struct vector *vector, struct window *window)
{
    vector->capacity *= 2;
    return xrealloc(vector->data, vector->capacity * sizeof(struct path), window->window);
}


void vector_add_path(struct vector *vector, const struct path *p, struct window *window)
{
    // Could transform the while into an if
    while (vector->size >= vector->capacity)
        vector->data = vector_double_capacity(vector, window);

    vector->data[vector->size].time_to_wait = p->time_to_wait;
    vector->data[vector->size].pos_y = p->pos_y;
    vector->data[vector->size].speed_x = p->speed_x;

    vector->size++;
}


void free_vector(struct vector *vector)
{
    free(vector->data);
    free(vector);
}
