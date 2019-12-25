#ifndef OBJECT_H
#define OBJECT_H

void set_object_attributes(struct list *new, enum object_type type, struct window *window);

void create_object(struct window *window, enum object_type type);

void move_objects(struct window *window);

void render_objects(struct window *window);

#endif /* !OBJECT_H */
