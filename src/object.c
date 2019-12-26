#include "utils.h"
#include "path.h"
#include "list.h"
#include <SDL2/SDL.h>

void set_object_attributes(struct list *new, enum object_type type, struct window *window)
{
    init_position(window->w, POS_CENTERED, window, window->img->health->texture, &new->pos_dst);

    new->type = type;
    new->framecount = 0;
}

void create_object(struct window *window, enum object_type type)
{
    list_push_front(NULL, window, OBJECT_LIST, NULL, NULL, type, 0);

    window->paths->index++;
}


void move_objects(struct window *window)
{
    struct list *temp = window->list[OBJECT_LIST]->next;
    struct list *prev = window->list[OBJECT_LIST];

    while (temp)
    {
        if (temp->framecount % 2 == 0)
            temp->pos_dst.x--;

        temp->framecount++;

        // Prevent out of bounds by deleting the object if not on screen
        if (temp->pos_dst.x + temp->pos_dst.w <= 0)
        {
            struct list *to_delete = temp;
            prev->next = temp->next;
            free(to_delete);

            // Go to next shot
            temp = prev->next;
        }
        else
        {
            // Go to next shot
            prev = temp;
            temp = temp->next;
        }
    }
}


void render_objects(struct window *window)
{
    struct list *temp = window->list[OBJECT_LIST]->next;

    while (temp)
    {
        // Display object
        SDL_RenderCopy(window->renderer, window->img->health->texture, NULL, &temp->pos_dst);

        // Go to next object
        temp = temp->next;
    }
}
