#ifndef SETTING_H
#define SETTING_H

#define NUM_SETTINGS 9
#define NUM_TITLES_SETTINGS 3

enum settings_titles
{
    AUDIO = 1,
    VIDEO = 4,
    INPUTS = 7
};

int is_fullscreen(struct window *window);

void load_settings(struct window *window);

void write_settings(struct window *window);

void settings(struct window *window);

#endif /* !SETTING_H */
