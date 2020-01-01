#include "utils.h"
#include "init.h"
#include "list.h"
#include "path.h"
#include "game.h"
#include "stars.h"
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

    window->img->ship = load_texture_collision("data/ship.png", window);
    window->img->shot = load_texture_collision("data/shot.bmp", window);
    window->img->enemy = load_texture_collision("data/enemy.png", window);
    window->img->explosion = load_texture("data/explosion.png", window);
    window->img->trail = load_texture("data/trail.png", window);
    window->img->enemy_shot = load_texture_collision("data/enemy_shot.bmp", window);
    window->img->boss = load_texture_collision("data/boss.png", window);
    window->img->health = load_texture_collision("data/health.png", window);
    window->img->asteroid = load_texture_collision("data/asteroid.png", window);
}


static void load_fonts(struct window *window)
{
    window->fonts = xmalloc(sizeof(struct fonts), window->window);

    window->fonts->pixel = load_font(window, "data/pixel.ttf", 26);
    window->fonts->zero4b_30 = load_font(window, "data/04b_30.ttf", 120);
    window->fonts->zero4b_30_small = load_font(window, "data/04b_30.ttf", 60);
    window->fonts->zero4b_30_extra_small = load_font(window, "data/04b_30.ttf", 40);
}

void load_music(struct window *window, const char *filename, int must_free)
{
    if (must_free)
        Mix_FreeMusic(window->music);

    window->music = Mix_LoadMUS(filename);
    if (!window->music)
        error("Could not load music", Mix_GetError(), window->window);

    if (Mix_PlayMusic(window->music, -1) == -1)
        error("Could not play music", Mix_GetError(), window->window);
}

static void load_sounds(struct window *window)
{
    window->sounds = xmalloc(sizeof(struct sounds), window->window);

    window->sounds->shot = load_sound(window, "data/shot.wav");
    window->sounds->explosion = load_sound(window, "data/explosion.wav");
    window->sounds->power_up = load_sound(window, "data/power_up.wav");
    window->sounds->select = load_sound(window, "data/select.wav");
    window->sounds->play = load_sound(window, "data/play.wav");
}

struct window *init_all(int width, int height)
{
    // Init SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0)
        error("Could not load SDL2", SDL_GetError(), NULL);

    SDL_ShowCursor(SDL_DISABLE);

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

    // Init controller
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (SDL_IsGameController(i))
        {
            window->in->c.controller = SDL_GameControllerOpen(i);
            window->in->c.id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(window->in->c.controller));
            break;
        }
        else
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Controller not reconized",
                                     "Your controller is not compatible.", window->window);
    }

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
    window->paths = NULL;

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

    load_music(window, "data/hybris.ogg", 0);
    load_sounds(window);

    // Initialize the stars lib
    new_universe(&window->universe, window->w, window->h, 256, window);

    return window;
}
