#include "dstring.h"

int main()
{
    string_t *full_name = str_alloc( "  \nSamuel\n  ");
    str_strip(full_name);
    str_append(full_name, " Martin");

    int start = 7;
    string_t *last_name = str_alloc_substr(full_name, &start, NULL, NULL);

    str_erase(full_name, "tin");
    string_array_t *split_name = str_alloc_split(full_name, "a", 0);

    str_free(&last_name);
    str_free(&full_name);
    str_array_free(&split_name);	

    return 0;
}
