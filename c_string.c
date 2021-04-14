#include "c_string.h"

#define STR_NULL                    0x00000001u
#define STR_ALLOC                   0x00000002u
#define TURN_OFF_WARNING            0x00000004u

#define PURPLE                      "\033[1;95m"
#define RED                         "\033[1;91m"
#define WHITE                       "\033[1;97m"
#define RESET                       "\033[0m"

typedef struct string
{
    int size;
    char *data;
} string_t;

typedef struct string_array
{
    int size;
    string_t **data_set;
} string_array_t;

static bool check_warnings(string_t *str, u_int16_t warning_code, const char *function_name)
{
    if (warning_code & STR_NULL)
    {
        if (str == NULL)
        {
            if (!(warning_code & TURN_OFF_WARNING))
            {
                printf("%s: %swarning:%s string is NULL%s\n", function_name, PURPLE, WHITE, RESET);
            }

            return true;
        }
    }

    if (warning_code & STR_ALLOC)
    {
        if (str != NULL)
        {
            if (!(warning_code & TURN_OFF_WARNING))
            {
                printf("%s: %serror:%s string is not NULL%s\n", function_name, RED, WHITE, RESET);
                exit(1);
            }
        }
    }

    return false;
}

static bool check_ranges(int *start, int *end, int size, const char *function_name)
{
    if (*start >= size || *end >= size)
    {
        printf("%s: %swarning:%s indices are out of range%s\n", function_name, PURPLE, WHITE, RESET);
        return true;
    }

    if (*start < 0)
    {
        *start += size;
    }

    if (*end == 0)
    {
        *end = size;
    }
    else if (*end < 0)
    {
        *end += size;
    }

    if (*start < 0 || *end > size || (*end - *start) <= 0)
    {
        printf("%s: %swarning:%s indices are out of range%s\n", function_name, PURPLE, WHITE, RESET);
        return true;
    }

    return false;
}

static int ceil_int(int x, int y) 
{
    return (int)ceil(x / (double)y);
}

static bool check_str_occurrences(char *characters, char value)
{
    if (value == '\0')
    {
        return false;
    }

    while (*characters != '\0')
    {
        if (*characters == value)
        {
            return true;
        }

        characters++;
    }

    return false;
}

static void str_array_null_init(string_array_t *str_array)
{
    for (int i = 0; i < str_array->size; i++)
    {
        str_array->data_set[i] = NULL;
    }
}

static int count_occurrences_in_str(string_t *str, const char *value, int count)
{
    if (*value == '\0' || count < 0)
    {
        return -1;
    }

    int occurrences = 0;
    const char *str_data = str->data;

    while (*str_data != '\0')
    {
        if (strstr(str_data, value) == str_data)
        {
            occurrences++;
        }

        str_data++;
    }

    if (occurrences == 0)
    {
        return -1;
    }
    else if (count > 0 && count < occurrences)
    {
        occurrences = count;
    }

    return occurrences;
}

void print_str(string_t *str, const char *beginning, const char *end)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    printf("%s%s%s", beginning, str->data, end);
}

void print_str_array(string_array_t *str_array, const char *beginning, const char *end)
{
    int i = 0;
    printf("%s{", beginning);

    for (i = 0; i < (str_array->size-1); i++)
    {
        printf("\"%s\", ", str_array->data_set[i]->data);
    }

    printf("\"%s\"}%s", str_array->data_set[i]->data, end);
}

int get_str_size(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1;
    }

    return str->size;
}

char *get_char_str(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return NULL;
    }

    return str->data;
}

int get_str_array_size(string_array_t *str_array)
{
    if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return -1;
    }

    return str_array->size;
}

string_t *get_str_array_index(string_array_t *str_array, int index)
{
    if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return NULL;
    }

    if (index < 0)
    {
        index += str_array->size;
    }

    return str_array->data_set[index];
}

void init_str(string_t **str, const char *str_literal)
{
    if (check_warnings(*str, STR_ALLOC, __func__))
    {
        return;
    }

    *str = new_mem(sizeof(string_t));
    (*str)->size = (int)strlen(str_literal);

    (*str)->data = new_mem(sizeof(char) * (size_t)((*str)->size + 1));
    memcpy((*str)->data, str_literal, (*str)->size);
    (*str)->data[(*str)->size] = '\0';
}

void append_str(string_t *str, const char *str_value)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int size = (int)strlen(str_value);
    str->size += size;

    mem_usage.allocated += (u_int32_t)size;

    str->data = realloc(str->data, sizeof(char) * (size_t)(str->size + 1));
    strcat(str->data, str_value);
}

void sub_str(string_t **str_dest, string_t *str_src, int start, int end, int step)
{
    if (check_warnings(*str_dest, STR_ALLOC, __func__)
        || check_warnings(str_src, STR_NULL, __func__)
        || check_ranges(&start, &end, str_src->size, __func__))
    {
        return;
    }
    else if (step <= 0)
    {
        printf("%s: %swarning:%sslice step cannot be less than or equal to zero%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    *str_dest = new_mem(sizeof(string_t));

    int total_size = end - start;
    total_size = step >= total_size ? 1 : ceil_int(total_size, step);

    (*str_dest)->data = new_mem(sizeof(char) * (size_t)(total_size+1));
    (*str_dest)->size = total_size;

    int step_counter = 0;
    step--;

    for (int i = 0; i < total_size; i++)
    {
        (*str_dest)->data[i] = str_src->data[i + start + step_counter];
        step_counter += step;
    }

    (*str_dest)->data[total_size] = '\0';
}

void copy_str(string_t **str_dest, string_t *str_src)
{
    if (check_warnings(*str_dest, STR_ALLOC, __func__)
        || (check_warnings(str_src, STR_NULL, __func__)))
    {
        return;
    }

    sub_str(str_dest, str_src, 0, 0, 1);
}

void replace_str(string_t *str, const char *old_str, const char *new_str, int count)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int num_of_occurrences = count_occurrences_in_str(str, old_str, count);

    if (num_of_occurrences == -1)
    {
        printf("%s: %swarning:%s could not find substring%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    int i = 0;
    int num_of_replacements = 0;
    int new_str_size = (int)strlen(new_str);
    int old_str_size = (int)strlen(old_str);
    int total_size = ((str->size - (old_str_size * num_of_occurrences)) + (new_str_size * num_of_occurrences));

    const char *copy_str_data = str->data;
    char *replace_str = new_mem(sizeof(char) * (size_t)(total_size + 1));

    while (*copy_str_data != '\0')
    {
        if (num_of_replacements < num_of_occurrences && strstr(copy_str_data, old_str) == copy_str_data)
        {
            memcpy(&replace_str[i], new_str, new_str_size);
            num_of_replacements++;
            i += new_str_size;
            copy_str_data += old_str_size;
        }
        else
        {
            replace_str[i++] = *copy_str_data++;
        }
    }

    replace_str[i] = '\0';

    free_mem(str->data, sizeof(char) * (size_t)(str->size + 1));

    str->size = total_size;
    str->data = replace_str;
}

void erase_str(string_t *str, const char *value)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    replace_str(str, value, "", 0);
}

void erase_str_count(string_t *str, const char *value, int count)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    replace_str(str, value, "", count);
}

void erase_str_index(string_t *str, int start, int end)
{
    if (check_warnings(str, STR_NULL, __func__)
        || check_ranges(&start, &end, str->size, __func__))
    {
        return;
    }

    int j = 0;
    int new_str_size = str->size - (end - start);
    char *new_str = new_mem(sizeof(char) * (size_t)(new_str_size + 1));

    for (int i = 0; i < str->size; i++)
    {
        if (!(i >= start && i < end))
        {
            new_str[j] = str->data[i];
            j++;
        }
    }

    new_str[new_str_size] = '\0';

    free_mem(str->data, sizeof(char) * (size_t)(str->size + 1));

    str->data = new_str;
    str->size = new_str_size;
}

int find_str(string_t *str, const char *value)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1;
    }

    int count = 0;
    char *str_data = str->data;

    while (*str->data != '\0')
    {
        if (strstr(str_data, value) == str_data)
        {
            return count;
        }

        count++;
        str_data++;
    }

    return -1;
}

int count_str(string_t *str, const char *value, int start, int end)
{
    if (check_warnings(str, STR_NULL, __func__) || *value == '\0' 
        || check_ranges(&start, &end, str->size, __func__))
    {
        return -1;
    }

    int i = start;
    int count = 0;
    const char *str_data = str->data + start;

    while (i < end)
    {
        if (strstr(str_data, value) == str_data)
        {
            count++;
        }

        i++;
        str_data++;
    }

    return count;
}

void lstrip_str(string_t *str, char *characters)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int new_str_size = str->size;
    char *str_data = str->data;

    while (check_str_occurrences(characters, *str_data))
    {
        new_str_size--;
        str_data++;
    }

    if (*str_data != '\0')
    {
        char *new_str = new_mem(sizeof(char) * (size_t)(new_str_size + 1));
        memcpy(new_str, str_data, new_str_size);
        new_str[new_str_size] = '\0';

        free_mem(str->data, sizeof(char) * (size_t)(str->size + 1));

        str->data = new_str;
        str->size = new_str_size;
    }
}

void rstrip_str(string_t *str, char *characters)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int new_str_size = str->size;
    const char *forward = str->data + new_str_size;

    while (check_str_occurrences(characters, (*(forward-1))))
    { 
        forward--;
        new_str_size--;
    }

    if (*forward != '\0')
    {
        char *new_str = new_mem(sizeof(char) * (size_t)(new_str_size + 1));
        memcpy(new_str, str->data, new_str_size);
        new_str[new_str_size] = '\0';

        free_mem(str->data, sizeof(char) * (size_t)(str->size + 1));

        str->data = new_str;
        str->size = new_str_size;
    }
}

void strip_str(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    lstrip_str(str, " ");
    rstrip_str(str, " ");
}

void strip_str_chars(string_t *str, char *characters)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    lstrip_str(str, characters);
    rstrip_str(str, characters);
}

void split_str(string_array_t **str_array, string_t *str, const char *separator, int max_split)
{
    if (*str_array != NULL)
    {
        printf("%s: %serror:%s string_array is not NULL%s\n", __func__, RED, WHITE, RESET);
        exit(1);
    }

    int num_of_occurrences = count_occurrences_in_str(str, separator, max_split);
    *str_array = new_mem(sizeof(string_array_t));

    if (num_of_occurrences == -1)
    {
        (*str_array)->size = 1;
        (*str_array)->data_set = new_mem((size_t)(*str_array)->size * sizeof(string_t*));

        str_array_null_init(*str_array);
        init_str(&(*str_array)->data_set[0], str->data);
    
        return;
    }

    int i = 0;
    int total_len = 0;
    const char *split_str = "";
    char *str_value = strdup(str->data);

    (*str_array)->size = num_of_occurrences + 1;
    (*str_array)->data_set = new_mem((size_t)(*str_array)->size * sizeof(string_t*));
    str_array_null_init(*str_array);
    split_str = strsep(&str_value, separator);

    while (split_str != NULL && i < num_of_occurrences)
    {
        init_str(&(*str_array)->data_set[i], split_str);
        split_str = strsep(&str_value, separator);
        total_len += (*str_array)->data_set[i]->size;
        i++;
    }

    init_str(&(*str_array)->data_set[i], str->data + total_len + (num_of_occurrences * (int)strlen(separator)));
}

void upper_str(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int i = 0;

    while (str->data[i] != '\0')
    {
        str->data[i] = (char)toupper(str->data[i]);
        i++;
    }
}

void lower_str(string_t *str)
{  
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int i = 0;

    while (str->data[i] != '\0')
    {
        str->data[i] = (char)tolower(str->data[i]);
        i++;
    }
}

void swapcase_str(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int i = 0;

    while (str->data[i] != '\0')
    {
        if (isupper(str->data[i]))
        {
            str->data[i] = (char)tolower(str->data[i]);
        }
        else
        {
            str->data[i] = (char)toupper(str->data[i]);
        }

        i++;
    }
}

void capitalize_str(string_t *str)
{  
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str->data[0] = (char)toupper(str->data[0]);
}

void title_str(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    capitalize_str(str);

    for (int i = (str->size-1); i > 0; i--)
    {
        if (!(isalpha(str->data[i-1])))
        {
            str->data[i] = (char)toupper(str->data[i]);
        }
        else
        {
            str->data[i] = str->data[i];
        }
    }
}

int int_str(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1;
    }

    return atoi(str->data);
}

double double_str(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1.0;
    }

    return atof(str->data);
}

void free_str(string_t **str)
{
    if (check_warnings(*str, STR_NULL, __func__))
    {
        return;
    }

    free_mem((*str)->data, sizeof(char) * (size_t)((*str)->size + 1));
    free_mem(*str, sizeof(string_t));
    *str = NULL;
}

void free_str_array(string_array_t **str_array)
{
    if (*str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
    }

    for (int i = 0; i < (*str_array)->size; i++)
    {
        free_str(&(*str_array)->data_set[i]);
    }

    free_mem((*str_array)->data_set, (size_t)(*str_array)->size * sizeof(string_t*));
    free_mem(*str_array, sizeof(string_t));
    *str_array = NULL;
}
