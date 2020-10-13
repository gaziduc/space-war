#ifndef MENU_H
#define MENU_H

#define TITLE_ALPHA_MAX 240
#define NUM_ITEMS 4

void render_stars(struct window *window);

void menu(struct window *window);

void select_num_players(struct window *window);

#endif /* !MENU_H */
