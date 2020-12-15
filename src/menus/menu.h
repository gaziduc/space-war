#ifndef MENU_H
#define MENU_H

#define TITLE_ALPHA_MAX 255
#define NUM_ITEMS 6

void render_controller_input_texts(struct window *window, Uint32 begin, int display_back);

void render_stars(struct window *window);

void menu(struct window *window);

void select_num_players(struct window *window);

#endif /* !MENU_H */
