#ifndef BACKGROUND_H
#define BACKGROUND_H

void init_background(struct window *window);

void move_background(struct window *window);

void render_background(struct window *window);

void free_background(struct points *star);

#endif /* !BACKGROUND_H */
