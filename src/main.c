#include "utils.h"
#include "init.h"
#include "free.h"
#include "menu.h"
#include "intro.h"
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

    // Menu loop
    menu(window);

    // Free all memory and exit
    free_all(window, 0);
    return EXIT_SUCCESS;
}
