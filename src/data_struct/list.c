#include "list.h"
#include "utils.h"
#include "shot.h"
#include "enemy.h"
#include "explosion.h"
#include "boss.h"
#include "object.h"
#include "hud.h"

void init_list(struct window *window, enum list_type type)
{
    // Creating list
    window->list[type] = xmalloc(sizeof(struct list), window->window, window->renderer);

    // Sentinel = window->list[type], So we don't care about window->list[type]->pos
    // window->list[type]->next is set to NULL because there is nothing in the list
    window->list[type]->next = NULL;
}

void list_push_front(SDL_Rect *pos, struct window *window,
                     enum list_type type, SDL_Texture *texture,
                     SDL_Rect *ship_pos, enum object_type object,
                     char enemy_type)
{
    // Allocating memory for new shot
    struct list *new = xmalloc(sizeof(struct list), window->window, window->renderer);

    // Setting new->pos depending of arguments (type, ...)
    switch (type)
    {
        case MY_SHOTS_LIST: // Setting shot initial position
            set_shot_pos(new, pos, window);
            break;
        case ENEMY_LIST: // Setting enemy initial position and speed
            set_enemy_attributes(new, pos, window, enemy_type);
            break;
        case EXPLOSION_LIST:
            set_explosion_pos(new, pos, texture);
            break;
        case ENEMY_SHOT_LIST:
            set_enemy_shot_attributes(new, pos, window, ship_pos, enemy_type);
            break;
        case BOSS_LIST:
            set_boss_attributes(new, pos, window, enemy_type);
            break;
        case OBJECT_LIST:
            set_object_attributes(new, object, window->img->objects[object]);
            break;
        case HUD_LIST:
            set_hud_text(new, pos, window);
            break;

        default:
            error("Unknown list index", "Could not add an element in list.", window->window, window->renderer);
            break;
    }

    // Pushing element in front of the correct list
    new->next = window->list[type]->next;
    window->list[type]->next = new;
}

void free_list(struct list *list)
{
    if (list->next)
        free_list(list->next);

    free(list);
}

void clear_list(struct list *list)
{
    if (list->next)
    {
        free_list(list->next);
        list->next = NULL;
    }
}