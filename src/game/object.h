#ifndef OBJECT_H
#define OBJECT_H

#define SHIELD_TIME 20000

void set_object_attributes(struct list *new, enum object_type type,
                           struct collision_texture *collision, SDL_FRect *override_pos);

void create_object(struct window *window, enum object_type type);

void move_objects(struct window *window);

void render_pre_bg_objects(struct window *window);

void render_post_bg_objects(struct window *window);

void render_shield_aura(struct window *window, struct player *player);

#endif /* !OBJECT_H */
