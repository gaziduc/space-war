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
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>


static void handle_arrow_event(struct window *window, SDL_Rect *pos)
{
    /* Move ship */
    // Up
    if (window->in->key[SDL_SCANCODE_UP]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_UP])
        pos->y -= SHIP_SPEED;
    else if (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value <= -DEAD_ZONE)
        pos->y += window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value * SHIP_SPEED / 32768;

    // Down
    if (window->in->key[SDL_SCANCODE_DOWN]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_DOWN])
        pos->y += SHIP_SPEED;
    else if (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY].value >= DEAD_ZONE)
        pos->y += window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTY] .value* SHIP_SPEED / 32767;


    // Left
    if (window->in->key[SDL_SCANCODE_LEFT]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_UP])
        pos->x -= SHIP_SPEED;
    else if (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value<= -DEAD_ZONE)
        pos->x += window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value * SHIP_SPEED / 32768;


    // Right
    if (window->in->key[SDL_SCANCODE_RIGHT]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_DPAD_RIGHT])
        pos->x += SHIP_SPEED;
    else if (window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value >= DEAD_ZONE)
        pos->x += window->in->c.axis[SDL_CONTROLLER_AXIS_LEFTX].value * SHIP_SPEED / 32767;


    // Prevent out of bounds
    if (pos->y < 0)
        pos->y = 0;
    else if (pos->y > window->h - pos->h)
        pos->y = window->h - pos->h;

    if (pos->x < 0)
        pos->x = 0;
    else if (pos->x > window->w - pos->w)
        pos->x = window->w - pos->w;
}


static void handle_shot_event(struct window *window, SDL_Rect *pos)
{
    if ((window->in->key[SDL_SCANCODE_SPACE]
         || window->in->c.axis[SDL_CONTROLLER_AXIS_TRIGGERRIGHT].value >= DEAD_ZONE) && window->health > 0)
    {
        Uint32 current_time = SDL_GetTicks();

        // If enough time between now and the last shot
        if (current_time - window->last_shot_time >= DELAY_BETWEEN_SHOTS)
        {
            // Shot
            shoot(window, pos);
            window->last_shot_time = current_time;
        }
    }
}


static void handle_bomb_event(struct window *window)
{
    if (window->in->key[SDL_SCANCODE_C]
        || window->in->c.button[SDL_CONTROLLER_BUTTON_B])
    {
        window->in->key[SDL_SCANCODE_C] = 0;
        window->in->c.button[SDL_CONTROLLER_BUTTON_B] = 0;

        if (window->health > 0 && window->num_bombs > 0)
        {
            // Bomb: erase all visible enemies
            bomb(window);
            window->num_bombs--;
        }
    }
}


void render_trail(struct window *window, SDL_Rect *pos, int is_enemy)
{
    // If ship is dead, don't display trail
    if (!is_enemy && window->health <= 0)
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

    SDL_RenderCopyEx(window->renderer, window->img->trail, NULL, &pos_dst_trail,
                     0, NULL, flip);
}


static int respawn(struct window *window, SDL_Rect *pos)
{
    // If already dead (e.g. a enemy managed to pass to the left of the screen)
    if (window->lives <= 0)
        return 1;

    if (window->health <= 0)
    {
        window->respawn_frame++;

        if (window->respawn_frame >= 120)
        {
            if (window->lives > 1)
            {
                window->health = window->max_health;

                window->respawn_frame = 0;
                init_position(120, POS_CENTERED, window, window->img->ship->texture, pos);

                window->lives--;
            }
            else
                return 1;
        }
    }

    return 0;
}


void reset_game_attributes(struct window *window, int difficulty)
{
    for (enum list_type i = 0; i < NUM_LISTS; i++)
        clear_list(window->list[i]);

    window->last_enemy_time = 0;
    window->score = 0;
    window->respawn_frame = 0;
    window->is_wave_title = 0;
    window->wave_title_time = 0;

    switch (difficulty)
    {
        case EASY:
            window->health = MAX_HEALTH_EASY;
            window->num_bombs = 3;
            window->bonus = 0;
            break;
        case HARD:
            window->health = MAX_HEALTH_HARD;
            window->num_bombs = 1;
            window->bonus = 1000;
            break;

        default:
            error("Unknown difficulty level", "Unknown difficulty level", window->window);
            break;
    }

    window->max_health = window->health;
    window->animated_health_low = window->health;
    window->animated_health_high = window->health;
    window->lives = 1;
}


void play_game(struct window *window, int mission_num, int difficulty)
{
    // Load enemy paths and set enemy timer
    char s[50] = { 0 };
    sprintf(s, "data/level%d.txt", mission_num);
    window->paths = load_paths(window, s);

    int escape = 0;

    while (!escape)
    {
        load_music(window, "data/madness.ogg", 1);

        init_background(window);

        reset_game_attributes(window, difficulty);
        window->paths->index = 0;

        SDL_Rect pos;
        init_position(120, POS_CENTERED, window, window->img->ship->texture, &pos);

        int dead = 0;
        int won = 0;
        unsigned long framecount = 0;

        while (!escape && !dead && !won)
        {
            // Get and handle events
            update_events(window->in, window);
            handle_quit_event(window, 1);
            escape = handle_escape_event(window);
            handle_arrow_event(window, &pos);
            handle_shot_event(window, &pos);
            handle_bomb_event(window);

            // Move elements and background
            move_shots(window);
            move_enemies(window, &pos);
            move_explosions(window);
            move_enemy_shots(window);
            move_objects(window);
            move_hud_texts(window);
            move_background(window, framecount);

            // Check collisions
            check_collisions(window, &pos);

            // If dead, wait some frames and respawn
            dead = respawn(window, &pos);

            // Display textures
            SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
            SDL_RenderClear(window->renderer);
            render_background(window);
            render_objects(window);
            render_trail(window, &pos, 0);
            render_enemies_health(window);
            render_shots(window);
            render_enemy_shots(window);
            render_enemies(window);
            if (window->health > 0)
                SDL_RenderCopy(window->renderer, window->img->ship->texture, NULL, &pos);
            render_explosions(window);
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

        free_background(window->stars);

        if (won)
        {
            success(window);
            escape = 1;
        }

        else if (dead)
            escape = failure(window);
    }

    free_vector(window->paths);
    load_music(window, "data/hybris.ogg", 1);
}
