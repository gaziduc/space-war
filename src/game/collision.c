#include "init.h"
#include "list.h"
#include "collision.h"
#include "utils.h"
#include "object.h"
#include "weapon.h"
#include "effect.h"

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


static void check_collisions_list(struct window *window, struct player *player,
                                  enum list_type type)
{
    struct list *temp_enemy = window->list[type]->next;
    struct list *prev_enemy = window->list[type];

    while (temp_enemy)
    {
        struct list *temp_shot = window->list[MY_SHOTS_LIST]->next;
        struct list *prev_shot = window->list[MY_SHOTS_LIST];

        int deleted_enemy = 0;

        // Get current enemy texture (correct animation frame)
        struct collision_texture *temp = NULL;
        SDL_Rect *temp_pos = NULL;
        int to_free = 0;

        if (temp_enemy->rotating)
        {
            temp = temp_enemy->texture.textures[temp_enemy->curr_texture];

            int w = 0;
            int h = 0;
            SDL_QueryTexture(temp->texture, NULL, NULL, &w, &h);

            temp_pos = xmalloc(sizeof(SDL_Rect), window->window, window->renderer);
            to_free = 1;

            temp_pos->x = temp_enemy->pos_dst.x + temp_enemy->pos_dst.w / 2 - w / 2;
            temp_pos->y = temp_enemy->pos_dst.y + temp_enemy->pos_dst.h / 2 - h / 2;
            temp_pos->w = w;
            temp_pos->h = h;
        }
        else
        {
            temp = temp_enemy->texture.texture;
            temp_pos = &temp_enemy->pos_dst;
        }


        while (temp_shot)
        {
            // If collision shot <-> enemy
            if (collision(temp_pos, temp,
                          &temp_shot->pos_dst, window->img->shot[window->weapon]))
            {
                // Decrease enemy health
                temp_enemy->health -= get_weapon_damage(window->weapon);

                Uint32 ticks = SDL_GetTicks();

                if (ticks - temp_enemy->last_time_hurt >= 1692)
                    temp_enemy->first_time_hurt = ticks;

                temp_enemy->last_time_hurt = ticks;

                // Add an explosion
                list_push_front(temp_pos, window, EXPLOSION_LIST,
                                temp->texture, NULL, 0, 0);

                Mix_PlayChannel(-1, window->sounds->explosion, 0);

                // Delete shot
                struct list *shot_to_delete = temp_shot;
                prev_shot->next = temp_shot->next;
                temp_shot = temp_shot->next;
                free(shot_to_delete);

                if (temp_enemy->health <= 0)
                {
                    // Add a "+100" text above the dead enemy
                    list_push_front(temp_pos, window, HUD_LIST,
                                    NULL, NULL, 0, 0);

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
        if (player->health > 0 && !deleted_enemy
            && collision(temp_pos, temp,
                         &player->pos, window->img->ship))
        {
            // Add an explosion
            list_push_front(temp_pos, window, EXPLOSION_LIST,
                            temp->texture, NULL, 0, 0);

            Mix_PlayChannel(-1, window->sounds->explosion, 0);

            if (type == ENEMY_LIST)
            {
                // Add a "+100" text above the dead enemy
                list_push_front(temp_pos, window, HUD_LIST,
                                NULL, NULL, 0, 0);

                // Delete enemy
                struct list *enemy_to_delete = temp_enemy;
                prev_enemy->next = temp_enemy->next;
                temp_enemy = temp_enemy->next;
                free(enemy_to_delete);

                deleted_enemy = 1;

                // Increase score
                window->score += SCORE_TO_INCREASE;

                // Decrease health
                if (SDL_GetTicks() - window->player[0].shield_time < SHIELD_TIME) // If shield
                    player->shield_time = 0;
                else
                    player->health -= HEALTH_TO_DECREASE_WHEN_HURT * 5;

                window->touched_anim = TOUCHED_EFFECT_MAX_ALPHA;

                // Force feedback
                if (window->settings->is_force_feedback && window->in->c.haptic)
                    SDL_HapticRumblePlay(window->in->c.haptic, 0.5, 500);
            }
            else /* if (type == BOSS_LIST) */
            {
                // Add an explosion
                list_push_front(&player->pos, window, EXPLOSION_LIST, window->img->ship->texture,
                                NULL, 0, 0);
                Mix_PlayChannel(-1, window->sounds->explosion, 0);

                player->health = 0;
                window->touched_anim = TOUCHED_EFFECT_MAX_ALPHA;

                // Force feedback
                if (window->settings->is_force_feedback && window->in->c.haptic)
                    SDL_HapticRumblePlay(window->in->c.haptic, 0.75, 750);
            }
        }


        if (!deleted_enemy)
        {
            prev_enemy = temp_enemy;
            temp_enemy = temp_enemy->next;
        }

        if (to_free)
            free(temp_pos);
    }


    if (type == ENEMY_LIST)
    {
        struct list *temp_enemy_shot = window->list[ENEMY_SHOT_LIST]->next;
        struct list *prev_enemy_shot = window->list[ENEMY_SHOT_LIST];

        while (temp_enemy_shot)
        {
            // If collision ship <-> enemy shot
            if (player->health > 0 &&
                collision(&player->pos, window->img->ship,
                          &temp_enemy_shot->pos_dst, window->img->enemy_shot))
            {
                // Delete enemy shot
                struct list *enemy_shot_to_delete = temp_enemy_shot;
                prev_enemy_shot->next = temp_enemy_shot->next;
                temp_enemy_shot = temp_enemy_shot->next;
                free(enemy_shot_to_delete);

                if (SDL_GetTicks() - player->shield_time >= SHIELD_TIME)
                {
                    // Add an explosion
                    list_push_front(&player->pos, window, EXPLOSION_LIST, window->img->ship->texture,
                                    NULL, 0, 0);

                    Mix_PlayChannel(-1, window->sounds->explosion, 0);

                    player->health -= HEALTH_TO_DECREASE_WHEN_HURT;
                    window->touched_anim = TOUCHED_EFFECT_MAX_ALPHA;

                    // force feedback
                    if (window->settings->is_force_feedback && window->in->c.haptic)
                        SDL_HapticRumblePlay(window->in->c.haptic, 0.25, 250);
                }
            }
            else
            {
                prev_enemy_shot = temp_enemy_shot;
                temp_enemy_shot = temp_enemy_shot->next;
            }
        }
    }
}


void check_collisions_objects(struct window *window, struct player *player)
{
    struct list *temp = window->list[OBJECT_LIST]->next;
    struct list *prev = window->list[OBJECT_LIST];

    while (temp)
    {
        if (player->health > 0 &&
            collision(&player->pos, window->img->ship,
                      &temp->pos_dst, temp->texture.texture))
        {
            // If planet or galaxy, go to next
            if (temp->type == PLANET || temp->type == GALAXY)
            {
                prev = temp;
                temp = temp->next;
            }
            else
            {
                // Play sound
                Mix_PlayChannel(-1, window->sounds->power_up, 0);

                // force feedback
                if (window->settings->is_force_feedback && window->in->c.haptic)
                    SDL_HapticRumblePlay(window->in->c.haptic, 0.25, 250);


                switch (temp->type)
                {
                    case HEALTH: // Increase health
                        player->health += 30;
                        if (player->health > window->max_health)
                            player->health = window->max_health;
                        break;

                    case SHIELD: // Activate  shield
                        player->shield_time = SDL_GetTicks();
                        break;

                    case MISSILE_AROUND:
                        player->missile_around = 1;
                        break;

                    default:
                        error("Unknown object", "Unknown object type", window->window, window->renderer);
                        break;
                }

                // Delete object
                struct list *to_delete = temp;
                temp = temp->next;
                free(to_delete);

                prev->next = temp;
            }
        }
        else
        {
            prev = temp;
            temp = temp->next;
        }
    }
}


void check_collisions(struct window *window, struct player *player)
{
    check_collisions_list(window, player, ENEMY_LIST);
    check_collisions_list(window, player, BOSS_LIST);
    check_collisions_objects(window, player);
}
