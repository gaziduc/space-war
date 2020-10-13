#ifndef VECTOR_H
#define VECTOR_H

#include "path.h"
#include "init.h"
#include <stddef.h>

struct vector *vector_create(struct window *window);

void vector_add_path(struct vector *vector, const struct path *p, struct window *window);

void free_vector(struct vector *vector);

#endif
