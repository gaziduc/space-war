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
#include "string_vec.h"
#include <stdio.h>
#include <SDL2/SDL.h>



static void handle_arrow_event(struct window *window, struct player *player)
{
    /* Move ship */
    SDL_Point last_player_pos = { .x = player->pos.x, .y = player->pos.y };

    switch (player->input_type)
    {
        case CONTROLLER:
            // Up
            if (window->in->c[player->controller_num].button[SDL_CONTROLLER_BUTTON_DPAD_UP])
                player->pos.y -= SHIP_SPEED;
            else if (window->in->c[player->controller_num].axis[SDL_CONTROLLER_AXIS_LEFTY].value <= -DEAD_ZONE)
                player->pos.y += window->in->c[player->controller_num].axis[SDL_CONTROLLER_AXIS_LEFTY].value * SHIP_SPEED / 32768;


            // Down
            if (window->in->c[player->controller_num].button[SDL_CONTROLLER_BUTTON_DPAD_DOWN])
                player->pos.y += SHIP_SPEED;
            else if (window->in->c[player->controller_num].axis[SDL_CONTROLLER_AXIS_LEFTY].value >= DEAD_ZONE)
                player->pos.y += window->in->c[player->controller_num].axis[SDL_CONTROLLER_AXIS_LEFTY].value * SHIP_SPEED / 32767;


            // Left
            if (window->in->c[player->controller_num].button[SDL_CONTROLLER_BUTTON_DPAD_LEFT])
                player->pos.x -= SHIP_SPEED;
            else if (window->in->c[player->controller_num].axis[SDL_CONTROLLER_AXIS_LEFTX].value <= -DEAD_ZONE)
                player->pos.x += window->in->c[player->controller_num].axis[SDL_CONTROLLER_AXIS_LEFTX].value * SHIP_SPEED / 32768;


            // Right
            if (window->in->c[player->controller_num].button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT])
                player->pos.x += SHIP_SPEED;
            else if (window->in->c[player->controller_num].axis[SDL_CONTROLLER_AXIS_LEFTX].value >= DEAD_ZONE)
                player->pos.x += window->in->c[player->controller_num].axis[SDL_CONTROLLER_AXIS_LEFTX].value * SHIP_SPEED / 32767;

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
            SDL_FRect last_pos = player->pos;

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

            SDL_FRect cursor_pos = player->pos;
            resize_pos_for_resolution_float(window, &cursor_pos);
            SDL_WarpMouseInWindow(window->window, cursor_pos.x + 0.5, cursor_pos.y + 0.5); // + 0.5 for fixing a bug, do not remove
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
                if (window->in->c[player->controller_num].axis[SDL_CONTROLLER_AXIS_TRIGGERRIGHT].value >= DEAD_ZONE)
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
                if (window->in->c[player->controller_num].button[SDL_CONTROLLER_BUTTON_X])
                {
                    window->in->c[player->controller_num].button[SDL_CONTROLLER_BUTTON_X] = 0;

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


void render_trail(struct window *window, struct player *player, SDL_FRect *pos,
                  int is_lan_player, int is_enemy)
{
    // If ship is dead, don't display trail
    if (!is_enemy && !is_lan_player && player->health <= 0)
        return;

    SDL_Rect pos_dst_trail;
    SDL_QueryTexture(window->img->trail, NULL, NULL, &pos_dst_trail.w, &pos_dst_trail.h);

    int count = window->fps->framecount % 10;

    if (count < 5)
        pos_dst_trail.x = pos->x - pos_dst_trail.w + 15 + count / 2;
    else
    {
        count -= 5;
        pos_dst_trail.x = pos->x - pos_dst_trail.w + 15 + 3 - count / 2;
    }

    pos_dst_trail.y = pos->y + pos->h / 2 - pos_dst_trail.h / 2;

    SDL_RendererFlip flip = 0;

    if (is_enemy)
    {
        pos_dst_trail.x = pos_dst_trail.x + pos->w + 50;
        flip |= SDL_FLIP_HORIZONTAL;
    }

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
                init_position_float(120, POS_CENTERED, window->img->ship[0]->texture, &player->pos);

                player->lives--;
            }
            else
                return 1;
        }
    }

    return 0;
}


void reset_game_attributes(struct window *window, int all_reset)
{
    if (all_reset)
    {
        for (enum list_type i = 0; i < NUM_LISTS; i++)
            clear_list(window->list[i]);

        window->score = 0;
        window->touched_anim = 0;
        window->combo = 0;
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

    switch (window->level_difficulty)
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
            window->bonus = 5000;
            break;

        case REALLY_HARD:
            if (all_reset)
                for (unsigned i = 0; i < window->num_players; i++)
                    window->player[i].health = MAX_HEALTH_REALLY_HARD;

            for (unsigned i = 0; i < window->num_players; i++)
                window->player[i].ammo = 200;

            window->num_bombs = 1;
            window->bonus = 20000;
            break;

        default:
            error("Unknown difficulty level", "Unknown difficulty level", window->window, window->renderer);
            break;
    }

    window->initial_bombs = window->num_bombs;

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

    // Trophies
    window->combo_lover = 1;
    window->num_enemies_collided = 0;
}


static unsigned get_ship_frame_num(Uint32 framecount)
{
    framecount /= 5;
    framecount = framecount % ((NUM_ANIM_SHIP - 1) * 2);

    return framecount < NUM_ANIM_SHIP ? framecount : -framecount + 2 * (NUM_ANIM_SHIP - 1);
}


static void render_ship(struct window *window, SDL_FRect *temp_pos, unsigned frame_num)
{
    SDL_Rect pos = { temp_pos->x, temp_pos->y, temp_pos->w, temp_pos->h };
    resize_pos_for_resolution(window, &pos);

    SDL_RenderCopy(window->renderer, window->img->ship[frame_num]->texture, NULL, &pos);
}


static void load_correct_music(struct window *window, int mission_num, int is_arcade)
{
    if (is_arcade)
    {
        if (mission_num == 1)
            load_music_and_play(window, "data/pirates.ogg", 1);
        else if (mission_num == 4)
            load_music_and_play(window, "data/battle.ogg", 1);
        else if (mission_num == 7)
            load_music_and_play(window, "data/planet.ogg", 1);
    }
    else
    {
        if (mission_num < 4)
            load_music_and_play(window, "data/pirates.ogg", 1);
        else if (mission_num < 7)
            load_music_and_play(window, "data/battle.ogg", 1);
        else
            load_music_and_play(window, "data/planet.ogg", 1);
    }
}


void play_game(struct window *window)
{
    int is_arcade = 0;
    window->restart = 0;

    int level_num = window->level_num;

    // Set mode to arcade if necessary
    if (level_num == NUM_LEVELS + 1)
    {
        is_arcade = 1;
        level_num = 1;
    }

    // Load enemy paths and set enemy timer
    char s[50] = { 0 };
    if (!window->is_lan || window->server)
    {
        sprintf(s, "data/level%d.txt", level_num);
        window->paths = load_paths(window, s);
    }
    
    int escape = 0;
    int retry = 1;

    while (!escape)
    {
        reset_game_attributes(window, retry);

        if (retry)
        {
            load_correct_music(window, level_num, is_arcade);
            init_background(window);

            for (unsigned i = 0; i < window->num_players; i++)
                init_position_float(120, POS_CENTERED, window->img->ship[0]->texture, &window->player[i].pos);

            retry = 0;
        }

        if (!window->is_lan || window->server)
            window->paths->index = 0;

        int dead = 0;
        int won = 0;
        unsigned long framecount = 0;

        if (window->is_lan)
        {
            struct msg msg = { .type = POSITION_MSG };
            msg.content.point.x = window->player[0].pos.x;
            msg.content.point.y = window->player[0].pos.y;
            send_msg(window, &msg);
        }

        SDL_ShowCursor(SDL_DISABLE);

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
                    quit_game(window);
                    return;
                }
            }

            for (unsigned i = 0; i < window->num_players; i++)
            {
                window->player[i].frame_num = get_ship_frame_num(framecount);

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
            if (level_num <= 3)
                SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
            else if (level_num <= 6)
                SDL_SetRenderDrawColor(window->renderer, 32, 32, 128, 255);
            else
                SDL_SetRenderDrawColor(window->renderer, 192, 128, 0, 255);
            SDL_RenderClear(window->renderer);
            render_pre_bg_objects(window);
            render_background(window);
            render_post_bg_objects(window);
            for (unsigned i = 0; i < window->num_players; i++)
                render_trail(window, &window->player[i], &window->player[i].pos, 0, 0);
            render_shots(window);
            render_enemy_shots(window);
            render_enemies(window);
            for (unsigned i = 0; i < window->num_players; i++)
            {
                if (window->player[i].health > 0)
                {
                    render_shield_aura(window, &window->player[i]);
                    render_ship(window, &window->player[i].pos, window->player[i].frame_num);
                }
            }

            render_explosions(window);
            render_enemies_health(window);
            render_touched_effect(window);
            render_hud_texts(window);
            render_hud(window);


            // Create enemies, display wave titles...
            if (!window->is_lan || window->server)
            {
                won = execute_path_action(window) && !window->list[BOSS_LIST]->next
                                                  && !window->list[EXPLOSION_LIST]->next;

                if (won && window->server)
                {
                    struct msg msg = { .type = WON_MSG };
                    send_msg(window, &msg);
                }
            }
            
            if (window->is_lan)
            {
                if (window->server && framecount % 15 == 0)
                {
                    struct msg msg = { .type = SERVER_ALL_MSG };
                    msg.content.string_vec = create_string(window);

                    char buf[8] = { 0 };
                    SDLNet_Write16(window->player[0].health, buf);
                    SDLNet_Write16(window->player[1].health, buf + 2);
                    SDLNet_Write32(window->score, buf + 4);
                    add_bytes(window, msg.content.string_vec, buf, 8);

                    struct list* temp = window->list[ENEMY_LIST]->next;
                    while (temp)
                    {
                        char buffer[24] = { 0 };
                        buffer[0] = 'e'; // Enemy
                        write_float(temp->pos_dst.x, buffer + 1);
                        write_float(temp->pos_dst.y, buffer + 5);
                        SDLNet_Write16(temp->max_health, buffer + 9);
                        SDLNet_Write16(temp->health, buffer + 11);
                        write_float(temp->speed.x, buffer + 13);
                        write_float(temp->speed.y, buffer + 17);
                        buffer[21] = temp->enemy_type;
                        SDLNet_Write16(temp->curr_texture, buffer + 22);

                        add_bytes(window, msg.content.string_vec, buffer, 24);

                        temp = temp->next;
                    }

                    temp = window->list[MY_SHOTS_LIST]->next;
                    while (temp)
                    {
                        char buffer[9] = { 0 };
                        buffer[0] = 's'; // My shots
                        write_float(temp->pos_dst.x, buffer + 1);
                        write_float(temp->pos_dst.y, buffer + 5);

                        add_bytes(window, msg.content.string_vec, buffer, 9);

                        temp = temp->next;
                    }


                    temp = window->list[ENEMY_SHOT_LIST]->next;
                    while (temp)
                    {
                        char buffer[18] = { 0 };
                        buffer[0] = 't'; // Enemy shots
                        write_float(temp->pos_dst.x, buffer + 1);
                        write_float(temp->pos_dst.y, buffer + 5);
                        write_float(temp->speed.x, buffer + 9);
                        write_float(temp->speed.y, buffer + 13);
                        buffer[17] = temp->enemy_type;

                        add_bytes(window, msg.content.string_vec, buffer, 18);

                        temp = temp->next;
                    }

                    temp = window->list[OBJECT_LIST]->next;
                    while (temp)
                    {
                        char buffer[10] = { 0 };
                        buffer[0] = 'o'; // Objects
                        write_float(temp->pos_dst.x, buffer + 1);
                        write_float(temp->pos_dst.y, buffer + 5);
                        buffer[9] = temp->type;

                        add_bytes(window, msg.content.string_vec, buffer, 10);

                        temp = temp->next;
                    }

                    temp = window->list[EXPLOSION_LIST]->next;
                    while (temp)
                    {
                        char buffer[26] = { 0 };
                        buffer[0] = 'x'; // Explosion
                        write_float(temp->pos_dst.x, buffer + 1);
                        write_float(temp->pos_dst.y, buffer + 5);
                        write_float(temp->pos_src.x, buffer + 9);
                        write_float(temp->pos_src.y, buffer + 13);
                        write_float(temp->pos_src.w, buffer + 17);
                        write_float(temp->pos_src.h, buffer + 21);
                        buffer[25] = temp->num_explosion;

                        add_bytes(window, msg.content.string_vec, buffer, 26);

                        temp = temp->next;
                    }

                    temp = window->list[HUD_LIST]->next;
                    while (temp)
                    {
                        char buffer[21] = { 0 };
                        buffer[0] = 'h'; // HUD
                        write_float(temp->pos_dst.x, buffer + 1);
                        write_float(temp->pos_dst.y, buffer + 5);
                        write_float(temp->pos_dst.w, buffer + 9);
                        write_float(temp->pos_dst.h, buffer + 13);
                        SDLNet_Write32(SDL_GetTicks() - temp->last_time_hurt, buffer + 17);

                        add_bytes(window, msg.content.string_vec, buffer, 21);

                        temp = temp->next;
                    }

                    temp = window->list[BOSS_LIST]->next;
                    while (temp)
                    {
                        char buffer[30] = { 0 };
                        buffer[0] = 'b'; // Boss
                        write_float(temp->pos_dst.x, buffer + 1);
                        write_float(temp->pos_dst.y, buffer + 5);
                        write_float(temp->pos_dst.w, buffer + 9);
                        write_float(temp->pos_dst.h, buffer + 13);
                        SDLNet_Write16(temp->max_health, buffer + 17);
                        SDLNet_Write16(temp->health, buffer + 19);
                        write_float(temp->speed.x, buffer + 21);
                        write_float(temp->speed.y, buffer + 25);
                        buffer[29] = temp->enemy_type;

                        add_bytes(window, msg.content.string_vec, buffer, 30);

                        temp = temp->next;
                    }

                    send_msg(window, &msg);

                    free_string(msg.content.string_vec);
                }

                // LAN only
                int res = handle_messages(window, "PSBQ:W");
                if (!res)
                    return;
                if (res == 3)
                    won = 1;
            }


            SDL_RenderPresent(window->renderer);

            // Wait a frame
            frame_delay(window->fps);
            framecount++;
        }


        if (won)
        {
            window->score += compute_combo_score(window->combo);

            if (is_arcade && level_num < NUM_LEVELS)
            {
                if (!window->is_lan || window->server)
                    free_vector(window->paths);
                window->paths = NULL; // important, see free_all in free.c

                level_num++;
                if (!window->is_lan || window->server)
                {
                    sprintf(s, "data/level%d.txt", level_num);
                    window->paths = load_paths(window, s);
                }
                

                load_correct_music(window, level_num, is_arcade);
            }
            else
            {
                free_background(window->stars);

                success(window, is_arcade ? NUM_LEVELS + 1 : level_num);
                escape = 1;
            }
        }

        else if (dead)
        {
            free_background(window->stars);
            if (is_arcade)
                level_num = 1;

            escape = failure(window, is_arcade ? NUM_LEVELS + 1 : level_num);
            if (!escape)
            {
                retry = 1;
                if (is_arcade)
                {
                    if (!window->is_lan || window->server)
                        free_vector(window->paths);
                    window->paths = NULL; // important, see free_all in free.c
                    
                    if (!window->is_lan || window->server)
                        window->paths = load_paths(window, "data/level1.txt");
                }
            }
        }
    }

    if (window->server)
        free_vector(window->paths);
    window->paths = NULL; // important, see free_all in free.c
    load_music_and_play(window, "data/endgame.ogg", 1);
}

void quit_game(struct window* window)
{
    free_background(window->stars);
    if (!window->is_lan || window->server)
        free_vector(window->paths);
    window->paths = NULL; // important, see free_all in free.c
    load_music_and_play(window, "data/endgame.ogg", 1);
}

