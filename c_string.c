
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

typedef enum boolean {false, true} bool;

typedef struct String
{
    int size;
    const char *data;
    bool check_allocation;
} String_T;

typedef struct String_Array
{
    int size;
    String_T *data_set;
} String_Array_T;

int allocated_mem = 0;

String_T InitStr(const char *str_literal)
{
    String_T curr_str;
    curr_str.size = strlen(str_literal);
    curr_str.data = str_literal;
    curr_str.check_allocation = false;
    return curr_str;
}

void FreeStr(String_T *orginal_str)
{
    if (orginal_str->check_allocation == false)
    {
        return;
    }

    allocated_mem -= (orginal_str->size + 1);

    free((char*)orginal_str->data);
    orginal_str->size = 0;
    orginal_str->check_allocation = false;
    orginal_str->data = NULL;
}

void FreeStrArray(String_Array_T *orginal_str_array)
{
    String_T *curr = orginal_str_array->data_set;

    for (int i = 0; i < orginal_str_array->size; i++)
    {
        FreeStr(curr);
        curr++;
    }

    allocated_mem -= (orginal_str_array->size * sizeof(String_T));

    free(orginal_str_array->data_set);
    orginal_str_array->size = 0;
    orginal_str_array->data_set = NULL;
}

bool CheckSubStr(const char *str_value, const char *sub_value)
{
    while (*sub_value != '\0')
    {
        if (*str_value == '\0' || *str_value != *sub_value)
        {
            return false;
        }

        str_value++;
        sub_value++;
    }

    return true;
}

int CountIndexInStr(String_T *orginal_str, const char *value)
{
    int count = 0;
    const char *str_data = orginal_str->data;

    while (*str_data != '\0')
    {
        if (CheckSubStr(str_data, value))
        {
            return count;
        }

        count++;
        str_data++;
    }

    return -1;
}

int CountOccurrencesInStr(String_T *orginal_str, const char *value, int max_split)
{
    if (*value == '\0')
    {
        return -1;
    }

    int count = 0;
    const char *str_data = orginal_str->data;

    while (*str_data != '\0')
    {
        if (CheckSubStr(str_data, value))
        {
            count++;
        }

        str_data++;
    }

    if (count == 0)
    {
        return -1;
    }
    else if (max_split > 0 && max_split < count)
    {
        count = max_split;
    }

    return count;
}

void AppendStr(String_T *orginal_str, const char *str_value)
{
    String_T result;
    result.size = orginal_str->size + strlen(str_value);
    result.check_allocation = false;

    if (orginal_str->check_allocation)
    {
        allocated_mem += strlen(str_value);
        result.data = realloc((char*)orginal_str->data, sizeof(char) * (result.size + 1));
        strcat((char*)result.data, str_value);
        result.check_allocation = true;
        *orginal_str = result;
    }
    else if (orginal_str->data)
    {
        allocated_mem += result.size + 1;
        result.data = malloc(sizeof(char) * (result.size + 1));
        strcpy((char*)result.data, orginal_str->data);
        strcat((char*)result.data, str_value);
        result.check_allocation = true;
        *orginal_str = result;
    }
}

String_T SubStrIndex(String_T *orginal_str, int low_range, int high_range)
{
    if (low_range >= high_range || high_range > orginal_str->size)
    {
        return InitStr("Unknown");
    }

    int i = 0;
    int total_size = (high_range - low_range) + 1;
    const char *str_data = orginal_str->data;
    char individual_chars[total_size];

    String_T result = InitStr("");
    allocated_mem += sizeof(char) * total_size;
    result.data = malloc(sizeof(char) * total_size);

    const char *copy_str_data = result.data;

    for (i = 0; i < low_range; i++)
    {
        str_data++;
    }

    for (i = 0; i < total_size-1; i++)
    {
        *(char*)copy_str_data = *str_data;
        str_data++;
        copy_str_data++;
    }
    
    *(char*)copy_str_data = '\0';

    return result;
}

String_T SubStrValue(String_T *orginal_str, const char *value1, const char *value2)
{
    int low = CountIndexInStr(orginal_str, value1);

    if (low == -1)
    {
        return InitStr("Unknown");
    }

    int high = 0;
    const char *str_value = orginal_str->data;

    for (int i = 0; i < low; i++)
    {
        str_value++;
    }

    String_T str_at_low_index = InitStr(str_value);
    high = CountIndexInStr(&str_at_low_index, value2);

    if (high == -1)
    {
        return InitStr("Unknown");
    }
    else if (high + 1 == str_at_low_index.size)
    {
        
    }

    return SubStrIndex(&str_at_low_index, 0, high + 1);
}

void ReplaceStr(String_T *orginal_str, const char *old_str, const char *new_str, int max_split)
{
    if (max_split == 0)
    {
        int num_of_occurrences = CountOccurrencesInStr(orginal_str, old_str, 0);

        if (num_of_occurrences == -1)
        {
            return;
        }

        max_split = num_of_occurrences;
    }

    int i = 0;
    int num_of_replacements = 0;
    int old_str_size = strlen(old_str);

    const char *copy_new_str = new_str;
    const char *str_data = orginal_str->data;
    char individual_chars[orginal_str->size + strlen(new_str) * max_split];

    while (*str_data != '\0')
    {
        if (num_of_replacements < max_split && CheckSubStr(str_data, old_str))
        {
            while (*new_str != '\0')
            {
                individual_chars[i] = *new_str;
                i++;
                new_str++;
            }

            num_of_replacements++;
            new_str = copy_new_str;
            str_data += old_str_size;
            continue;
        }

        individual_chars[i] = *str_data;
        i++;
        str_data++;
    }

    individual_chars[i] = '\0';

    String_T result = InitStr(&individual_chars[0]);
    AppendStr(&result, "");
    FreeStr(orginal_str);
    *orginal_str = result;
}

void EraseStrIndex(String_T *orginal_str, int low_range, int high_range)
{
    String_T result = SubStrIndex(orginal_str, low_range, high_range);
    ReplaceStr(orginal_str, result.data, "", 0);
    FreeStr(&result);
}

void EraseStrValue(String_T *orginal_str, const char *value)
{
    ReplaceStr(orginal_str, value, "", 0);
}

void StripStr(String_T *orginal_str)
{
    while (*orginal_str->data == ' ' || *orginal_str->data == '\n')
    {
        orginal_str->data++;
        orginal_str->size--;
    }

    if (orginal_str->data == '\0')
    {
        *orginal_str = InitStr("");
        return;
    }

    if (orginal_str->check_allocation == false)
    {
        AppendStr(orginal_str, "");
    }

    const char *forward = orginal_str->data + orginal_str->size;

    while (*(forward-1) == ' ' || *(forward-1) == '\n')
    {
        forward--;
        orginal_str->size--;
    }

    *(char*)forward = '\0';
}

String_Array_T SplitStr(String_T *orginal_str, const char *separator, int max_split)
{
    String_Array_T result;
    int num_of_occurrences = CountOccurrencesInStr(orginal_str, separator, max_split);

    if (num_of_occurrences == -1)
    {
        result.size = 1;
        result.data_set = malloc(result.size * sizeof(String_T));
        allocated_mem += (result.size * sizeof(String_T));
        result.data_set[0] = InitStr(orginal_str->data);
        AppendStr(&result.data_set[0], "");
    
        return result;
    }

    int i = 0;
    int j = 0;
    int separator_size = strlen(separator);

    const char *str_data = orginal_str->data;
    char split_string[num_of_occurrences+1][orginal_str->size+1];

    while (*str_data != '\0')
    {
        if (i < num_of_occurrences && CheckSubStr(str_data, separator))
        {
            str_data += separator_size;
            split_string[i][j] = '\0';
            i++;
            j = 0;
            continue;
        }

        split_string[i][j] = *str_data;
        j++;
        str_data++;
    }

    split_string[i][j] = '\0';

    result.size = num_of_occurrences + 1;
    result.data_set = malloc(result.size * sizeof(String_T));
    allocated_mem += (result.size * sizeof(String_T));

    for (int k = 0; k < result.size; k++)
    {
        result.data_set[k] = InitStr(&split_string[k][0]);
        AppendStr(&result.data_set[k], "");
    }

    return result;
}

void PrintStr(String_T orginal_str, const char *beginning, const char *end)
{
    printf("%s%s%s", beginning, orginal_str.data, end);
}

void PrintStrArray(String_Array_T orginal_str_array, const char *beginning, const char *end)
{
    int i = 0;
    printf("%s", beginning);
    printf("{");

    for (i = 0; i < (orginal_str_array.size-1); i++)
    {
        printf("\"%s\", ", orginal_str_array.data_set[i].data);
    }

    printf("\"%s\"}%s", orginal_str_array.data_set[i].data, end);
}

void PrintAllocatedMemory()
{
    printf("Bytes Allocated: %d\n", allocated_mem);
}