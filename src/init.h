#ifndef INIT_H
#define INIT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_framerate.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#define NUM_LEVELS 6

enum object_type
{
    HEALTH = 0,
    SHIELD,
    NUM_OBJECTS
};

struct collision_texture
{
    SDL_Texture *texture;
    int w;
    int h;
    short *collision;
};

struct textures
{
    struct collision_texture *ship;
    struct collision_texture *shot;
    struct collision_texture *enemy;
    SDL_Texture *explosion;
    SDL_Texture *trail;
    struct collision_texture *enemy_shot;
    struct collision_texture *boss;
    struct collision_texture *objects[NUM_OBJECTS];
    struct collision_texture *asteroid;
    SDL_Texture *aura;
};


struct axis
{
    Sint16 value;
    int state;
};

struct controller
{
    SDL_GameController *controller;
    Sint32 id;
    char button[SDL_CONTROLLER_BUTTON_MAX];
    struct axis axis[SDL_CONTROLLER_AXIS_MAX];
    SDL_Haptic *haptic;
};

struct input
{
    char quit;
    char key[SDL_NUM_SCANCODES];
    struct controller c;
};

struct list
{
    SDL_Rect pos_src;
    SDL_Rect pos_dst;
    unsigned long framecount;
    SDL_Point speed;
    int health;
    int max_health;
    Uint32 last_time_hurt;
    enum object_type type;
    char enemy_type;
    struct collision_texture *texture;
    struct list *next;
};

enum list_type
{
    MY_SHOTS_LIST = 0,
    ENEMY_LIST,
    EXPLOSION_LIST,
    ENEMY_SHOT_LIST,
    BOSS_LIST,
    OBJECT_LIST,
    HUD_LIST,
    NUM_LISTS
};

struct vector
{
    struct path *data;
    size_t size;
    size_t capacity;
    size_t index;
};

struct fonts
{
    TTF_Font *pixel;
    TTF_Font *zero4b_30;
    TTF_Font *zero4b_30_small;
    TTF_Font *zero4b_30_extra_small;
};

struct sounds
{
    Mix_Chunk *shot;
    Mix_Chunk *explosion;
    Mix_Chunk *power_up;
    Mix_Chunk *select;
    Mix_Chunk *play;
};

struct settings
{
    int is_fullscreen;
    int music_volume;
    int sfx_volume;
    int is_force_feedback;
};

struct save
{
    int progress[NUM_LEVELS];
};

struct window
{
    SDL_Window *window;
    int w;
    int h;
    SDL_Renderer *renderer;
    struct textures *img;
    struct input *in;
    FPSmanager *fps;
    struct list *list[NUM_LISTS];
    Uint32 last_shot_time;
    struct vector *paths;
    Uint32 last_enemy_time;
    int health;
    int max_health;
    int animated_health_low;
    int animated_health_high;
    int lives;
    struct fonts *fonts;
    int score;
    Mix_Music *music;
    struct sounds *sounds;
    int respawn_frame;
    int is_wave_title;
    Uint32 wave_title_time;
    int num_bombs;
    struct universe *universe;
    struct point *stars;
    int bonus;
    struct settings *settings;
    int shield_time;
    int ammo;
    struct save *save;
};

void load_music(struct window *window, const char *filename, int must_free);

struct window *init_all(int width, int height);

#endif /* !INIT_H */
