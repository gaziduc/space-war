#include "init.h"
#include "list.h"
#include "collision.h"
#include "weapon.h"
#include "menu.h"
#include "utils.h"
#include "event.h"
#include "game.h"
#include "ready.h"
#include "net.h"
#include "effect.h"
#include "trophies.h"
#include "save.h"
#include <stdio.h>
#include <SDL2/SDL.h>

int shoot(struct window *window, struct player *player, int provoked_by_me)
{
    if (player->ammo == -1 || player->ammo > 0)
    {
        if (player->missile_around) // double missile
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

            if (window->is_lan && provoked_by_me)
            {
                struct msg msg = { .type = SHOOT_MSG };
                msg.content.boolean = 2;
                send_msg(window, &msg);
            }
        }
        else
        {
            list_push_front(&player->pos, window, MY_SHOTS_LIST, NULL, NULL, 0, 0);

            Mix_PlayChannel(-1, window->sounds->shot, 0);

            if (player->ammo > 0)
                player->ammo--;

            if (window->is_lan && provoked_by_me)
            {
                struct msg msg = { .type = SHOOT_MSG };
                msg.content.boolean = 1;
                send_msg(window, &msg);
            }
        }

        return 1;
    }

    return 0;
}


void bomb(struct window *window, int provoked_by_me)
{
    /* NORMAL ENEMY */
    struct list *sentinel = window->list[ENEMY_LIST];

    int num_enemies_destroyed = 0;

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
        num_enemies_destroyed++;
    }


    /* BOSS */
    sentinel = window->list[BOSS_LIST];

    while (sentinel->next)
    {
        sentinel->next->health -= BOMB_DAMAGE;

        // Explosion
        list_push_front(&sentinel->next->pos_dst, window, EXPLOSION_LIST,
                        sentinel->next->texture.texture->texture, NULL, 0, 0);
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
            num_enemies_destroyed++;
        }
        else
        {
            Uint32 ticks = SDL_GetTicks();

            if (ticks - sentinel->next->last_time_hurt >= 1692)
                sentinel->next->first_time_hurt = ticks;

            sentinel->next->last_time_hurt = ticks;

            sentinel = sentinel->next;
        }
    }

    if (num_enemies_destroyed >= 20 && !window->save->trophies[PERFECT_BOMB])
    {
        achieve_trophy(window, PERFECT_BOMB);
        write_save(window, window->save);
    }


    // Set quake effect
    set_shake_effect(window);

    if (window->is_lan && provoked_by_me)
    {
        struct msg msg = { .type = BOMB_MSG };
        send_msg(window, &msg);
    }
}



static void render_weapons(struct window *window, int choice, Uint32 begin)
{
    Uint32 alpha = SDL_GetTicks() - begin;

    if (alpha > TITLE_ALPHA_MAX)
        alpha = TITLE_ALPHA_MAX;
    else if (alpha == 0)
        alpha = 1;

    SDL_Color orange = { 255, 127, 39, alpha };
    SDL_Color green = { GREEN_R, GREEN_G, GREEN_B, alpha };
    SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = alpha };

    render_text(window, window->fonts->zero4b_30_small, window->txt[CHOOSE_WEAPON],
                orange, 150, 150);

    for (int i = 1; i <= 3; i++)
    {
        int w = 0;
        int h = 0;
        SDL_QueryTexture(window->img->shot[i - 1]->texture, NULL, NULL, &w, &h);

        SDL_Rect pos_shot = { .x = 225 - w / 2,
                              .y = 370 - h / 2 + (i - 1) * 200,
                              .w = w,
                              .h = h
                            };

        resize_pos_for_resolution(window, &pos_shot);

        SDL_RenderCopy(window->renderer, window->img->shot[i - 1]->texture, NULL, &pos_shot);

        char s[128] = { 0 };
        sprintf(s, window->txt[WEAPON_CONDITIONS],
                get_weapon_damage(i - 1),
                get_weapon_speed(i - 1),
                get_weapon_shots_per_second(i - 1));

        render_text(window, window->fonts->zero4b_30_extra_small,
                    s, choice == i ? green : blue, 350, 350 + (i - 1) * 200);

        if (i == choice)
        {
            SDL_Rect pos = { .x = 150, .y = 320 + (i - 1) * 200, .w = 150, .h = 100 };
            resize_pos_for_resolution(window, &pos);

            SDL_SetRenderDrawColor(window->renderer, 0, 128, 255, alpha);
            SDL_RenderDrawRect(window->renderer, &pos);
        }
    }

    render_text(window, window->fonts->zero4b_30_extra_small, window->txt[BACK_4],
                choice == 4 ? green : blue, 150, 900);
}


void choose_weapons(struct window *window, int selected_level, int selected_difficulty, const char *str)
{
    int escape = 0;
    Uint32 begin = SDL_GetTicks();
    window->weapon = 0;
    SDL_Rect areas[4];

    for (unsigned i = 0; i < 4; i++)
    {
        areas[i].x = 150;

        char s[128] = { 0 };

        if (i < 3)
        {
            areas[i].y = 320 + i * 200;
            areas[i].w = 1620;
            areas[i].h = 100;
        }
        else
        {
            areas[i].y = 900;
            sprintf(s, "%s", window->txt[BACK_4]);
            TTF_SizeText(window->fonts->zero4b_30_extra_small, s, &areas[i].w, &areas[i].h);
        }
    }

    while (!escape)
    {
        // Get and handle events
        update_events(window->in, window, 0);
        handle_quit_event(window, 0);
        handle_select_arrow_event(window, &window->weapon, 4, areas);

        if (window->weapon > 0 && handle_play_event(window))
        {
            if (window->weapon <= 3)
            {
                // Decrement window->weapon to get a 0 based number
                window->weapon--;

                int was_ready = ready(window, selected_level, selected_difficulty, str);

                window->weapon++;

                if (was_ready)
                    return;

                begin = SDL_GetTicks();
            }
            else if (window->weapon == 4)
                escape = 1;
        }

        escape = escape || handle_escape_event(window);

        // Display black bachground
        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);

        // Process/Draw all the things
        render_stars(window);
        render_weapons(window, window->weapon, begin);
        render_controller_input_texts(window, begin, 1);
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
            return 30;

        case 1:
            return 20;

        case 2:
            return 40;

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
