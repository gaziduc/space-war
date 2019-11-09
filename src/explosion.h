#ifndef EXPLOSION_H
#define EXPLOSION_H

#define EXPLOSION_SIZE 256

void set_explosion_pos(struct list *new, SDL_Rect *pos_dst, struct window *window);

void move_explosions(struct window *window);

void render_explosions(struct window *window);

#endif /* !EXPLOSION_H */
