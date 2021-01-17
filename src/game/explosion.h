#ifndef EXPLOSION_H
#define EXPLOSION_H

#define EXPLOSION_1_NUM_TILES_X 8
#define EXPLOSION_1_NUM_TILES_Y 8

#define EXPLOSION_2_NUM_TILES_X 5
#define EXPLOSION_2_NUM_TILES_Y 4

void set_explosion_pos(struct list *new, struct window *window, SDL_FRect *pos_dst, SDL_Texture *texture, int explosion_texture_num);

void move_explosions(struct window *window);

void render_explosions(struct window *window);

#endif /* !EXPLOSION_H */
