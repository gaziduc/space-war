#ifndef BOSS_H
#define BOSS_H

#define FRAMES_BETWEEN_BOSS_1_SHOTS 40
#define FRAMES_BETWEEN_BOSS_2_SHOTS 60
#define FRAMES_BETWEEN_BOSS_3_SHOTS 60
#define FRAMES_BETWEEN_BOSS_4_SHOTS 40
#define FRAMES_BETWEEN_BOSS_5_SHOTS 40
#define FRAMES_BETWEEN_BOSS_6_SHOTS 40
#define FRAMES_BETWEEN_BOSS_7_SHOTS 40
#define FRAMES_BETWEEN_BOSS_8_SHOTS 40
#define FRAMES_BETWEEN_BOSS_9_SHOTS 40
#define FRAMES_BETWEEN_FINAL_BOSS_SHOTS 40

void set_boss_attributes(struct list *new, SDL_FRect *pos,
                         struct window *window, char enemy_type);

void move_boss(struct window *window);

void render_boss(struct window *window);

void render_boss_health(struct window *window);

#endif /* !BOSS_H */
