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
#include "msg_list.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>




static void handle_arrow_event(struct window *window, struct player *player)
{
    /* Move ship */
    SDL_Point last_player_pos = { .x = player->pos.x, .y = player->pos.y };

    switch (player->input_type)
    {
        case CONTROLLER:
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

            break;

        case KEYBOARD:
            if (window->in->key[window->settings->controls[UP]])
                player->pos.y -= SHIP_SPEED;
            if (window->in->key[window->settings->controls[DOWN]])
                player->pos.y += SHIP_SPEED;
            if (window->in->key[window->settings->controls[LEFT]])
                player->pos.x -= SHIP_SPEED;
            if (window->in->key[window->settings->controls[RIGHT]])
                player->pos.x += SHIP_SPEED;
            break;

        case MOUSE:
            ;
            int cursor_speed_factor = window->settings->mouse_sensitivity + 1;
            SDL_Rect last_pos = player->pos;

            if (window->in->mouse_pos.x - player->pos.x > SHIP_SPEED / cursor_speed_factor)
                player->pos.x += SHIP_SPEED / cursor_speed_factor;
            else if (window->in->mouse_pos.x - player->pos.x < -SHIP_SPEED / cursor_speed_factor)
                player->pos.x -= SHIP_SPEED / cursor_speed_factor;
            else
                player->pos.x = window->in->mouse_pos.x;

            if (window->in->mouse_pos.y - player->pos.y > SHIP_SPEED / cursor_speed_factor)
                player->pos.y += SHIP_SPEED / cursor_speed_factor;
            else if (window->in->mouse_pos.y - player->pos.y < -SHIP_SPEED / cursor_speed_factor)
                player->pos.y -= SHIP_SPEED / cursor_speed_factor;
            else
                player->pos.y = window->in->mouse_pos.y;

            player->pos.x = (player->pos.x - last_pos.x) * cursor_speed_factor + last_pos.x;
            player->pos.y = (player->pos.y - last_pos.y) * cursor_speed_factor + last_pos.y;

            SDL_Rect cursor_pos = player->pos;
            resize_pos_for_resolution(window, &cursor_pos);
            SDL_WarpMouseInWindow(window->window, cursor_pos.x, cursor_pos.y);
            break;

        default:
            break;
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

    if (window->is_lan && (player->pos.x != last_player_pos.x || player->pos.y != last_player_pos.y))
    {
        struct msg msg = { .type = POSITION_MSG };
        msg.content.point.x = player->pos.x;
        msg.content.point.y = player->pos.y;
        send_msg(window, &msg);
    }
}


static int try_to_shoot(struct window *window, struct player *player)
{
    Uint32 current_time = SDL_GetTicks();

    // If enough time between now and the last shot
    if (current_time - player->last_shot_time >= (Uint32) (1000 / get_weapon_shots_per_second(window->weapon)))
    {
        // Shot
        player->last_shot_time = current_time;
        return shoot(window, player, 1);
    }

    return 0;
}


static int handle_shot_event(struct window *window, struct player *player)
{
    if (player->health > 0)
    {
        switch (player->input_type)
        {
            case CONTROLLER:
                if (window->in->c.axis[SDL_CONTROLLER_AXIS_TRIGGERRIGHT].value >= DEAD_ZONE)
                    return try_to_shoot(window, player);
                break;
            case KEYBOARD:
                if (window->in->key[window->settings->controls[SHOOT]])
                    return try_to_shoot(window, player);
                break;
            case MOUSE:
                if (window->in->mouse_button[SDL_BUTTON_LEFT])
                    return try_to_shoot(window, player);
                break;

            default:
                break;
        }
    }

    return 0;
}


static int handle_bomb_event(struct window *window, struct player *player)
{
    if (player->health > 0 && window->num_bombs > 0)
    {
        switch (player->input_type)
        {
            case CONTROLLER:
                if (window->in->c.button[SDL_CONTROLLER_BUTTON_B])
                {
                    window->in->c.button[SDL_CONTROLLER_BUTTON_B] = 0;

                    // Bomb: erase all visible enemies
                    bomb(window, 1);
                    window->num_bombs--;
                    return 1;
                }
                break;

            case KEYBOARD:
                if (window->in->key[window->settings->controls[BOMB]])
                {
                    window->in->key[window->settings->controls[BOMB]] = 0;

                    // Bomb: erase all visible enemies
                    bomb(window, 1);
                    window->num_bombs--;
                    return 1;
                }
                break;

            case MOUSE:
                if (window->in->mouse_button[SDL_BUTTON_RIGHT])
                {
                    window->in->mouse_button[SDL_BUTTON_RIGHT] = 0;

                    // Bomb: erase all visible enemies
                    bomb(window, 1);
                    window->num_bombs--;
                    return 1;
                }
                break;

            default:
                break;
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
    window->mission_start_time = SDL_GetTicks();

    for (unsigned i = 0; i < window->num_players; i++)
    {
        window->player[i].respawn_frame = 0;

        if (all_reset)
            for (unsigned i = 0; i < window->num_players; i++)
                window->player[i].missile_around = 0;
    }

    switch (difficulty)
    {
        case EASY:
            if (all_reset)
                for (unsigned i = 0; i < window->num_players; i++)
                    window->player[i].health = MAX_HEALTH_EASY;

            for (unsigned i = 0; i < window->num_players; i++)
                window->player[i].ammo = -1;

            window->num_bombs = 2;
            window->bonus = 0;
            break;

        case HARD:
            if (all_reset)
                for (unsigned i = 0; i < window->num_players; i++)
                    window->player[i].health = MAX_HEALTH_HARD;

            for (unsigned i = 0; i < window->num_players; i++)
                window->player[i].ammo = -1;

            window->num_bombs = 1;
            window->bonus = 1500;
            break;

        case REALLY_HARD:
            if (all_reset)
                for (unsigned i = 0; i < window->num_players; i++)
                    window->player[i].health = MAX_HEALTH_REALLY_HARD;

            for (unsigned i = 0; i < window->num_players; i++)
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

        for (unsigned i = 0; i < window->num_players; i++)
        {
            window->player[i].animated_health_low = window->player[i].health;
            window->player[i].animated_health_high = window->player[i].health;
        }
    }

    for (unsigned i = 0; i < window->num_players; i++)
    {
        window->player[i].lives = 1;
        window->player[i].shield_time = -SHIELD_TIME;
    }
}


static void render_ship(struct window *window, SDL_Rect *temp_pos)
{
    SDL_Rect pos = *temp_pos;

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
    window->restart = 0;

    // Set mode to arcade if necessary
    if (mission_num == NUM_LEVELS + 1)
    {
        is_arcade = 1;
        mission_num = 1;
    }

    SDL_ShowCursor(SDL_DISABLE);

    // Load enemy paths and set enemy timer
    char s[50] = { 0 };
    sprintf(s, "data/level%d.txt", mission_num);
    window->paths = load_paths(window, s);

    int escape = 0;
    int retry = 1;

    while (!escape)
    {
        reset_game_attributes(window, difficulty, retry);

        if (retry)
        {
            load_correct_music(window, mission_num, is_arcade);
            init_background(window);

            for (unsigned i = 0; i < window->num_players; i++)
                init_position(120, POS_CENTERED, window->img->ship->texture, &window->player[i].pos);

            retry = 0;
        }

        window->paths->index = 0;

        int dead = 0;
        int won = 0;
        unsigned long framecount = 0;

        while (!escape && !dead && !won)
        {
            // Get and handle events
            update_events(window->in, window, 1);
            handle_quit_event(window, 1);

            if (!window->is_lan && window->in->focus_lost)
                pause(window);

            if (handle_escape_event(window))
            {
                if (!window->is_lan)
                    escape = pause(window);
                else
                {
                    // Send quit message
                    struct msg msg = { .type = QUIT_MSG };
                    msg.content.boolean = 1;
                    send_msg(window, &msg);

                    // Quit game
                    free_background(window->stars);
                    free_vector(window->paths);
                    window->paths = NULL; // important, see free_all in free.c
                    load_music(window, "data/endgame.ogg", 1);
                    return;
                }
            }


            for (unsigned i = 0; i < window->num_players; i++)
            {
                if (i == 0 || (i == 1 && !window->is_lan))
                {
                    handle_arrow_event(window, &window->player[i]);
                    handle_shot_event(window, &window->player[i]);
                    handle_bomb_event(window, &window->player[i]);
                }
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
                if (!handle_messages(window, "PSBQ"))
                    return;

            for (unsigned i = 0; i < window->num_players; i++)
            {
                // Check collisions
                check_collisions(window, &window->player[i]);

                // If dead, wait some frames and respawn
                dead = respawn(window, &window->player[i]);
                if (dead)
                    break;
            }


            // Display textures
            if (mission_num <= 3)
                SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
            else if (mission_num <= 6)
                SDL_SetRenderDrawColor(window->renderer, 32, 32, 128, 255);
            else
                SDL_SetRenderDrawColor(window->renderer, 192, 128, 0, 255);
            SDL_RenderClear(window->renderer);
            render_background(window);
            render_objects(window);
            for (unsigned i = 0; i < window->num_players; i++)
                render_trail(window, &window->player[i], &window->player[i].pos, 0, 0);
            render_enemies_health(window);
            render_shots(window);
            render_enemy_shots(window);
            render_enemies(window);
            for (unsigned i = 0; i < window->num_players; i++)
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
                window->paths = NULL; // important, see free_all in free.c

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

            escape = failure(window, is_arcade ? NUM_LEVELS + 1 : mission_num, difficulty);
            if (!escape)
            {
                retry = 1;
                if (is_arcade)
                {
                    free_vector(window->paths);
                    window->paths = NULL; // important, see free_all in free.c
                    window->paths = load_paths(window, "data/level1.txt");
                }
            }
        }
    }

    free_vector(window->paths);
    window->paths = NULL; // important, see free_all in free.c
    load_music(window, "data/endgame.ogg", 1);
}

