#include "vector.h"
#include "utils.h"
#include "init.h"
#include <stdlib.h>
#include <string.h>


struct vector *vector_create(struct window *window)
{
    struct vector *vector = xmalloc(sizeof(struct vector), window->window, window->renderer);

    vector->size = 0;
    vector->capacity = 4;
    vector->index = 0;
    vector->data = xcalloc(vector->capacity, sizeof(struct path), window->window, window->renderer);

    return vector;
}


static struct path *vector_double_capacity(struct vector *vector, struct window *window)
{
    vector->capacity *= 2;
    return xrealloc(vector->data, vector->capacity * sizeof(struct path), window->window, window->renderer);
}


void vector_add_path(struct vector *vector, const struct path *p, struct window *window)
{
    // Could transform the while into an if
    while (vector->size >= vector->capacity)
        vector->data = vector_double_capacity(vector, window);

    if (p->type == TITLE)
    {
        vector->data[vector->size].type = TITLE;
        strcpy(vector->data[vector->size].line.title, p->line.title);
    }
    else if (p->type == OBJECT)
    {
        vector->data[vector->size].type = OBJECT;
        vector->data[vector->size].line.type = p->line.type;
    }
    else if (p->type == ENEMY)
    {
        vector->data[vector->size].type = ENEMY;
        vector->data[vector->size].line.enemy_path = p->line.enemy_path;
    }

    vector->size++;
}


void free_vector(struct vector *vector)
{
    free(vector->data);
    free(vector);
}
