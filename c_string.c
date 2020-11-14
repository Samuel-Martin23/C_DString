
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
    if (*value == '\0' || max_split < 0)
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
    int size = strlen(str_value);

    result.size = orginal_str->size + size;

    if (orginal_str->check_allocation)
    {
        allocated_mem += size;

        result.data = realloc((char*)orginal_str->data, sizeof(char) * (result.size + 1));
        strcat((char*)result.data, str_value);
        result.check_allocation = true;
        *orginal_str = result;
    }
    else
    {
        allocated_mem += result.size + 1;

        result.data = malloc(sizeof(char) * (result.size + 1));
        memcpy((char*)result.data, orginal_str->data, orginal_str->size);
        strcat((char*)result.data, str_value);
        *((char*)result.data + result.size) = '\0';
        result.check_allocation = true;
        *orginal_str = result;
    }
}

String_T SubStrIndex(String_T *orginal_str, int low_range, int high_range)
{
    if (low_range >= high_range || high_range > orginal_str->size)
    {
        return InitStr("");
    }

    int total_size = (high_range - low_range) + 1;

    String_T result;
    allocated_mem += sizeof(char) * total_size;
    result.data = malloc(sizeof(char) * total_size);
    result.check_allocation = true;

    memcpy((char*)result.data, orginal_str->data + low_range, total_size-1);
    *((char*)result.data + total_size) = '\0';
    result.size = strlen(result.data);

    return result;
}

String_T SubStrValue(String_T *orginal_str, const char *value1, const char *value2)
{
    if (*value1 == '\0' || *value2 == '\0')
    {
        return InitStr("");
    }

    int low = CountIndexInStr(orginal_str, value1);

    if (low == -1)
    {
        return InitStr("");
    }

    int high = 0;
    const char *str_value = orginal_str->data;

    str_value += low;

    String_T str_at_low_index = InitStr(str_value);
    high = CountIndexInStr(&str_at_low_index, value2);

    if (high == -1)
    {
        return InitStr("");
    }

    return SubStrIndex(&str_at_low_index, 0, high + strlen(value2));
}

void ReplaceStr(String_T *orginal_str, const char *old_str, const char *new_str, int max_split)
{
    int num_of_occurrences = CountOccurrencesInStr(orginal_str, old_str, max_split);

    if (num_of_occurrences == -1)
    {
        return;
    }

    int i = 0;
    int num_of_replacements = 0;
    int old_str_size = strlen(old_str);
    int total_size = ((orginal_str->size - (old_str_size * num_of_occurrences)) + (strlen(new_str) * num_of_occurrences) + 1);

    allocated_mem += sizeof(char) * total_size;

    const char *copy_new_str = new_str;
    const char *str_data = orginal_str->data;
    const char *replace_str = malloc(sizeof(char) * total_size);

    while (*str_data != '\0')
    {
        if (num_of_replacements < num_of_occurrences && CheckSubStr(str_data, old_str))
        {
            while (*new_str != '\0')
            {
                *((char*)replace_str + i) = *new_str;
                i++;
                new_str++;
            }

            num_of_replacements++;
            new_str = copy_new_str;
            str_data += old_str_size;
            continue;
        }

        *((char*)replace_str + i) = *str_data;
        i++;
        str_data++;
    }

    *((char*)replace_str + i) = '\0';

    String_T result = InitStr(replace_str);
    result.check_allocation = true;
    FreeStr(orginal_str);
    *orginal_str = result;
}


void EraseStrValue(String_T *orginal_str, const char *value)
{
    ReplaceStr(orginal_str, value, "", 0);
}

void EraseStrIndex(String_T *orginal_str, int low_range, int high_range)
{
    String_T result = SubStrIndex(orginal_str, low_range, high_range);
    EraseStrValue(orginal_str, result.data);
    FreeStr(&result);
}

void StripStr(String_T *orginal_str)
{
    const char *str_data = orginal_str->data;

    while (*str_data == ' ' || *str_data == '\n')
    {
        str_data++;
    }

    if (str_data == '\0')
    {
        *orginal_str = InitStr("");
        return;
    }

    int size = strlen(str_data);
    const char *forward = str_data + size;

    while (*(forward-1) == ' ' || *(forward-1) == '\n')
    {
        forward--;
        size--;
    }

    allocated_mem += sizeof(char) * (size + 1);

    const char *result = malloc(sizeof(char) * (size + 1));
    memcpy((char*)result, str_data, size);
    *((char*)result + size) = '\0';
    
    FreeStr(orginal_str);

    orginal_str->size = strlen(result);
    orginal_str->data = result;
    orginal_str->check_allocation = true;
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
    int total_len = 0;

    const char *split_str = "";
    char *str_value = strdup(orginal_str->data);

    result.size = num_of_occurrences + 1;
    result.data_set = malloc(result.size * sizeof(String_T));
    allocated_mem += (result.size * sizeof(String_T));

    while ((split_str = strsep(&str_value, separator)) != NULL && i < num_of_occurrences)
    {
        result.data_set[i] = InitStr(split_str);
        AppendStr(&result.data_set[i], "");

        total_len += result.data_set[i].size;
        i++;
    }

    result.data_set[i] = InitStr(orginal_str->data + total_len + (num_of_occurrences * strlen(separator)));
    AppendStr(&result.data_set[i], "");

    return result;
}

void PrintStr(String_T orginal_str, const char *beginning, const char *end)
{
    printf("%s%s%s", beginning, orginal_str.data, end);
}

void PrintStrArray(String_Array_T orginal_str_array, const char *beginning, const char *end)
{
    int i = 0;
    printf("%s{", beginning);

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