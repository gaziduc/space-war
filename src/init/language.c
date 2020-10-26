#include "language.h"
#include "init.h"
#include "string_vec.h"
#include "file.h"
#include "utils.h"

void load_language_file(struct window *window, char *filename)
{
    struct string_vec *str = dump_file_in_string(filename, window);

    size_t index = 0;

    for (enum texts i = 0; i < NUM_TXT; i++)
    {
        // Compute line length
        size_t j = 0;
        while (str->ptr[index + j] && str->ptr[index + j] != '\r' && str->ptr[index + j] != '\n')
            j++;

        window->txt[i] = xmalloc(sizeof(char) * (j + 1), window->window, window->renderer);

        j = 0;
        while (str->ptr[index + j] && str->ptr[index + j] != '\r' && str->ptr[index + j] != '\n')
        {
            window->txt[i][j] = str->ptr[index + j];
            j++;
        }

        window->txt[i][j] = '\0';

        index += j + 1;
    }

    free_string(str);
}
