
#include "c_string.c"

int main()
{
    String_T total_name = InitStr("\n    Samuel");
    AppendStr(&total_name, " Isaac Martin   \n");
    StripStr(&total_name);
    String_T first_name = SubStrValue(&total_name, "S", "l");
    String_T last_name = SubStrIndex(&total_name, 13, 19);
    ReplaceStr(&first_name, "uel", "my", 0);
    EraseStrValue(&total_name, "Isaac ");
    String_Array_T split_name = SplitStr(&total_name, " ", 0);

    PrintStr(total_name, "", "\n");
    PrintStr(first_name, "", "\n");
    PrintStr(last_name, "", "\n");
    PrintStrArray(split_name, "", "\n");

    FreeStr(&total_name);
    FreeStr(&first_name);
    FreeStr(&last_name);
    FreeStrArray(&split_name);

    PrintAllocatedMemory();

    return 0;
}