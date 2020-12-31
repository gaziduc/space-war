#ifndef BOSS_H
#define BOSS_H

#define FRAMES_BETWEEN_BOSS_SHOTS 40

void set_boss_attributes(struct list *new, SDL_FRect *pos,
                         struct window *window, char enemy_type);

void move_boss(struct window *window);

void render_boss(struct window *window);

void render_boss_health(struct window *window);

#endif /* !BOSS_H */
