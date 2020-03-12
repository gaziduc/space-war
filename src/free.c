#include "init.h"
#include "free.h"
#include "list.h"
#include "vector.h"
#include "stars.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>


static void free_collision_texture(struct collision_texture *t)
{
    SDL_DestroyTexture(t->texture);
    free(t->collision);
    free(t);
}

static void free_textures(struct window *window)
{
    free_collision_texture(window->img->ship);
    free_collision_texture(window->img->shot);
    free_collision_texture(window->img->enemy);
    free_collision_texture(window->img->enemy_shot);
    SDL_DestroyTexture(window->img->explosion);
    SDL_DestroyTexture(window->img->trail);
    free_collision_texture(window->img->boss);

    for (enum object_type i = 0; i < NUM_OBJECTS; i++)
        free_collision_texture(window->img->objects[i]);

    free_collision_texture(window->img->asteroid);
    SDL_DestroyTexture(window->img->aura);

    free(window->img);
}


static void free_window(struct window *window)
{
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
    free(window);
}


static void free_fonts(struct window *window)
{
    TTF_CloseFont(window->fonts->pixel);
    TTF_CloseFont(window->fonts->pixel_large);
    TTF_CloseFont(window->fonts->zero4b_30);
    TTF_CloseFont(window->fonts->zero4b_30_small);
    TTF_CloseFont(window->fonts->zero4b_30_extra_small);
    free(window->fonts);
}


static void free_sounds(struct window *window)
{
    Mix_FreeChunk(window->sounds->shot);
    Mix_FreeChunk(window->sounds->explosion);
    Mix_FreeChunk(window->sounds->power_up);
    Mix_FreeChunk(window->sounds->select);
    Mix_FreeChunk(window->sounds->play);
    free(window->sounds);
}


void free_all(struct window *window, int is_in_level)
{
    free_textures(window);
    free(window->in);
    free(window->fps);

    for (enum list_type i = 0; i < NUM_LISTS; i++)
        free_list(window->list[i]);

    if (is_in_level)
        free_vector(window->paths);

    free_fonts(window);
    Mix_CloseAudio();
    free_sounds(window);
    Mix_FreeMusic(window->music);
    free_universe(window->universe);
    free(window->save);
    free(window->settings);
    free_window(window);

    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
