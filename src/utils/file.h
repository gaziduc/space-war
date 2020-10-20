#ifndef FILE_H
#define FILE_H

struct string_vec* dump_file_in_string(char *filename, struct window *window);

void go_to_next_line(size_t *index, char *s);

#endif // FILE_H
