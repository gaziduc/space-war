#ifndef BACKGROUND_H
#define BACKGROUND_H

struct point
{
    int x;
    int y;
    int z;
    int opacity;
    struct point *next;
};

void init_background(struct window *window);

void move_background(struct window *window, unsigned long framecount);

void render_background(struct window *window);

void free_background(struct point *star);

#endif /* !BACKGROUND_H */
