#include "c_string.h"

int main()
{
    string_t *last_name = NULL;
    string_t *full_name = NULL;
    string_array_t *split_name = NULL;

    init_str(&full_name, "  \nSamuel\n  ");

    strip_str(full_name);
    strip_str_chars(full_name, "\n");
    append_str(full_name, " Martin");

    sub_str(&last_name, full_name, 7, 0, 1);

    erase_str(full_name, "tin");
    split_str(&split_name, full_name, "a", 0);

    free_str(&last_name);
    free_str(&full_name);
    free_str_array(&split_name);

    return 0;
}
