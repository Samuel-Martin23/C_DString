#include "c_string.h"

#define STR_NULL                    0x00000001u
#define STR_ALLOC                   0x00000002u
#define TURN_OFF_WARNING            0x00000004u

#define PURPLE                      "\033[1;95m"
#define RED                         "\033[1;91m"
#define WHITE                       "\033[1;97m"
#define RESET                       "\033[0m"

#define MAX_CHARS                   1000000

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

static char *strsep_m(char **string, char *separator) 
{
    if (*string == NULL)
    {
        return *string;
    }

    char *end = strstr(*string, separator);
    char *temp = *string;

    if (end == NULL)
    {
        *string = NULL;
        return temp;
    }

    *end = '\0';
    *string = end + strlen(separator);

    return temp;
}

static bool check_index(int *index, int size, const char *function_name)
{
    if (*index < 0)
    {
        *index += size;
    }

    if (*index < 0 || *index >= size)
    {
        printf("%s: %swarning:%s index is out of range%s\n", function_name, PURPLE, WHITE, RESET);
        return true;
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

static int count_occurrences_in_str(const char *str_data, const char *value, int count, int start, int end)
{
    if (*value == '\0' || count < 0)
    {
        return 0;
    }

    int i = 0;
    int occurrences = 0;
    int value_len = (int)strlen(value);
    str_data = strstr(str_data + start, value);

    while (str_data != NULL && i < end)
    {
        i++;
        occurrences++;
        str_data += value_len;
        str_data = strstr(str_data, value);
    }

    if (count > 0 && count < occurrences)
    {
        occurrences = count;
    }

    return occurrences;
}

static void read_content(FILE *fp, string_t **str)
{
    init_str(str, "");
    char source_file[MAX_CHARS];

    if (fp == NULL) 
    {
        printf("%s: %swarning:%s failed to read content%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    while (!feof(fp))
    {
        if (fgets(source_file, MAX_CHARS, fp) != NULL)
        {
            append_str(*str, source_file);
        }
    }
}

void read_file_str(string_t **str, char *path)
{
    if (check_warnings(*str, STR_ALLOC, __func__) || *path == '\0')
    {
        return;
    }

    FILE *fp = fopen(path, "r");
    read_content(fp, str);
    fclose(fp);
}

void write_file_str(string_t *str, char *path, bool append_file)
{
    if (check_warnings(str, STR_ALLOC, __func__) || *path == '\0')
    {
        return;
    }

    const char *write_opt = append_file ? "a" : "w";
    FILE *fp = fopen(path, write_opt);

    if (fp == NULL) 
    {
        printf("%s: %swarning:%s failed to write content%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    fprintf(fp, "%s", str->data);
    fclose(fp);
}

void sys_output_str(string_t **str, char *cmd)
{
    if (check_warnings(*str, STR_ALLOC, __func__) || *cmd == '\0')
    {
        return;
    }

    FILE *fp = popen(cmd, "r");
    read_content(fp, str);
    fclose(fp);
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
    if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

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

void get_char_str(char **new_char_str, string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }
    else if (*new_char_str != NULL)
    {
        printf("%s: %swarning:%s char pointer is not NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    mem_usage.allocated += sizeof(char) * (size_t)(str->size + 1);
    *new_char_str = strdup(str->data);
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

void get_str_array_index(string_t **new_str, string_array_t *str_array, int index)
{
    if (check_warnings(*new_str, STR_ALLOC, __func__) || 
        check_index(&index, str_array->size, __func__))
    {
        return;
    }
    else if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    return init_str(new_str, str_array->data_set[index]->data);
}

bool str_array_cmp_char(char *char_str, string_array_t *str_array, int index)
{
    if (check_index(&index, str_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(char_str, (str_array->data_set[index]->data)));
}

bool str_array_cmp_str(string_t *str, string_array_t *str_array, int index)
{
    if (check_index(&index, str_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(str->data, (str_array->data_set[index]->data)));
}

void init_str(string_t **str, const char *str_literal)
{
    if (check_warnings(*str, STR_ALLOC, __func__))
    {
        return;
    }

    *str = new_mem(sizeof(string_t));
    (*str)->size = (int)strlen(str_literal);
    (*str)->data = strdup(str_literal);
    mem_usage.allocated += sizeof(char) * (size_t)((*str)->size + 1);
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
    memcpy(&str->data[str->size-size], str_value, size+1);
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

    int step_counter = 0;
    int total_size = end - start;
    total_size = step >= total_size ? 1 : ceil_int(total_size, step);

    (*str_dest)->data = new_mem(sizeof(char) * (size_t)(total_size+1));
    (*str_dest)->size = total_size;

    step--;

    for (int i = 0; i < total_size; i++)
    {
        (*str_dest)->data[i] = str_src->data[i + start + step_counter];
        step_counter += step;
    }

    (*str_dest)->data[(*str_dest)->size] = '\0';
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

    int num_of_occurrences = count_occurrences_in_str(str->data, old_str, count, 0, str->size);

    if (num_of_occurrences == 0)
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

    char *found_str = strstr(str->data, value);

    return found_str ? (int)(found_str - str->data) : -1;
}

int count_str(string_t *str, const char *value, int start, int end)
{
    if (check_warnings(str, STR_NULL, __func__) || *value == '\0' 
        || check_ranges(&start, &end, str->size, __func__))
    {
        return 0;
    }

    return count_occurrences_in_str(str->data, value, 0, start, end);
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
        memcpy(new_str, str_data, new_str_size+1);

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

    lstrip_str(str, "\n ");
    rstrip_str(str, "\n ");
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

void split_str(string_array_t **str_array, string_t *str, char *separator, int max_split)
{
    if (*str_array != NULL)
    {
        printf("%s: %serror:%s string_array is not NULL%s\n", __func__, RED, WHITE, RESET);
        exit(1);
    }
    else if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int i = 0;
    char *split_str = "";
    char *alloc_str = strdup(str->data);
    char *str_data = alloc_str;
    int num_of_occurrences = count_occurrences_in_str(str->data, separator, max_split, 0, str->size);

    mem_usage.allocated += sizeof(char) * (size_t)(str->size + 1);

    *str_array = new_mem(sizeof(string_array_t));
    (*str_array)->size = num_of_occurrences + 1;
    (*str_array)->data_set = new_mem((size_t)(*str_array)->size * sizeof(string_t*));
    str_array_null_init(*str_array);

    while (i < num_of_occurrences)
    {
        split_str = strsep_m(&str_data, separator);
        init_str(&(*str_array)->data_set[i], split_str);
        i++;
    }

    init_str(&(*str_array)->data_set[i], str_data);
    free_mem(alloc_str, sizeof(char) * (size_t)(str->size + 1));
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
        return;
    }

    for (int i = 0; i < (*str_array)->size; i++)
    {
        free_str(&(*str_array)->data_set[i]);
    }

    free_mem((*str_array)->data_set, (size_t)(*str_array)->size * sizeof(string_t*));
    free_mem(*str_array, sizeof(string_array_t));
    *str_array = NULL;
}
