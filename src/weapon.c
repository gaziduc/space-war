#include "init.h"
#include "list.h"
#include "collision.h"
#include "weapon.h"
#include <SDL2/SDL.h>

void shoot(struct window *window, struct player *player)
{
    if (player->ammo == -1 || player->ammo > 0)
    {
        list_push_front(&player->pos, window, MY_SHOTS_LIST, NULL, NULL, 0, 0);
        Mix_PlayChannel(-1, window->sounds->shot, 0);

        if (player->ammo > 0)
            player->ammo--;
    }
}


void bomb(struct window *window)
{
    /* NORMAL ENEMY */
    struct list *sentinel = window->list[ENEMY_LIST];

    while (sentinel->next)
    {
        struct list *enemy_to_delete = sentinel->next;
        sentinel->next = sentinel->next->next;

        // Destroy enemy
        list_push_front(&enemy_to_delete->pos_dst, window, EXPLOSION_LIST,
                        window->img->enemy->texture, NULL, 0, 0);
        list_push_front(&enemy_to_delete->pos_dst, window, HUD_LIST, NULL,
                        NULL, 0, 0);

        free(enemy_to_delete);
        Mix_PlayChannel(-1, window->sounds->explosion, 0);

        // Increase score
        window->score += SCORE_TO_INCREASE;
    }


    /* BOSS */
    sentinel = window->list[BOSS_LIST];

    while (sentinel->next)
    {
        sentinel->next->health -= BOMB_DAMAGE;

        // Explosion
        list_push_front(&sentinel->next->pos_dst, window, EXPLOSION_LIST,
                        window->img->boss->texture, NULL, 0, 0);
        Mix_PlayChannel(-1, window->sounds->explosion, 0);


        if (sentinel->next->health <= 0)
        {
            struct list *boss_to_delete = sentinel->next;
            sentinel->next = sentinel->next->next;

            list_push_front(&boss_to_delete->pos_dst, window, HUD_LIST, NULL,
                            NULL, 0, 0);

            free(boss_to_delete);

            // Increase score
            window->score += SCORE_TO_INCREASE;
        }
        else
        {
            sentinel->next->last_time_hurt = SDL_GetTicks();
            sentinel = sentinel->next;
        }
    }
}
