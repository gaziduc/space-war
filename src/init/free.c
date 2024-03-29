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
    for (unsigned i = 0; i < NUM_ANIM_SHIP; i++)
        free_collision_texture(window->img->ship[i]);

    for (int i = 0; i < 3; i++)
        free_collision_texture(window->img->shot[i]);

    free_collision_texture(window->img->enemy);
    free_collision_texture(window->img->enemy_shot);
    free_collision_texture(window->img->enemy_shot_teleguided);
    free_collision_texture(window->img->enemy_shot_explode);
    SDL_DestroyTexture(window->img->explosion[0]);
    SDL_DestroyTexture(window->img->explosion[1]);
    SDL_DestroyTexture(window->img->trail);
    free_collision_texture(window->img->bosses[0]);
    free_collision_texture(window->img->bosses[1]);
    free_collision_texture(window->img->bosses[2]);
    free_collision_texture(window->img->bosses[9]);

    for (enum object_type i = 0; i < NUM_OBJECTS; i++)
        free_collision_texture(window->img->objects[i]);

    free_collision_texture(window->img->asteroid);
    SDL_DestroyTexture(window->img->aura);

    for (size_t i = 0; i < NUM_ROTATING_FRAMES; i++)
        free_collision_texture(window->img->rotating_enemy[i]);

    free_collision_texture(window->img->drone);
    free_collision_texture(window->img->wall);

    SDL_DestroyTexture(window->img->a_button);
    SDL_DestroyTexture(window->img->b_button);
    SDL_DestroyTexture(window->img->trophy);
    SDL_DestroyTexture(window->img->bomb);
    SDL_DestroyTexture(window->img->padlock);
    SDL_DestroyTexture(window->img->play_icon);
    SDL_DestroyTexture(window->img->wheel);
    SDL_DestroyTexture(window->img->help);
    SDL_DestroyTexture(window->img->copyright);
    SDL_DestroyTexture(window->img->exit_logo);

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
    TTF_CloseFont(window->fonts->craft);
    TTF_CloseFont(window->fonts->craft_small);
    TTF_CloseFont(window->fonts->craft_large);
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
    Mix_FreeChunk(window->sounds->trophy);
    free(window->sounds);
}


void free_all(struct window *window, int is_in_level)
{
    free_textures(window);
    free(window->in);
    free(window->fps);

    for (enum list_type i = 0; i < NUM_LISTS; i++)
        free_list(window->list[i]);

    if (is_in_level && window->paths) // window->paths check is important because is_in_level can be true...
         free_vector(window->paths);  // ...even if you are not in a level

    free_fonts(window);
    Mix_CloseAudio();
    free_sounds(window);
    Mix_FreeMusic(window->music);
    free_universe(window->universe);
    free(window->save);
    free(window->settings);
    free_window(window);

    SDLNet_Quit();
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
