#include "init.h"
#include "list.h"
#include "collision.h"
#include "weapon.h"
#include "menu.h"
#include "utils.h"
#include "event.h"
#include "game.h"
#include <SDL2/SDL.h>

int shoot(struct window *window, struct player *player)
{
    if (player->ammo == -1 || player->ammo > 0)
    {
        if (player->missile_around)
        {
            SDL_Rect missile_pos = player->pos;

            missile_pos.y += 15;
            list_push_front(&missile_pos, window, MY_SHOTS_LIST, NULL, NULL, 0, 0);
            Mix_PlayChannel(-1, window->sounds->shot, 0);

            if (player->ammo > 0)
                player->ammo--;

            if (player->ammo == -1 || player->ammo > 0)
            {
                missile_pos.y = player->pos.y - 15;
                list_push_front(&missile_pos, window, MY_SHOTS_LIST, NULL, NULL, 0, 0);
                Mix_PlayChannel(-1, window->sounds->shot, 0);

                if (player->ammo > 0)
                    player->ammo--;
            }
        }
        else
        {
            list_push_front(&player->pos, window, MY_SHOTS_LIST, NULL, NULL, 0, 0);

            Mix_PlayChannel(-1, window->sounds->shot, 0);

            if (player->ammo > 0)
                player->ammo--;
        }

        return 1;
    }

    return 0;
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



static void render_weapons(struct window *window, int choice, Uint32 begin)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color orange = { 255, 128, 0, alpha };

    render_text(window, window->fonts->zero4b_30_small, "CHOOSE WEAPON",
                orange, 150, 150);

    for (int i = 1; i <= 3; i++)
    {
        int w = 0;
        int h = 0;
        SDL_QueryTexture(window->img->shot[i - 1]->texture, NULL, NULL, &w, &h);

        SDL_Rect pos_shot = { .x = 225 - w / 2,
                              .y = 375 - h / 2 + (i - 1) * 200,
                              .w = w,
                              .h = h
                            };

        resize_pos_for_resolution(window, &pos_shot);

        SDL_RenderCopy(window->renderer, window->img->shot[i - 1]->texture, NULL, &pos_shot);

        char s[128] = { 0 };
        sprintf(s, "DAMAGE: %d - SPEED: %d - SHOTS/SECOND: %d",
                get_weapon_damage(i - 1),
                get_weapon_speed(i - 1),
                get_weapon_shots_per_second(i - 1));

        render_text(window, window->fonts->zero4b_30_extra_small,
                    s, orange, 350, 350 + (i - 1) * 200);

        if (i == choice)
        {
            SDL_Rect pos = { .x = 150, .y = 300 + (i - 1) * 200, .w = 150, .h = 150 };
            resize_pos_for_resolution(window, &pos);

            SDL_SetRenderDrawColor(window->renderer, 0, 128, 255, alpha);
            SDL_RenderDrawRect(window->renderer, &pos);
        }
    }
}


void choose_weapons(struct window *window, int selected_level, int selected_difficulty)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();
    window->weapon = 1;

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &window->weapon, 3);

        if (handle_play_event(window))
        {
            window->weapon--;

            if (window->is_lan)
            {
                char data[4] = { selected_level, selected_difficulty, window->weapon, 0 };
                SDLNet_TCP_Send(window->client, data, sizeof(data));
            }

            play_game(window, selected_level, selected_difficulty);
            begin = SDL_GetTicks();
            window->weapon++;
        }

        escape = handle_escape_event(window);

        // Display black bachground
        SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_weapons(window, window->weapon, begin);
        SDL_RenderPresent(window->renderer);

        // Wait a frame
        SDL_framerateDelay(window->fps);
    }
}


int get_weapon_damage(int weapon)
{
    switch (weapon)
    {
        case 0:
            return 1;

        case 1:
            return 2;

        case 2:
            return 3;

        default:
            return 0;
    }
}


int get_weapon_speed(int weapon)
{
    switch (weapon)
    {
        case 0:
            return 20;

        case 1:
            return 15;

        case 2:
            return 30;

        default:
            return 0;
    }
}


int get_weapon_shots_per_second(int weapon)
{
    switch (weapon)
    {
        case 0:
            return 3;

        case 1:
            return 2;

        case 2:
            return 1;

        default:
            return 0;
    }
}
