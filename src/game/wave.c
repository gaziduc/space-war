#include "utils.h"
#include "path.h"
#include "init.h"
#include "wave.h"
#include "menu.h"
#include "string_vec.h"
#include "net.h"
#include <SDL2/SDL.h>


void render_title(struct window *window, Uint8 alpha, char* override_title)
{
    SDL_Color fg = { 0, 255, 255, alpha };

    render_text(window, window->fonts->craft_large,
                override_title ? override_title : window->paths->data[window->paths->index].line.title,
                fg, POS_CENTERED, 40);
}


void render_wave_title(struct window *window)
{
    // While there are still enemies, wait
    if (window->list[ENEMY_LIST]->next || window->list[BOSS_LIST]->next)
        return;

    Uint32 ticks = SDL_GetTicks();

    if (!window->is_wave_title)
    {
        window->wave_title_time = ticks;
        window->is_wave_title = 1;

        if (window->server)
        {
            

            struct msg msg = { .type = WAVE_TITLE_MSG };
            msg.content.string_vec = create_string(window);
            add_string(window, msg.content.string_vec, window->paths->data[window->paths->index].line.title);
            send_msg(window, &msg);

            free_string(msg.content.string_vec);
        }

        return;
    }

    Uint32 diff = ticks - window->wave_title_time;

    if (diff < TITLE_ALPHA_MAX)
        render_title(window, diff, NULL);
    else if (diff < TIME_SHOW_TITLE - TITLE_ALPHA_MAX)
        render_title(window, TITLE_ALPHA_MAX, NULL);
    else if (diff < TIME_SHOW_TITLE)
        render_title(window, TIME_SHOW_TITLE - diff, NULL);
    else
    {
        window->last_enemy_time = ticks;
        window->paths->index++;
        window->is_wave_title = 0;
    }
}
