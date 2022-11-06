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



