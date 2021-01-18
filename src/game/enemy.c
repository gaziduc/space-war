#include "init.h"
#include "utils.h"
#include "enemy.h"
#include "list.h"
#include "game.h"
#include "path.h"
#include "boss.h"
#include "collision.h"
#include "explosion.h"
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>


static int is_rotating(char enemy_type)
{
    if (enemy_type == 'C')
        return 1;

    return 0;
}

void set_enemy_attributes(struct list *new, SDL_FRect *pos,
                          struct window *window, char enemy_type)
{
    new->speed.x = window->paths->data[window->paths->index].line.enemy_path.speed_x;
    new->health = window->paths->data[window->paths->index].line.enemy_path.health;
    new->max_health = new->health;
    new->last_time_hurt = 0;
    new->first_time_hurt = 0;
    new->enemy_type = enemy_type;

    switch (enemy_type)
    {
        case 'A':
            new->texture.texture = window->img->enemy;
            break;

        case 'B':
            new->texture.texture = window->img->asteroid;
            break;

        case 'C':
            for (size_t i = 0; i < NUM_ROTATING_FRAMES; i++)
                new->texture.textures[i] = window->img->rotating_enemy[i];
            break;

        case 'D':
            new->texture.texture = window->img->drone;
            // Set vertical speed
            new->speed.y = new->speed.x;
            break;

        case 'E':
            new->texture.texture = window->img->wall;
            break;

        default:
            break;
    }

    new->rotating = is_rotating(enemy_type);
    new->curr_texture = 0;
    init_position_float(DEFAULT_W, pos->y,
                  new->rotating ? new->texture.textures[0]->texture
                                : new->texture.texture->texture,
                  &new->pos_dst);
    new->framecount = 0;
}


void create_enemies(struct window *window)
{
    Uint32 ticks = SDL_GetTicks();

    // If it is time to create enemy
    if (ticks - window->last_enemy_time
        >= window->paths->data[window->paths->index].line.enemy_path.time_to_wait)
    {
        int h = 0;

        switch (window->paths->data[window->paths->index].line.enemy_path.enemy_type)
        {
            case 'A':
                SDL_QueryTexture(window->img->enemy->texture, NULL, NULL, NULL, &h);
                break;
            case 'B':
                SDL_QueryTexture(window->img->asteroid->texture, NULL, NULL, NULL, &h);
                break;
            case 'C':
                SDL_QueryTexture(window->img->rotating_enemy[0]->texture, NULL, NULL, NULL, &h);
                break;
            case 'D':
                SDL_QueryTexture(window->img->drone->texture, NULL, NULL, NULL, &h);
                break;
            case 'E':
                SDL_QueryTexture(window->img->wall->texture, NULL, NULL, NULL, &h);
                break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                SDL_QueryTexture(window->img->bosses[window->paths->data[window->paths->index].line.enemy_path.enemy_type - '0']->texture, NULL, NULL, NULL, &h);
                break;

            default:
                error("Error in paths file", "Unknown enemy type", window->window, window->renderer);
                break;
        }

        SDL_FRect pos = { .x = 0,
                         .y = window->paths->data[window->paths->index].line.enemy_path.pos_y
                              - h / 2,
                         .w = 0,
                         .h = 0
                       };

        char type = window->paths->data[window->paths->index].line.enemy_path.enemy_type;

        if (type >= 'A' && type <= 'Z')
            list_push_front(&pos, window, ENEMY_LIST, NULL, NULL, 0, type, 0);
        else if (type >= '0' && type <= '9')
            list_push_front(&pos, window, BOSS_LIST, NULL, NULL, 0, type, 0);

        window->last_enemy_time = ticks;
        window->paths->index++;
    }
}


static int is_shooting(char enemy_type)
{
    if (enemy_type == 'A' || enemy_type == 'C' || enemy_type == 'D')
        return 1;

    return 0;
}


struct player* select_player(struct window *window, struct list *temp)
{
    if (window->num_players == 1)
        return &window->player[0];
    else // if (window->num_players == 2)
    {
        int dx1 = abs(temp->pos_dst.x - window->player[0].pos.x);
        int dy1 = abs(temp->pos_dst.y - window->player[0].pos.y);
        double d1 = sqrt(dx1 * dx1 + dy1 * dy1);

        int dx2 = abs(temp->pos_dst.x - window->player[1].pos.x);
        int dy2 = abs(temp->pos_dst.y - window->player[1].pos.y);
        double d2 = sqrt(dx2 * dx2 + dy2 * dy2);

        if (d1 < d2)
            return &window->player[0];

        return &window->player[1];
    }
}



void move_enemies(struct window *window)
{
    struct list *temp = window->list[ENEMY_LIST]->next;
    struct list *prev = window->list[ENEMY_LIST];

    while (temp)
    {
        // Move enemy
        temp->pos_dst.x -= temp->speed.x;

        // If drone
        if (temp->enemy_type == 'D')
        {
            temp->pos_dst.y += temp->speed.y;
            // If vertical out of bounds, change vertical speed
            if (temp->pos_dst.y < 20 || temp->pos_dst.y + temp->pos_dst.h > DEFAULT_H - 20)
                temp->speed.y = -temp->speed.y;
        }

        temp->framecount++;

        if (is_rotating(temp->enemy_type))
            temp->curr_texture = (temp->curr_texture + 1) % NUM_ROTATING_FRAMES;

        int shoot = is_shooting(temp->enemy_type);

        // Selecting on which player to shoot
        struct player *closest_player = select_player(window, temp);

        if (temp->enemy_type == 'A' && temp->framecount % FRAMES_BETWEEN_ENEMY_SHOTS == 0)
            list_push_front(&temp->pos_dst, window, ENEMY_SHOT_LIST, NULL,
                            &closest_player->pos, 0, 0, 0);

        if (temp->enemy_type == 'C' && temp->framecount % FRAMES_BETWEEN_ROTATING_ENEMY_SHOTS == 0)
            list_push_front(&temp->pos_dst, window, ENEMY_SHOT_LIST, NULL,
                            &closest_player->pos, 0, 0, 0);

        if (temp->enemy_type == 'D' && temp->framecount % FRAMES_BETWEEN_DRONE_SHOTS == 0)
            list_push_front(&temp->pos_dst, window, ENEMY_SHOT_LIST, NULL,
                            &closest_player->pos, 0, 0, 0);

        // Prevent out of bounds by deleting the enemy if not on screen
        if (temp->pos_dst.x + temp->pos_dst.w <= 0)
        {
            struct list *to_delete = temp;
            prev->next = temp->next;
            free(to_delete);

            // If enemy passed, mission failed
            if (shoot)
                for (unsigned i = 0; i < window->num_players; i++)
                    window->player[i].lives = 0;

            // Go to next shot
            temp = prev->next;
        }
        else
        {
            // Go to next shot
            prev = temp;
            temp = temp->next;
        }
    }

    move_boss(window);
}


void render_enemies(struct window *window)
{
    struct list *temp = window->list[ENEMY_LIST]->next;

    while (temp)
    {
        // Display enemy trail
        if (is_shooting(temp->enemy_type) && !temp->rotating)
            render_trail(window, NULL, &temp->pos_dst, 0, 1);

        // Display enemy
        if (temp->rotating)
        {
            int w = 0;
            int h = 0;

            SDL_QueryTexture(temp->texture.textures[temp->curr_texture]->texture, NULL, NULL, &w, &h);

            SDL_Rect pos = { .x = temp->pos_dst.x + temp->pos_dst.w / 2 - w / 2,
                             .y = temp->pos_dst.y + temp->pos_dst.h / 2 - h / 2,
                             .w = w,
                             .h = h
                           };

            resize_pos_for_resolution(window, &pos);

            SDL_RenderCopy(window->renderer,
                           temp->texture.textures[temp->curr_texture]->texture,
                           NULL, &pos);
        }
        else
        {
            SDL_Rect pos = { .x = temp->pos_dst.x,
                             .y = temp->pos_dst.y,
                             .w = temp->pos_dst.w,
                             .h = temp->pos_dst.h
                           };

            resize_pos_for_resolution(window, &pos);

            SDL_RenderCopy(window->renderer,
                           temp->texture.texture->texture,
                           NULL, &pos);
        }

        // Go to next enemy
        temp = temp->next;
    }

    // Display boss if any
    render_boss(window);
}


void render_enemy_health(struct window *window, struct list *enemy, Uint8 alpha)
{
    SDL_Rect pos = { .x = enemy->pos_dst.x + enemy->pos_dst.w / 2 - 50,
                     .y = enemy->pos_dst.y - 25,
                     .w = (100 * enemy->health) / enemy->max_health,
                     .h = 5
                   };

    int old_w = pos.w;

    resize_pos_for_resolution(window, &pos);

    SDL_SetRenderDrawColor(window->renderer, 0, 255, 0, alpha); // green
    SDL_RenderFillRect(window->renderer, &pos);


    pos.x = enemy->pos_dst.x + enemy->pos_dst.w / 2 - 50 + (100 * enemy->health) / enemy->max_health;
    pos.y = enemy->pos_dst.y - 25; // Mandatory because of the resize_pos_for_resolution
    pos.w = 100 - old_w;
    pos.h = 5; // Mandatory because of the resize_pos_for_resolution

    resize_pos_for_resolution(window, &pos);

    SDL_SetRenderDrawColor(window->renderer, 255, 0, 0, alpha); // red
    SDL_RenderFillRect(window->renderer, &pos);
}


void render_enemies_health(struct window *window)
{
    struct list *temp = window->list[ENEMY_LIST]->next;
    Uint32 ticks = SDL_GetTicks();

    while (temp)
    {
        Uint32 diff_ticks = ticks - temp->last_time_hurt;
        Uint32 first_diff_ticks = ticks - temp->first_time_hurt;

        if (first_diff_ticks < 192)
            render_enemy_health(window, temp, first_diff_ticks);
        else if (diff_ticks < 1500)
            render_enemy_health(window, temp, 192);
        else if (diff_ticks < 1692)
            render_enemy_health(window, temp, 1692 - diff_ticks);


        // Go to next enemy
        temp = temp->next;
    }

    render_boss_health(window);
}

static int is_teleguided(char enemy_type)
{
    return enemy_type == '1';
}

static int is_explose(char enemy_type)
{
    return enemy_type == '2';
}

void set_enemy_shot_attributes(struct list *new, SDL_FRect *pos_dst,
                               SDL_FRect *ship_pos, char enemy_type, struct window *window)
{
    if (is_teleguided(enemy_type))
        new->texture.texture = window->img->enemy_shot_teleguided;
    else if (is_explose(enemy_type))
        new->texture.texture = window->img->enemy_shot_explode;
    else
        new->texture.texture = window->img->enemy_shot;

    // Setting shot initial position
    new->pos_dst.x = pos_dst->x + pos_dst->w / 2;
    new->pos_dst.y = pos_dst->y + pos_dst->h / 2 - 16;
    new->pos_dst.w = 32;
    new->pos_dst.h = 32;

    new->pos_src.x = 0;
    new->pos_src.y = 0;
    new->pos_src.w = 32;
    new->pos_src.h = 32;

    // Setting shot speed (horizontal and vertical)
    int gap_x = new->pos_dst.x + new->pos_dst.w / 2 - (ship_pos->x + ship_pos->w / 2);
    int gap_y = ship_pos->y + ship_pos->h / 2 - (new->pos_dst.y + new->pos_dst.h / 2);
    float gap = sqrt(gap_x * gap_x + gap_y * gap_y);

    new->speed.x = enemy_type == '9' ? (gap_x * FINAL_BOSS_SHOT_SPEED) / gap : (gap_x * ENEMY_SHOT_SPEED) / gap;
    new->speed.y = enemy_type == '9' ? (gap_y * FINAL_BOSS_SHOT_SPEED) / gap : (gap_y * ENEMY_SHOT_SPEED) / gap;

    new->enemy_type = enemy_type;
}


void move_enemy_shots(struct window *window)
{
    struct list *temp = window->list[ENEMY_SHOT_LIST]->next;
    struct list *prev = window->list[ENEMY_SHOT_LIST];

    while (temp)
    {
        if (is_teleguided(temp->enemy_type))
        {
            struct player *closest_player = select_player(window, temp);

            int gap_x = temp->pos_dst.x + temp->pos_dst.w / 2 - (closest_player->pos.x + closest_player->pos.w / 2);
            int gap_y = closest_player->pos.y + closest_player->pos.h / 2 - (temp->pos_dst.y + temp->pos_dst.h / 2);
            float gap = sqrt(gap_x * gap_x + gap_y * gap_y);

            float speed_x = temp->enemy_type == '9' ? (gap_x * FINAL_BOSS_SHOT_SPEED) / gap : (gap_x * ENEMY_SHOT_SPEED) / gap;
            float speed_y = temp->enemy_type == '9' ? (gap_y * FINAL_BOSS_SHOT_SPEED) / gap : (gap_y * ENEMY_SHOT_SPEED) / gap;
            float diff_speed_y = (speed_y - temp->speed.y) / 40;
            float diff_speed_x = (speed_x - temp->speed.x) / 40;

            temp->speed.x += diff_speed_x;
            if (temp->speed.x < 3)
                temp->speed.x = 3;
            else
                temp->speed.y += diff_speed_y;
        }
        else if (is_explose(temp->enemy_type))
        {
            struct player *closest_player = select_player(window, temp);

            int gap_x = temp->pos_dst.x + temp->pos_dst.w / 2 - (closest_player->pos.x + closest_player->pos.w / 2);
            int gap_y = closest_player->pos.y + closest_player->pos.h / 2 - (temp->pos_dst.y + temp->pos_dst.h / 2);
            float gap = sqrt(gap_x * gap_x + gap_y * gap_y);

            if (gap < 145)
            {
                list_push_front(&temp->pos_dst, window, EXPLOSION_LIST,
                                NULL, NULL, 0, 0, 1);

                hurt(window, closest_player);

                struct list *to_delete = temp;
                prev->next = temp->next;
                free(to_delete);

                // Go to next shot
                temp = prev->next;
                continue;
            }
        }

        // Move shot
        temp->pos_dst.x -= temp->speed.x;
        temp->pos_dst.y += temp->speed.y;

        // Go to next frame
        temp->pos_src.y += 32;
        if (temp->pos_src.y >= 28 * 32)
            temp->pos_src.y = 0;

        // Prevent out of bounds by deleting the shot if not on screen
        if (temp->pos_dst.x + temp->pos_dst.w <= 0
            || temp->pos_dst.y + temp->pos_dst.h <= 0
            || temp->pos_dst.y >= DEFAULT_H)
        {
            struct list *to_delete = temp;
            prev->next = temp->next;
            free(to_delete);

            // Go to next shot
            temp = prev->next;
        }
        else
        {
            // Go to next shot
            prev = temp;
            temp = temp->next;
        }
    }
}

void render_enemy_shots(struct window *window)
{
    struct list *temp = window->list[ENEMY_SHOT_LIST]->next;

    while (temp)
    {
        SDL_Rect pos_dst = { .x = (int) temp->pos_dst.x, .y = (int) temp->pos_dst.y,
                             .w = (int) temp->pos_dst.w, .h = (int) temp->pos_dst.h
                            };

        resize_pos_for_resolution(window, &pos_dst);

        // Display shot
        SDL_RenderCopy(window->renderer, temp->texture.texture->texture, &temp->pos_src, &pos_dst);

        // Go to next shot
        temp = temp->next;
    }
}
