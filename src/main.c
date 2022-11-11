#include "utils.h"
#include "init.h"
#include "free.h"
#include "menu.h"
#include "intro.h"
#include "level.h"
#include "game.h"
#include "setting.h"
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Cast argc and argv to avoid warnings
    (void) argc;
    (void) argv;

    // Init SDL2, window, renderer and load textures, events, ...
    struct window *window = init_all();

    // Game intro
    intro(window);

    // Tutorial
    if (window->settings->show_tutorial)
    {
        window->is_lan = 0;
        window->num_players = 1;
        window->level_num = 0;
        window->level_difficulty = EASY;
        play_game(window);

        window->settings->show_tutorial = 0;
        write_settings(window);
    }

    // Menu loop
    menu(window);

    // Free all memory and exit
    free_all(window, 0);
    return EXIT_SUCCESS;
}
