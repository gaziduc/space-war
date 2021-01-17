#ifndef COLLISION_H
#define COLLISION_H

#define HEALTH_TO_DECREASE_WHEN_HURT 10
#define SCORE_TO_INCREASE 100

void hurt(struct window *window, struct player *player);

void check_collisions(struct window *window, struct player *player);

#endif /* !COLLISION_H */
