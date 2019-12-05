#include "utils.h"
#include "init.h"
#include "list.h"
#include "path.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL_mixer.h>

static struct window *init_window(int width, int height)
{
    struct window *window = xmalloc(sizeof(struct window), NULL);

    window->window = SDL_CreateWindow("Space War",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      width,
                                      height,
                                      SDL_WINDOW_FULLSCREEN);

    if (!window->window)
        error("Could not create window", SDL_GetError(), NULL);

    window->w = width;
    window->h = height;

    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (!window->renderer)
        error("Could not create renderer", SDL_GetError(), NULL);

    return window;
}


static void load_textures(struct window *window)
{
    window->img = xmalloc(sizeof(struct textures), window->window);

    window->img->ship = load_texture("data/ship.png", window);
    window->img->shot = load_texture("data/shot.bmp", window);
    window->img->bg = load_texture("data/background.jpg", window);
    window->img->bg2 = load_texture("data/background2.jpg", window);
    window->img->enemy = load_texture("data/enemy.png", window);
    window->img->explosion = load_texture("data/explosion.png", window);
    window->img->trail = load_texture("data/trail.png", window);
    window->img->enemy_shot = load_texture("data/enemy_shot.bmp", window);
}


static void load_fonts(struct window *window)
{
    window->fonts = xmalloc(sizeof(struct fonts), window->window);

    window->fonts->pixel = load_font(window, "data/pixel.ttf", 26);
}

static void load_music(struct window *window, const char *filename)
{
    window->music = Mix_LoadMUS(filename);
    if (!window->music)
        error("Could not load music", Mix_GetError(), window->window);
}

struct window *init_all(int width, int height)
{
    // Init SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        error("Could not load SDL2", SDL_GetError(), NULL);

    // Create window and renderer
    struct window *window = init_window(width, height);

    // Init SDL2_image
    int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
    int img_initted = IMG_Init(img_flags);
    if ((img_initted & img_flags) != img_flags)
        error("Could not load SDL2_image", IMG_GetError(), window->window);

    // Load textures
    load_textures(window);

    // Init inputs
    window->in = xcalloc(1, sizeof(struct input), window->window);

    // Init framerate manager
    window->fps = xmalloc(sizeof(FPSmanager), window->window);
    SDL_initFramerate(window->fps);
    if (SDL_setFramerate(window->fps, 60) == -1)
        error("Could not set framerate", "Could not set framerate to 60 Hz", window->window);

    // Init linked lists for shots, enemies, ...
    for (enum list_type i = 0; i < NUM_LISTS; i++)
        init_list(window, i);

    // We can now shot
    window->last_shot_time = 0;

    // Load enemy paths and set enemy timer
    window->paths = load_paths(window, "data/paths.txt");
    window->last_enemy_time = 0;

    // Set hud attributes
    window->health = 200;
    window->score = 0;

    // Init SDL2_tff and load fonts
    if (TTF_Init() == -1)
        error("Could not load SDL2_ttf", TTF_GetError(), window->window);

    load_fonts(window);

    // Init SDL2_mixer and load music
    int mix_flags = MIX_INIT_OGG;
    int mix_initted = Mix_Init(mix_flags);
    if ((mix_initted & mix_flags) != mix_flags)
        error("Could not load SDL2_mixer", Mix_GetError(), window->window);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        error("Could not initialize SDL2_mixer", Mix_GetError(), window->window);

    load_music(window, "data/hybris.ogg");
    if (Mix_PlayMusic(window->music, -1) == -1)
        error("Could not play music", Mix_GetError(), window->window);

    return window;
}
