#include "effect.h"
#include "init.h"
#include "utils.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

void render_touched_effect(struct window *window)
{
    if (window->touched_anim > 0)
    {
        SDL_SetRenderDrawColor(window->renderer, 255, 0, 0, window->touched_anim);
        SDL_RenderFillRect(window->renderer, NULL);
        window->touched_anim -= 2;
    }
}


void set_shake_effect(struct window *window)
{
    window->shake.x = (rand() % 3) + 3;
    window->shake.y = (rand() % 3) + 3;
}


static void force_feedback_on_controller(struct window* window, int controller_num, float strength, Uint32 length)
{
    if (window->in->c[controller_num].has_rumble)
    {
        Uint16 rumble_strength = strength * 0xFFFF;
        SDL_GameControllerRumble(window->in->c[controller_num].controller, rumble_strength, rumble_strength, length);
    }
    else if (window->in->c[controller_num].haptic)
    {
        SDL_HapticRumblePlay(window->in->c[controller_num].haptic, strength, length);
    }
}


void force_feedback(struct window *window, struct player *player, float strength, Uint32 length)
{
    if (player->input_type == CONTROLLER && window->settings->is_force_feedback)
        force_feedback_on_controller(window, player->controller_num, strength, length);
}

void force_feedback_on_all_controllers(struct window* window, float strength, Uint32 length)
{
    for (int i = 0; i < window->in->num_controllers; i++)
        force_feedback_on_controller(window, i, strength, length);
}




/* ------------------------------------------------------------------------- */
/* Particles                                                                  */
/* ------------------------------------------------------------------------- */

#define MAX_PARTICLES 768
#define MAX_SHOCKWAVES 6
#define SHOCKWAVE_FRAMES 40
#define SHOCKWAVE_POINTS 72

struct particle
{
    float x;
    float y;
    float vx;
    float vy;
    float size;
    float shrink;   // size lost per frame
    float drag;     // velocity multiplier per frame
    int life;       // remaining frames, <= 0 = free slot
    int max_life;
    Uint8 r;
    Uint8 g;
    Uint8 b;
};

struct shockwave
{
    float x;
    float y;
    int frame; // < 0 = free slot
};

static struct particle particles[MAX_PARTICLES];
static struct shockwave shockwaves[MAX_SHOCKWAVES] = { {0, 0, -1}, {0, 0, -1}, {0, 0, -1},
                                                       {0, 0, -1}, {0, 0, -1}, {0, 0, -1} };
static int flash_alpha = 0;
static unsigned next_slot = 0;


static float random_float(float min, float max)
{
    return min + (rand() / (float) RAND_MAX) * (max - min);
}


static struct particle *new_particle(void)
{
    // Reuse free slots, otherwise overwrite the oldest ones (ring buffer)
    for (unsigned i = 0; i < MAX_PARTICLES; i++)
    {
        struct particle *p = &particles[(next_slot + i) % MAX_PARTICLES];

        if (p->life <= 0)
        {
            next_slot = (next_slot + i + 1) % MAX_PARTICLES;
            return p;
        }
    }

    struct particle *p = &particles[next_slot];
    next_slot = (next_slot + 1) % MAX_PARTICLES;
    return p;
}


void spawn_explosion_particles(float x, float y, float scale)
{
    int num_sparks = 18 * scale + 6;

    // Hot sparks
    for (int i = 0; i < num_sparks; i++)
    {
        struct particle *p = new_particle();
        float angle = random_float(0, 2 * 3.14159265f);
        float speed = random_float(1.5f, 9.0f) * (0.5f + scale);

        p->x = x;
        p->y = y;
        p->vx = cosf(angle) * speed;
        p->vy = sinf(angle) * speed;
        p->max_life = p->life = 18 + rand() % 22;
        p->size = random_float(4, 9) * (0.6f + scale * 0.4f);
        p->shrink = p->size / p->max_life;
        p->drag = 0.93f;

        // From white-yellow to orange
        p->r = 255;
        p->g = 140 + rand() % 116;
        p->b = rand() % 90;
    }

    // Slow dark red embers
    for (int i = 0; i < 6 * scale + 2; i++)
    {
        struct particle *p = new_particle();
        float angle = random_float(0, 2 * 3.14159265f);
        float speed = random_float(0.3f, 2.5f);

        p->x = x + random_float(-15, 15) * scale;
        p->y = y + random_float(-15, 15) * scale;
        p->vx = cosf(angle) * speed;
        p->vy = sinf(angle) * speed;
        p->max_life = p->life = 35 + rand() % 25;
        p->size = random_float(14, 26) * (0.5f + scale * 0.5f);
        p->shrink = p->size / p->max_life;
        p->drag = 0.97f;
        p->r = 150;
        p->g = 40;
        p->b = 10;
    }
}


void spawn_engine_particles(const SDL_FRect *ship_pos)
{
    struct particle *p = new_particle();

    p->x = ship_pos->x + 8;
    p->y = ship_pos->y + ship_pos->h / 2 + random_float(-6, 6);
    p->vx = random_float(-7, -3);
    p->vy = random_float(-0.6f, 0.6f);
    p->max_life = p->life = 10 + rand() % 8;
    p->size = random_float(5, 9);
    p->shrink = p->size / p->max_life;
    p->drag = 0.95f;
    p->r = 60;
    p->g = 150 + rand() % 60;
    p->b = 255;
}


void spawn_shockwave(float x, float y)
{
    for (unsigned i = 0; i < MAX_SHOCKWAVES; i++)
    {
        if (shockwaves[i].frame < 0)
        {
            shockwaves[i].x = x;
            shockwaves[i].y = y;
            shockwaves[i].frame = 0;
            break;
        }
    }

    flash_alpha = 160;

    // Debris all over the screen
    for (int i = 0; i < 60; i++)
    {
        struct particle *p = new_particle();
        float angle = random_float(0, 2 * 3.14159265f);
        float speed = random_float(8, 30);

        p->x = x;
        p->y = y;
        p->vx = cosf(angle) * speed;
        p->vy = sinf(angle) * speed;
        p->max_life = p->life = 25 + rand() % 20;
        p->size = random_float(4, 10);
        p->shrink = p->size / p->max_life;
        p->drag = 0.94f;
        p->r = 120 + rand() % 100;
        p->g = 200;
        p->b = 255;
    }
}


void clear_particles(void)
{
    for (unsigned i = 0; i < MAX_PARTICLES; i++)
        particles[i].life = 0;

    for (unsigned i = 0; i < MAX_SHOCKWAVES; i++)
        shockwaves[i].frame = -1;

    flash_alpha = 0;
}


static void fill_square(struct window *window, float x, float y, float size)
{
    SDL_FRect rect = { .x = x - size / 2, .y = y - size / 2, .w = size, .h = size };

    resize_pos_for_resolution_float(window, &rect);
    SDL_RenderFillRectF(window->renderer, &rect);
}


void update_and_render_particles(struct window *window)
{
    SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_ADD);

    // Particles
    for (unsigned i = 0; i < MAX_PARTICLES; i++)
    {
        struct particle *p = &particles[i];

        if (p->life <= 0)
            continue;

        p->x += p->vx;
        p->y += p->vy;
        p->vx *= p->drag;
        p->vy *= p->drag;
        p->size -= p->shrink;
        p->life--;

        if (p->life <= 0 || p->size <= 0.5f)
        {
            p->life = 0;
            continue;
        }

        // Fade out during the second half of the life
        float fade = p->life > p->max_life / 2 ? 1.0f : p->life / (float) (p->max_life / 2);

        SDL_SetRenderDrawColor(window->renderer, p->r, p->g, p->b, (Uint8) (255 * fade));
        fill_square(window, p->x + window->shake.x, p->y + window->shake.y, p->size);
    }

    // Shockwaves
    for (unsigned i = 0; i < MAX_SHOCKWAVES; i++)
    {
        struct shockwave *w = &shockwaves[i];

        if (w->frame < 0)
            continue;

        float t = w->frame / (float) SHOCKWAVE_FRAMES;
        float radius = 40 + t * t * 1500 + t * 300; // Fast at first, covers the whole screen at the end
        Uint8 alpha = (Uint8) (255 * (1 - t));

        SDL_SetRenderDrawColor(window->renderer, 120, 200, 255, alpha);

        for (int j = 0; j < SHOCKWAVE_POINTS; j++)
        {
            float angle = j * 2 * 3.14159265f / SHOCKWAVE_POINTS;
            fill_square(window, w->x + cosf(angle) * radius, w->y + sinf(angle) * radius, 12 * (1 - t) + 3);
        }

        w->frame++;
        if (w->frame >= SHOCKWAVE_FRAMES)
            w->frame = -1;
    }

    // White flash
    if (flash_alpha > 0)
    {
        SDL_SetRenderDrawColor(window->renderer, 255, 255, 255, flash_alpha);
        SDL_RenderFillRect(window->renderer, NULL);
        flash_alpha -= 8;
    }

    SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);
}
