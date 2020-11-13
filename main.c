
#include "c_string.c"

int main()
{
    String_T total_name = InitStr("Samuel Isaac");
    String_T first_name = SubStrValue(&total_name, "S", "c");

    printf("%s\n", first_name.data);


    return 0;
}