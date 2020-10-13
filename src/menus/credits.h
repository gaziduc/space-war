#ifndef CREDITS_H
#define CREDITS_H

#include "init.h"

#define CREDITS_CHAR_W 22
#define CREDITS_CHAR_H 34
#define CREDITS_COLS 50
#define CREDITS_LINES 30
#define LEFT_PADDING 150
#define UP_PADDING 150

#define ADD_LETTERS(window, screen, letters, line, col, t1, t2, begin, is_menu) \
        if (add_letters(window, screen, letters, line, col, t1, t2, begin, is_menu)) \
            return;

int add_letters(struct window *window, char screen[][CREDITS_COLS], char *letters,
                size_t *line, size_t *col, Uint32 time_between_two_letters,
                Uint32 time_end, Uint32 begin, int is_in_menu);

void credits(struct window *window);

#endif /* !CREDITS_H */
