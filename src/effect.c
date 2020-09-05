#include "effect.h"
#include "init.h"
#include <SDL2/SDL.h>

void render_touched_effect(struct window *window)
{
    if (window->touched_anim > 0)
    {
        SDL_SetRenderDrawColor(window->renderer, 255, 0, 0, window->touched_anim);
        SDL_RenderFillRect(window->renderer, NULL);
        window->touched_anim -= 4;
    }
}
