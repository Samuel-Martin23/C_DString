#include "c_string.h"

int main()
{
    string_t *full_name = alloc_str( "  \nSamuel\n  ");
    strip_str(full_name);
    append_str(full_name, " Martin");

    int start = 7;
    string_t *last_name = alloc_sub_str(full_name, &start, NULL, NULL);

    erase_str(full_name, "tin");
    string_array_t *split_name = alloc_split_str(full_name, "a", 0);

    free_str(&last_name);
    free_str(&full_name);
    free_str_array(&split_name);

    return 0;
}
