#include "init.h"
#include "list.h"
#include "collision.h"
#include <SDL2/SDL.h>

void shoot(struct window *window, SDL_Rect *pos)
{
    if (window->ammo == -1 || window->ammo > 0)
    {
        list_push_front(pos, window, MY_SHOTS_LIST, NULL, NULL, 0, 0);
        Mix_PlayChannel(-1, window->sounds->shot, 0);

        if (window->ammo > 0)
            window->ammo--;
    }
}


void bomb(struct window *window)
{
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
}
