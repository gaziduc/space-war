#include "utils.h"
#include "init.h"
#include "list.h"
#include "path.h"
#include "game.h"
#include "stars.h"
#include "setting.h"
#include "save.h"
#include "event.h"
#include "language.h"
#include "msg_list.h"
#include "menu.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_rotozoom.h>


void render_loading_screen(struct window *window)
{
    static Uint32 last_time = 0;
    Uint32 time = SDL_GetTicks();

    static int progress = 10;

    progress += 3;

    if (time - last_time > 16)
    {
        last_time = time;

        SDL_Color white = { 195, 195, 195, 255 };
        SDL_Color blue = { .r = BLUE_R, .g = BLUE_G, .b = BLUE_B, .a = TITLE_ALPHA_MAX };

        char s[128] = { 0 };
        sprintf(s, "%d %%", progress);

        SDL_SetRenderDrawColor(window->renderer, 8, 8, 8, 255);
        SDL_RenderClear(window->renderer);
        render_text(window, window->fonts->zero4b_30, "SPACE WAR", blue, POS_CENTERED, 200);
        render_text(window, window->fonts->pixel_large, s, white, POS_CENTERED, 675);
        SDL_RenderPresent(window->renderer);
    }
}


static void add_resolution(struct window *window, int index, int w, int h)
{
    window->resolutions[index].x = w;
    window->resolutions[index].y = h;
}

static struct window *init_window(void)
{
    struct window *window = xcalloc(1, sizeof(struct window), NULL, NULL);

    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
        error("SDL_GetDesktopDisplayMode failed", SDL_GetError(), window->window, window->renderer);

    add_resolution(window, 0, dm.w, dm.h);
    add_resolution(window, 1, 1280, 720);
    add_resolution(window, 2, 1366, 768);
    add_resolution(window, 3, 1600, 900);
    add_resolution(window, 4, DEFAULT_W, DEFAULT_H);
    add_resolution(window, 5, 2560, 1440);
    add_resolution(window, 6, 3840, 2160);

    load_settings(window);

    window->window = SDL_CreateWindow("Space War",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      window->w,
                                      window->h,
                                      window->settings->is_fullscreen ? SDL_WINDOW_FULLSCREEN
                                                                      : 0);

    if (!window->window)
        error("Could not create window", SDL_GetError(), NULL, NULL);

    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_PRESENTVSYNC);

    if (!window->renderer)
        error("Could not create renderer", SDL_GetError(), window->window, NULL);


    // To fix a bug on linux...
    SDL_SetWindowPosition(window->window,
                          SDL_WINDOWPOS_CENTERED,
                          SDL_WINDOWPOS_CENTERED);

    return window;
}


static void load_textures(struct window *window)
{
    window->img = xmalloc(sizeof(struct textures), window->window, window->renderer);

    window->img->ship = load_texture_collision("data/ship.png", window);
    window->img->shot[0] = load_texture_collision("data/shot0.bmp", window);
    window->img->shot[1] = load_texture_collision("data/shot1.bmp", window);
    window->img->shot[2] = load_texture_collision("data/shot2.bmp", window);
    window->img->enemy = load_texture_collision("data/enemy.png", window);
    window->img->explosion = load_texture("data/explosion.png", window);
    window->img->trail = load_texture("data/trail.png", window);
    window->img->enemy_shot = load_texture_collision("data/enemy_shot.png", window);
    window->img->boss = load_texture_collision("data/boss.png", window);
    window->img->objects[HEALTH] = load_texture_collision("data/health.png", window);
    window->img->objects[SHIELD] = load_texture_collision("data/shield.png", window);
    window->img->objects[PLANET] = load_texture_collision("data/planet.png", window);
    window->img->objects[GALAXY] = load_texture_collision("data/galaxy.png", window);
    window->img->objects[MISSILE_AROUND] = load_texture_collision("data/missile.bmp", window);
    window->img->aura = load_texture("data/aura.png", window);
    window->img->asteroid = load_texture_collision("data/asteroid.png", window);

    // Load/create rotating enemy animation
    SDL_Surface *original = IMG_Load("data/rotating_enemy.png");
    if (!original)
        error("Could not load surface", IMG_GetError(), window->window, window->renderer);

    window->img->rotating_enemy[0] = get_texture_collision(original, window);

    for (size_t i = 1; i < NUM_ROTATING_FRAMES; i++)
    {
        SDL_Surface *temp = rotozoomSurface(original, i * 2.0, 1.0, SMOOTHING_ON);
        window->img->rotating_enemy[i] = get_texture_collision(temp, window);

        SDL_FreeSurface(temp);
    }

    SDL_FreeSurface(original);

    window->img->drone = load_texture_collision("data/drone.png", window);
    window->img->final_boss = load_texture_collision("data/final_boss.png", window);
    window->img->wall = load_texture_collision("data/wall.png", window);
    window->img->a_button = load_texture("data/a_button.png", window);
    window->img->b_button = load_texture("data/b_button.png", window);
    window->img->trophy = load_texture("data/trophy_gold.png", window);
}


static void load_fonts(struct window *window)
{
    window->fonts = xmalloc(sizeof(struct fonts), window->window, window->renderer);

    window->fonts->pixel_small_bold = load_font(window, "data/pixel.ttf", 18);
    window->fonts->pixel = load_font(window, "data/pixel.ttf", 30);
    window->fonts->pixel_large = load_font(window, "data/pixel.ttf", 37);
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
        error("Could not load music", Mix_GetError(), window->window, window->renderer);

    if (Mix_PlayMusic(window->music, -1) == -1)
        error("Could not play music", Mix_GetError(), window->window, window->renderer);
}

static void load_sounds(struct window *window)
{
    window->sounds = xmalloc(sizeof(struct sounds), window->window, window->renderer);

    window->sounds->shot = load_sound(window, "data/shot.wav");
    window->sounds->explosion = load_sound(window, "data/explosion.wav");
    window->sounds->power_up = load_sound(window, "data/power_up.wav");
    window->sounds->select = load_sound(window, "data/select.wav");
    window->sounds->play = load_sound(window, "data/play.wav");
    window->sounds->trophy = load_sound(window, "data/trophy.wav");
}


struct window *init_all(void)
{
    // Init SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) != 0)
        error("Could not load SDL2", SDL_GetError(), NULL, NULL);

    // Create window and renderer
    struct window *window = init_window();

    // Enable blending (alpha)
    SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);

    // Window icon
    SDL_Surface *icon = SDL_LoadBMP("data/icon.bmp");
    if (!icon)
        error("Could not load surface", SDL_GetError(), window->window, window->renderer);
    SDL_SetWindowIcon(window->window, icon);
    SDL_FreeSurface(icon);

    // Init SDL2_image
    int img_flags = IMG_INIT_PNG;
    int img_initted = IMG_Init(img_flags);
    if ((img_initted & img_flags) != img_flags)
        error("Could not load SDL2_image", IMG_GetError(), window->window, window->renderer);

    // Init SDL2_tff
    if (TTF_Init() == -1)
        error("Could not load SDL2_ttf", TTF_GetError(), window->window, window->renderer);

    load_fonts(window);

    load_language_file(window, "data/en.txt");

    // Load textures
    load_textures(window);

    // Init inputs
    window->in = xcalloc(1, sizeof(struct input), window->window, window->renderer);

    // Init controller
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        if (SDL_IsGameController(i))
        {
            init_controller(window->in, i);
            break;
        }
    }

    // Init framerate manager
    window->fps = xmalloc(sizeof(FPSmanager), window->window, window->renderer);
    SDL_initFramerate(window->fps);
    if (SDL_setFramerate(window->fps, 60) == -1)
        error("Could not set framerate", "Could not set framerate to 60 Hz", window->window, window->renderer);

    // Init linked lists for shots, enemies, ...
    for (enum list_type i = 0; i < NUM_LISTS; i++)
        init_list(window, i);

    // We can now shot
    for (unsigned i = 0; i < window->num_players; i++)
        window->player[i].last_shot_time = 0;

    window->paths = NULL;

    // Init SDL2_mixer
    int mix_flags = MIX_INIT_OGG;
    int mix_initted = Mix_Init(mix_flags);
    if ((mix_initted & mix_flags) != mix_flags)
        error("Could not load SDL2_mixer", Mix_GetError(), window->window, window->renderer);

    // Load music with volume settings
    Mix_AllocateChannels(32);

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        error("Could not initialize SDL2_mixer", Mix_GetError(), window->window, window->renderer);

    Mix_VolumeMusic(window->settings->music_volume);
    Mix_Volume(-1, window->settings->sfx_volume);

    load_sounds(window);

    // Init SDL2_net
    if (SDLNet_Init() == -1)
        error("Could not initialize SDL2_net", SDLNet_GetError(), window->window, window->renderer);

    // Initialize the stars lib
    new_universe(&window->universe, 256, window);

    // Load save file and progress
    read_save(window);

    // LAN meesages
    window->msg_list = xmalloc(sizeof(struct msg_list), window->window, window->renderer);
    window->msg_list->next = NULL;

    // Trophies
    window->trophy.id[0] = -1;

    return window;
}
