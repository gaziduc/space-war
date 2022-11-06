#ifndef STRING_VEC_H
#define STRING_VEC_H

#include <stddef.h>
#include "init.h"

struct string_vec
{
    char *ptr;
    size_t size;
    size_t capacity;
};

struct string_vec* create_string(struct window *window);

void add_string(struct window *window, struct string_vec *str, const char *to_add);

void add_bytes(struct window* window, struct string_vec* str, const char* to_add, long bytes_size);

void free_string(struct string_vec *str);

#endif // STRING_VEC_H
