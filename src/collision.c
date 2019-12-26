#include "init.h"
#include "list.h"
#include "collision.h"
#include "utils.h"

static int collision_aabb(SDL_Rect *pos1, SDL_Rect *pos2)
{
    if (pos1->x > pos2->x + pos2->w
    || pos1->x + pos1->w < pos2->x
    || pos1->y > pos2->y + pos2->h
    || pos1->y + pos1->h < pos2->y)
        return 0;

    return 1;
}


static int collision(SDL_Rect *pos1, struct collision_texture *t1,
                     SDL_Rect *pos2, struct collision_texture *t2)
{
    if (!collision_aabb(pos1, pos2))
        return 0;

    for (int i = pos1->x; i < pos1->x + pos1->w; i++)
    {
        if (i < pos2->x || i >= pos2->x + pos2->w)
            continue;

        for (int j = pos1->y; j < pos1->y + pos1->h; j++)
        {
            if (j < pos2->y || j >= pos2->y + pos2->h)
                continue;

            if (t1->collision[(j - pos1->y) * pos1->w + i - pos1->x]
                && t2->collision[(j - pos2->y) * pos2->w + i - pos2->x])
                return 1;
        }
    }

    return 0;
}


static void check_collisions_list(struct window *window, SDL_Rect *pos,
                                  enum list_type type, struct collision_texture *enemy_texture)
{
    struct list *temp_enemy = window->list[type]->next;
    struct list *prev_enemy = window->list[type];

    while (temp_enemy)
    {
        struct list *temp_shot = window->list[MY_SHOTS_LIST]->next;
        struct list *prev_shot = window->list[MY_SHOTS_LIST];

        int deleted_enemy = 0;

        while (temp_shot)
        {
            // If collision shot <-> enemy
            if (collision(&temp_enemy->pos_dst, enemy_texture,
                          &temp_shot->pos_dst, window->img->shot))
            {
                // Decrease enemy health
                temp_enemy->health--;
                temp_enemy->last_time_hurt = SDL_GetTicks();

                // Add an explosion
                list_push_front(&temp_enemy->pos_dst, window, EXPLOSION_LIST,
                                window->img->enemy->texture, NULL, 0);

                Mix_PlayChannel(-1, window->sounds->explosion, 0);

                // Delete shot
                struct list *shot_to_delete = temp_shot;
                prev_shot->next = temp_shot->next;
                temp_shot = temp_shot->next;
                free(shot_to_delete);

                if (temp_enemy->health <= 0)
                {
                    // Delete enemy
                    struct list *enemy_to_delete = temp_enemy;
                    prev_enemy->next = temp_enemy->next;
                    temp_enemy = temp_enemy->next;
                    free(enemy_to_delete);

                    // Increase score
                    window->score += SCORE_TO_INCREASE;

                    // Exit shot loop
                    deleted_enemy = 1;
                    break;
                }
            }
            else
            {
                // Go to next shot
                prev_shot = temp_shot;
                temp_shot = temp_shot->next;
            }
        }

        // If collision ship <-> enemy
        if (window->health > 0 && !deleted_enemy
            && collision(&temp_enemy->pos_dst, enemy_texture,
                         pos, window->img->ship))
        {
            // Add an explosion
            list_push_front(&temp_enemy->pos_dst, window, EXPLOSION_LIST,
                            window->img->enemy->texture, NULL, 0);

            Mix_PlayChannel(-1, window->sounds->explosion, 0);

            if (type == ENEMY_LIST)
            {
                // Delete enemy
                struct list *enemy_to_delete = temp_enemy;
                prev_enemy->next = temp_enemy->next;
                temp_enemy = temp_enemy->next;
                free(enemy_to_delete);

                deleted_enemy = 1;

                // Increase score
                window->score += SCORE_TO_INCREASE;

                // Decrease health
                window->health -= HEALTH_TO_DECREASE_WHEN_HURT * 5;
            }
            else /* if (type == BOSS_LIST) */
            {
                // Add an explosion
                list_push_front(pos, window, EXPLOSION_LIST, window->img->ship->texture, NULL, 0);
                Mix_PlayChannel(-1, window->sounds->explosion, 0);

                window->health = 0;
            }
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
        if (window->health > 0 &&
            collision(pos, window->img->ship,
                      &temp_enemy_shot->pos_dst, window->img->enemy_shot))
        {
            // Add an explosion
            list_push_front(pos, window, EXPLOSION_LIST, window->img->ship->texture, NULL, 0);

            Mix_PlayChannel(-1, window->sounds->explosion, 0);

            // Delete enemy shot
            struct list *enemy_shot_to_delete = temp_enemy_shot;
            prev_enemy_shot->next = temp_enemy_shot->next;
            temp_enemy_shot = temp_enemy_shot->next;
            free(enemy_shot_to_delete);

            window->health -= HEALTH_TO_DECREASE_WHEN_HURT;
        }
        else
        {
            prev_enemy_shot = temp_enemy_shot;
            temp_enemy_shot = temp_enemy_shot->next;
        }
    }
}


void check_collisions_objects(struct window *window, SDL_Rect *pos)
{
    struct list *temp = window->list[OBJECT_LIST]->next;
    struct list *prev = window->list[OBJECT_LIST];

    while (temp)
    {
        if (window->health > 0 &&
            collision(pos, window->img->ship,
                      &temp->pos_dst, window->img->health))
        {
            // Play sound
            Mix_PlayChannel(-1, window->sounds->power_up, 0);

            // Increase health
            window->health += 30;
            if (window->health > window->max_health)
                window->health = window->max_health;

            // Delete object
            struct list *to_delete = temp;
            temp = temp->next;
            free(to_delete);

            prev->next = temp;
        }
        else
        {
            prev = temp;
            temp = temp->next;
        }
    }
}


void check_collisions(struct window *window, SDL_Rect *pos)
{
    check_collisions_list(window, pos, ENEMY_LIST, window->img->enemy);
    check_collisions_list(window, pos, BOSS_LIST, window->img->boss);
    check_collisions_objects(window, pos);
}
