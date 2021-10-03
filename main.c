#include "dstring.h"

int main()
{
    dstr_t *full_name = dstr_alloc( "  \nSamuel\n  ");
    dstr_strip(full_name);
    dstr_append(full_name, " Martin");

    int64_t start = 7;
    dstr_t *last_name = dstr_alloc_subdstr(full_name, &start, 0, 0);

    dstr_erase(full_name, "tin");
    dstr_arr_t *split_name = dstr_alloc_splitdstr(full_name, "a", 0);

    dstr_free(&last_name);
    dstr_free(&full_name);
    dstr_arr_free(&split_name);
    return 0;
}
