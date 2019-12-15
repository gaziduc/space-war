#ifndef BOSS_H
#define BOSS_H

#define FRAMES_BETWEEN_BOSS_SHOTS 60

void set_boss_attributes(struct list *new, SDL_Rect *pos, struct window *window);

void create_boss(struct window *window);

void move_boss(struct window *window, SDL_Rect *ship_pos);

void render_boss(struct window *window);

void render_boss_health(struct window *window);

#endif /* !BOSS_H */
