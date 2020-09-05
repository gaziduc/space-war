#include "init.h"
#include "event.h"
#include "utils.h"
#include "game.h"
#include "list.h"
#include "shot.h"
#include "enemy.h"
#include "explosion.h"
#include "collision.h"
#include "hud.h"
#include "path.h"
#include "weapon.h"
#include "end.h"
#include "vector.h"
#include "background.h"
#include "level.h"
#include "object.h"
#include "pause.h"
#include "net.h"
#include "effect.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>

// LAN
struct state state;
int shots_to_handle;


static void handle_arrow_event(struct window *window, struct player *player)
{
    /* Move ship */

    if (player->is_controller)
    {
        // Up
        if (window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_UP])
            player->pos.y -= SHIP_SPEED;
        else if (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value <= -DEAD_ZONE)
            player->pos.y += window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value * SHIP_SPEED / 32768;


        // Down
        if (window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_DOWN])
            player->pos.y += SHIP_SPEED;
        else if (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value >= DEAD_ZONE)
            player->pos.y += window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value * SHIP_SPEED / 32767;


        // Left
        if (window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_LEFT])
            player->pos.x -= SHIP_SPEED;
        else if (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value <= -DEAD_ZONE)
            player->pos.x += window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value * SHIP_SPEED / 32768;


        // Right
        if (window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT])
            player->pos.x += SHIP_SPEED;
        else if (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value >= DEAD_ZONE)
            player->pos.x += window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value * SHIP_SPEED / 32767;
    }
    else
    {
        if (window->in->key[SDL_SCANCODE_UP])
            player->pos.y -= SHIP_SPEED;
        if (window->in->key[SDL_SCANCODE_DOWN])
            player->pos.y += SHIP_SPEED;
        if (window->in->key[SDL_SCANCODE_LEFT])
            player->pos.x -= SHIP_SPEED;
        if (window->in->key[SDL_SCANCODE_RIGHT])
            player->pos.x += SHIP_SPEED;
    }


    // Prevent out of bounds
    if (player->pos.y < 0)
        player->pos.y = 0;
    else if (player->pos.y > DEFAULT_H - player->pos.h)
        player->pos.y = DEFAULT_H - player->pos.h;

    if (player->pos.x < 0)
        player->pos.x = 0;
    else if (player->pos.x > DEFAULT_W - player->pos.w)
        player->pos.x = DEFAULT_W - player->pos.w;
}


static int try_to_shoot(struct window *window, struct player *player)
{
    Uint32 current_time = SDL_GetTicks();

    // If enough time between now and the last shot
    if (current_time - player->last_shot_time >= (Uint32) (1000 / get_weapon_shots_per_second(window->weapon)))
    {
        // Shot
        player->last_shot_time = current_time;
        return shoot(window, player);
    }

    return 0;
}


static int handle_shot_event(struct window *window, struct player *player)
{
    if (player->health > 0)
    {
        if (player->is_controller)
        {
            if (window->in->c.axis[SDL_CONTROLLER_AXIS_TRIGGERRIGHT].value >= DEAD_ZONE)
                return try_to_shoot(window, player);
        }
        else
        {
            if (window->in->key[SDL_SCANCODE_SPACE])
                return try_to_shoot(window, player);
        }
    }

    return 0;
}


static int handle_bomb_event(struct window *window, struct player *player)
{
    if (player->health > 0 && window->num_bombs > 0)
    {
        if (player->is_controller)
        {
            if (window->in->c.button[SDL_CONTROLLER_BUTTON_B])
            {
                window->in->c.button[SDL_CONTROLLER_BUTTON_B] = 0;

                // Bomb: erase all visible enemies
                bomb(window);
                window->num_bombs--;
                return 1;
            }
        }
        else
        {
            if (window->in->key[SDL_SCANCODE_C])
            {
                window->in->key[SDL_SCANCODE_C] = 0;

                // Bomb: erase all visible enemies
                bomb(window);
                window->num_bombs--;
                return 1;
            }
        }
    }

    return 0;
}


void render_trail(struct window *window, struct player *player, SDL_Rect *pos,
                  int is_lan_player, int is_enemy)
{
    // If ship is dead, don't display trail
    if (!is_enemy && !is_lan_player && player->health <= 0)
        return;

    SDL_Rect pos_dst_trail;
    SDL_QueryTexture(window->img->trail, NULL, NULL, &pos_dst_trail.w, &pos_dst_trail.h);

    int count = window->fps->framecount % 10;

    if (count < 5)
        pos_dst_trail.x = pos->x - pos_dst_trail.w / 2 + count * 2;
    else
    {
        count -= 5;
        pos_dst_trail.x = pos->x - pos_dst_trail.w / 2 + 10 - count * 2;
    }

    pos_dst_trail.y = pos->y + pos->h / 2 - pos_dst_trail.h / 2;

    SDL_RendererFlip flip = 0;

    if (is_enemy)
    {
        pos_dst_trail.x += pos->w;
        flip |= SDL_FLIP_HORIZONTAL;
    }
    else
        pos_dst_trail.x -= 5;

    resize_pos_for_resolution(window, &pos_dst_trail);

    SDL_RenderCopyEx(window->renderer, window->img->trail, NULL, &pos_dst_trail,
                     0, NULL, flip);
}


static int respawn(struct window *window, struct player *player)
{
    // If already dead (e.g. a enemy managed to pass to the left of the screen)
    if (player->lives <= 0)
        return 1;

    if (player->health <= 0)
    {
        player->respawn_frame++;

        if (player->respawn_frame >= 90)
        {
            if (player->lives > 1)
            {
                player->health = window->max_health;

                player->respawn_frame = 0;
                init_position(120, POS_CENTERED, window->img->ship->texture, &player->pos);

                player->lives--;
            }
            else
                return 1;
        }
    }

    return 0;
}


void reset_game_attributes(struct window *window, int difficulty, int all_reset)
{
    if (all_reset)
    {
        for (enum list_type i = 0; i < NUM_LISTS; i++)
            clear_list(window->list[i]);

        window->score = 0;
        window->touched_anim = 0;
    }

    window->last_enemy_time = 0;
    window->is_wave_title = 0;
    window->wave_title_time = 0;

    for (int i = 0; i < window->num_players; i++)
    {
        window->player[i].respawn_frame = 0;

        if (all_reset)
            for (int i = 0; i < window->num_players; i++)
                window->player[i].missile_around = 0;
    }

    switch (difficulty)
    {
        case EASY:
            if (all_reset)
                for (int i = 0; i < window->num_players; i++)
                    window->player[i].health = MAX_HEALTH_EASY;

            for (int i = 0; i < window->num_players; i++)
                window->player[i].ammo = -1;

            window->num_bombs = 2;
            window->bonus = 0;
            break;

        case HARD:
            if (all_reset)
                for (int i = 0; i < window->num_players; i++)
                    window->player[i].health = MAX_HEALTH_HARD;

            for (int i = 0; i < window->num_players; i++)
                window->player[i].ammo = -1;

            window->num_bombs = 1;
            window->bonus = 1500;
            break;

        case REALLY_HARD:
            if (all_reset)
                for (int i = 0; i < window->num_players; i++)
                    window->player[i].health = MAX_HEALTH_REALLY_HARD;

            for (int i = 0; i < window->num_players; i++)
                window->player[i].ammo = 200;

            window->num_bombs = 1;
            window->bonus = 3000;
            break;

        default:
            error("Unknown difficulty level", "Unknown difficulty level", window->window, window->renderer);
            break;
    }

    if (all_reset)
    {
        window->max_health = window->player[0].health;

        for (int i = 0; i < window->num_players; i++)
        {
            window->player[i].animated_health_low = window->player[i].health;
            window->player[i].animated_health_high = window->player[i].health;
        }
    }

    for (int i = 0; i < window->num_players; i++)
    {
        window->player[i].lives = 1;
        window->player[i].shield_time = -SHIELD_TIME;
    }
}


static void render_ship(struct window *window, SDL_Rect *temp_pos)
{
    SDL_Rect pos = { .x = temp_pos->x,
                     .y = temp_pos->y,
                     .w = temp_pos->w,
                     .h = temp_pos->h
                   };

    resize_pos_for_resolution(window, &pos);

    SDL_RenderCopy(window->renderer, window->img->ship->texture, NULL, &pos);
}


static void load_correct_music(struct window *window, int mission_num, int is_arcade)
{
    if (is_arcade)
    {
        if (mission_num == 1)
            load_music(window, "data/pirates.ogg", 1);
        else if (mission_num == 4)
            load_music(window, "data/battle.ogg", 1);
        else if (mission_num == 7)
            load_music(window, "data/planet.ogg", 1);
    }
    else
    {
        if (mission_num < 4)
            load_music(window, "data/pirates.ogg", 1);
        else if (mission_num < 7)
            load_music(window, "data/battle.ogg", 1);
        else
            load_music(window, "data/planet.ogg", 1);
    }
}


void play_game(struct window *window, int mission_num, int difficulty)
{
    int is_arcade = 0;

    // Set mode to arcade if necessary
    if (mission_num == NUM_LEVELS + 1)
    {
        is_arcade = 1;
        mission_num = 1;
    }

    // Load enemy paths and set enemy timer
    char s[50] = { 0 };
    sprintf(s, "data/level%d.txt", mission_num);
    window->paths = load_paths(window, s);

    int escape = 0;
    int retry = 1;

    // Thread for LAN
    if (window->is_lan)
        SDL_CreateThread(recv_thread, "recv_thread", window);

    while (!escape)
    {
        reset_game_attributes(window, difficulty, retry);

        if (retry)
        {
            load_correct_music(window, mission_num, is_arcade);
            init_background(window);

            for (int i = 0; i < window->num_players; i++)
                init_position(120, 250 + i * 500, window->img->ship->texture, &window->player[i].pos);

            retry = 0;
        }

        window->paths->index = 0;

        int dead = 0;
        int won = 0;
        unsigned long framecount = 0;

        while (!escape && !dead && !won)
        {
            // Get and handle events
            update_events(window->in, window);
            handle_quit_event(window, 1);
            if (handle_focus_lost_event(window))
               escape = pause(window);
            else if (handle_escape_event(window))
            {
                if (!window->is_lan)
                    escape = pause(window);
                else
                {
                    // Quit
                    send_state(&window->player[0], window, 0, 0, 0, 1);
                    free_background(window->stars);
                    free_vector(window->paths);
                    load_music(window, "data/endgame.ogg", 1);
                    return;
                }
            }

            int is_shooting = 0;
            int is_throwing_bomb = 0;

            for (int i = 0; i < window->num_players; i++)
            {
                handle_arrow_event(window, &window->player[i]);
                is_shooting += handle_shot_event(window, &window->player[i]);
                is_throwing_bomb += handle_bomb_event(window, &window->player[i]);
            }

            // Move elements and background
            move_shots(window);
            move_enemies(window);
            move_explosions(window);
            move_enemy_shots(window);
            move_objects(window);
            move_hud_texts(window);
            move_background(window, framecount);

            // LAN only
            if (window->is_lan)
            {
                send_state(&window->player[0], window, is_shooting, is_throwing_bomb, 0, 0);

                if (state.quit)
                {
                    // Quit
                    free_background(window->stars);
                    free_vector(window->paths);
                    load_music(window, "data/endgame.ogg", 1);
                    return;
                }

                window->player[1].pos.x = state.pos_x;
                window->player[1].pos.y = state.pos_y;
                window->player[1].health = state.health;
                window->player[1].ammo = state.ammo;
                if (window->player[1].ammo == 1000)
                    window->player[1].ammo = -1;

                if (shots_to_handle)
                {
                    shoot(window, &window->player[1]);
                    shots_to_handle = 0;
                }

                if (state.throw_bomb)
                {
                    bomb(window);
                    window->num_bombs--;
                }

                if (state.has_shield)
                    window->player[1].shield_time = SDL_GetTicks();
            }

            for (int i = 0; i < window->num_players; i++)
            {
                // Check collisions
                check_collisions(window, &window->player[i]);

                // If dead, wait some frames and respawn
                dead = respawn(window, &window->player[i]);
                if (dead)
                    break;
            }

            // Display textures
            if (mission_num <= 6)
                SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
            else
                SDL_SetRenderDrawColor(window->renderer, 192, 128, 0, 255);
            SDL_RenderClear(window->renderer);
            render_background(window);
            render_objects(window);
            for (int i = 0; i < window->num_players; i++)
                render_trail(window, &window->player[i], &window->player[i].pos, 0, 0);
            render_enemies_health(window);
            render_shots(window);
            render_enemy_shots(window);
            render_enemies(window);
            for (int i = 0; i < window->num_players; i++)
            {
                if (window->player[i].health > 0)
                {
                    render_shield_aura(window, &window->player[i]);
                    render_ship(window, &window->player[i].pos);
                }
            }

            render_explosions(window);
            render_touched_effect(window);
            render_hud_texts(window);
            render_hud(window);

            // Create enemies, display wave titles...
            won = execute_path_action(window) && !window->list[BOSS_LIST]->next
                                              && !window->list[EXPLOSION_LIST]->next;
            SDL_RenderPresent(window->renderer);

            // Wait a frame
            SDL_framerateDelay(window->fps);
            framecount++;
        }


        if (won)
        {
            if (is_arcade && mission_num < NUM_LEVELS)
            {
                free_vector(window->paths);

                mission_num++;
                sprintf(s, "data/level%d.txt", mission_num);
                window->paths = load_paths(window, s);

                load_correct_music(window, mission_num, is_arcade);
            }
            else
            {
                free_background(window->stars);

                success(window, is_arcade ? NUM_LEVELS + 1 : mission_num, difficulty);
                escape = 1;
            }
        }

        else if (dead)
        {
            free_background(window->stars);
            if (is_arcade)
                mission_num = 1;

            escape = failure(window, is_arcade ? NUM_LEVELS + 1 : mission_num);
            if (!escape)
            {
                retry = 1;
                if (is_arcade)
                {
                    free_vector(window->paths);
                    window->paths = load_paths(window, "data/level1.txt");
                }
            }
        }
    }

    free_vector(window->paths);
    load_music(window, "data/endgame.ogg", 1);
}

int recv_thread(void *data)
{
    struct window *window = data;

    do
    {
        recv_state(window, &state);
        if (state.is_shooting)
            shots_to_handle = 1;
    } while (!state.quit);

    return 0;
}
