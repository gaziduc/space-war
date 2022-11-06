#include "string_vec.h"
#include "utils.h"
#include <stdlib.h>

struct string_vec* create_string(struct window *window)
{
    struct string_vec *str = xmalloc(sizeof(struct string_vec), window->window, window->renderer);

    str->size = 0;
    str->capacity = 1024;
    str->ptr = xcalloc(str->capacity, sizeof(char), window->window, window->renderer);

    return str;
}


static void double_string_capacity(struct string_vec *str, struct window *window)
{
    str->capacity *= 2;
    str->ptr = xrealloc(str->ptr, str->capacity * sizeof(char), window->window, window->renderer);
}


void add_string(struct window *window, struct string_vec *str, const char *to_add)
{
    size_t index = 0;

    while (to_add[index])
    {
        // If string_vec too small
        if (str->size + 1 >= str->capacity)
            double_string_capacity(str, window);

        str->ptr[str->size] = to_add[index];

        str->size++;
        index++;
    }

    str->ptr[str->size] = '\0';
}


/*
* Warning: this function is not intended for use as normal string...
*/
void add_bytes(struct window* window, struct string_vec* str, const char* to_add, long bytes_size)
{
    for (size_t index = 0; index < bytes_size; index++)
    {
        // If string_vec too small
        if (str->size + 1 >= str->capacity)
            double_string_capacity(str, window);

        str->ptr[str->size] = to_add[index];

        str->size++;
    }

    str->ptr[str->size] = '\0';
}


void free_string(struct string_vec *str)
{
    free(str->ptr);
    free(str);
}

