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
#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>


static void handle_arrow_event(struct window *window, SDL_Rect *pos)
{
    // Move ship
    if (window->in->key[SDL_SCANCODE_UP])
        pos->y -= SHIP_SPEED;

    if (window->in->key[SDL_SCANCODE_DOWN])
        pos->y += SHIP_SPEED;

    if (window->in->key[SDL_SCANCODE_LEFT])
        pos->x -= SHIP_SPEED;

    if (window->in->key[SDL_SCANCODE_RIGHT])
        pos->x += SHIP_SPEED;

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
    if (window->in->key[SDL_SCANCODE_SPACE] && window->health > 0)
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
    if (window->in->key[SDL_SCANCODE_C])
    {
        window->in->key[SDL_SCANCODE_C] = 0;

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


static void move_background(unsigned long framecount, struct window *window, SDL_Rect *pos_src_bg)
{
    if (framecount % 2 == 0)
    {
        pos_src_bg->x++;

        int w = 0;
        SDL_QueryTexture(window->img->bg, NULL, NULL, &w, NULL);

        if (pos_src_bg->x >= w)
            pos_src_bg->x = 0;

    }
}


static void render_background(struct window *window, SDL_Rect *pos_src_bg)
{
    int w = 0;
    SDL_QueryTexture(window->img->bg, NULL, NULL, &w, NULL);

    if (pos_src_bg->x + window->w <= w)
    {
        // pos_dst_bg is here to avoid a glitch when passing NULL
        // on fourth parameter of SDL_RenderCopy on some Linux congigurations
        SDL_Rect pos_dst_bg = { .x = 0, .y = 0, .w = window->w, .h = window->h };
        SDL_RenderCopy(window->renderer, window->img->bg, pos_src_bg, &pos_dst_bg);
    }
    else
    {
        SDL_Rect pos_dst_bg = { .x = 0, .y = 0, .w = w - pos_src_bg->x, .h = window->h };
        SDL_RenderCopy(window->renderer, window->img->bg, pos_src_bg, &pos_dst_bg);

        SDL_Rect pos_src_bg2 = { .x = 0, .y = 0, .w = window->w - pos_dst_bg.w, .h = window->h };
        SDL_Rect pos_dst_bg2 = { .x = pos_dst_bg.w , .y = 0, window->w - pos_dst_bg.w, .h = window->h };
        SDL_RenderCopy(window->renderer, window->img->bg, &pos_src_bg2, &pos_dst_bg2);
    }
}


static int respawn(struct window *window, SDL_Rect *pos)
{
    if (window->health <= 0)
    {
        window->respawn_frame++;

        if (window->respawn_frame >= 120)
        {
            if (window->lives > 1)
            {
                window->health = MAX_HEALTH;
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


void reset_game_attributes(struct window *window)
{
    for (enum list_type i = 0; i < NUM_LISTS; i++)
        clear_list(window->list[i]);

    window->last_enemy_time = 0;
    window->health = MAX_HEALTH;
    window->score = 0;
    window->respawn_frame = 0;
    window->is_wave_title = 0;
    window->wave_title_time = 0;
    window->num_bombs = 3;
    window->paths->index = 0;
    window->lives = 1;
}


void play_game(struct window *window)
{
    int escape = 0;

    while (!escape)
    {
        load_music(window, "data/spy.ogg", 1);

        SDL_Rect pos;
        init_position(120, POS_CENTERED, window, window->img->ship->texture, &pos);
        SDL_Rect pos_src_bg = { .x = 0, .y = 0, .w = window->w, .h = window->h };

        int dead = 0;
        int won = 0;
        unsigned long framecount = 0;

        while (!escape && !dead && !won)
        {
            // Get and handle events
            update_events(window->in);
            handle_quit_event(window);
            escape = handle_escape_event(window);
            handle_arrow_event(window, &pos);
            handle_shot_event(window, &pos);
            handle_bomb_event(window);

            // Move elements and background
            move_shots(window);
            move_enemies(window, &pos);
            move_explosions(window);
            move_enemy_shots(window);
            move_background(framecount, window, &pos_src_bg);

            // Check collisions
            check_collisions(window, &pos);

            // If dead, wait some frames and respawn
            dead = respawn(window, &pos);

            // Display textures
            SDL_RenderClear(window->renderer);
            render_background(window, &pos_src_bg);
            render_trail(window, &pos, 0);
            render_enemies_health(window);
            render_shots(window);
            render_enemy_shots(window);
            render_enemies(window);
            if (window->health > 0)
                SDL_RenderCopy(window->renderer, window->img->ship->texture, NULL, &pos);
            render_explosions(window);
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
            success(window);

        else if (dead)
            escape = failure(window);

        reset_game_attributes(window);
    }

    load_music(window, "data/hybris.ogg", 1);
}
