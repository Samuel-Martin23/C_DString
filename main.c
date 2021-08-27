#include "dstring.h"

int main()
{
    string_t *file = str_alloc_read_keyboard("\nHow are you doing today? ");

    str_print(file, "", "\n");

    str_free(&file);

    return 0;
}
