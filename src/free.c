#include "init.h"
#include "free.h"
#include "list.h"
#include "vector.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

static void free_textures(struct window *window)
{
    SDL_DestroyTexture(window->img->ship);
    SDL_DestroyTexture(window->img->shot);
    SDL_DestroyTexture(window->img->bg);
    SDL_DestroyTexture(window->img->enemy);
    SDL_DestroyTexture(window->img->explosion);
    SDL_DestroyTexture(window->img->trail);
    free(window->img);
}

static void free_window(struct window *window)
{
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
    free(window);
}

void free_all(struct window *window)
{
    free_textures(window);
    free(window->in);
    free(window->fps);

    for (enum list_type i = 0; i < NUM_LISTS; i++)
        free_list(window->list[i]);

    free_vector(window->paths);
    free_window(window);
    IMG_Quit();
    SDL_Quit();
}
