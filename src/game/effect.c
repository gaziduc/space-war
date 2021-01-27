#include "effect.h"
#include "init.h"
#include <SDL2/SDL.h>

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


void force_feedback(struct window *window, struct player *player, float strength, Uint32 length)
{
    if (player->input_type == CONTROLLER
        && window->settings->is_force_feedback
        && window->in->c[player->controller_num].haptic)
        SDL_HapticRumblePlay(window->in->c[player->controller_num].haptic, strength, length);
}
