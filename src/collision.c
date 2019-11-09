#include "init.h"
#include "list.h"

static int collision(SDL_Rect *pos_dst1, SDL_Rect *pos_dst2)
{
    if (pos_dst1->x > pos_dst2->x + pos_dst2->w
    || pos_dst1->x + pos_dst1->w < pos_dst2->x
    || pos_dst1->y > pos_dst2->y + pos_dst2->h
    || pos_dst1->y + pos_dst1->h < pos_dst2->y)
        return 0;

    return 1;
}


void check_collisions(struct window *window)
{
    struct list *temp_shot = window->list[SHOTS_LIST]->next;
    struct list *prev_shot = window->list[SHOTS_LIST];

    while (temp_shot)
    {
        struct list *temp_enemy = window->list[ENEMY_LIST]->next;
        struct list *prev_enemy = window->list[ENEMY_LIST];
        int deleted = 0;

        while (temp_enemy)
        {
            // If collision, delete both elements
            if (collision(&temp_shot->pos_dst, &temp_enemy->pos_dst))
            {
                struct list *shot_to_delete = temp_shot;
                prev_shot->next = temp_shot->next;
                free(shot_to_delete);

                struct list *enemy_to_delete = temp_enemy;
                prev_enemy->next = temp_enemy->next;
                free(enemy_to_delete);

                list_push_front(&temp_enemy->pos_dst, window, EXPLOSION_LIST);

                deleted = 1;
                break;
            }
            else
            {
                // Go to next enemy
                prev_enemy = temp_enemy;
                temp_enemy = temp_enemy->next;
            }
        }

        // Go to next shot
        if (deleted)
        {
            temp_shot = prev_shot->next;
            deleted = 0;
        }
        else
        {
            prev_shot = temp_shot;
            temp_shot = temp_shot->next;
        }
    }
}
