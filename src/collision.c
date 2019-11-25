#include "init.h"
#include "list.h"

static int collision(SDL_Rect *pos1, SDL_Rect *pos2)
{
    if (pos1->x > pos2->x + pos2->w
    || pos1->x + pos1->w < pos2->x
    || pos1->y > pos2->y + pos2->h
    || pos1->y + pos1->h < pos2->y)
        return 0;

    return 1;
}


void check_collisions(struct window *window, SDL_Rect *pos)
{
    struct list *temp_enemy = window->list[ENEMY_LIST]->next;
    struct list *prev_enemy = window->list[ENEMY_LIST];

    while (temp_enemy)
    {
        struct list *temp_shot = window->list[MY_SHOTS_LIST]->next;
        struct list *prev_shot = window->list[MY_SHOTS_LIST];

        int deleted_enemy = 0;

        while (temp_shot)
        {
            // If collision shot <-> enemy, delete both elements
            if (collision(&temp_shot->pos_dst, &temp_enemy->pos_dst))
            {
                // Add an explosion
                list_push_front(&temp_enemy->pos_dst, window, EXPLOSION_LIST, window->img->enemy);

                // Delete shot
                struct list *shot_to_delete = temp_shot;
                prev_shot->next = temp_shot->next;
                temp_shot = temp_shot->next;
                free(shot_to_delete);

                // Delete enemy
                struct list *enemy_to_delete = temp_enemy;
                prev_enemy->next = temp_enemy->next;
                temp_enemy = temp_enemy->next;
                free(enemy_to_delete);

                // Exit shot loop
                deleted_enemy = 1;
                break;
            }
            else
            {
                // Go to next shot
                prev_shot = temp_shot;
                temp_shot = temp_shot->next;
            }
        }

        // If collision ship <-> enemy, delete enemy
        if (!deleted_enemy && collision(&temp_enemy->pos_dst, pos))
        {
            // Add an explosion
            list_push_front(&temp_enemy->pos_dst, window, EXPLOSION_LIST, window->img->enemy);

            // Delete enemy
            struct list *enemy_to_delete = temp_enemy;
            prev_enemy->next = temp_enemy->next;
            temp_enemy = temp_enemy->next;
            free(enemy_to_delete);

            deleted_enemy = 1;
        }


        if (!deleted_enemy)
        {
            prev_enemy = temp_enemy;
            temp_enemy = temp_enemy->next;
        }
    }


    struct list *temp_enemy_shot = window->list[ENEMY_SHOT_LIST]->next;
    struct list *prev_enemy_shot = window->list[ENEMY_SHOT_LIST];

    while (temp_enemy_shot)
    {
        // If collision ship <-> enemy shot
        if (collision(&temp_enemy_shot->pos_dst, pos))
        {
            // Add an explosion
            list_push_front(pos, window, EXPLOSION_LIST, window->img->ship);

            // Delete enemy shot
            struct list *enemy_shot_to_delete = temp_enemy_shot;
            prev_enemy_shot->next = temp_enemy_shot->next;
            temp_enemy_shot = temp_enemy_shot->next;
            free(enemy_shot_to_delete);
        }
        else
        {
            prev_enemy_shot = temp_enemy_shot;
            temp_enemy_shot = temp_enemy_shot->next;
        }
    }
}
