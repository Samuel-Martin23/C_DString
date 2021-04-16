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
    *str = new_str("");
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

string_t *new_read_file_str(char *path)
{
    if (*path == '\0')
    {
        return NULL;
    }

    string_t *str = NULL;

    FILE *fp = fopen(path, "r");
    read_content(fp, &str);
    fclose(fp);

    return str;
}

void write_file_str(string_t *str, char *path, bool append_file)
{
    if (check_warnings(str, STR_NULL, __func__) || *path == '\0')
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

string_t *new_sys_output_str(char *cmd)
{
    if (*cmd == '\0')
    {
        return NULL;
    }

    string_t *str = NULL;

    FILE *fp = popen(cmd, "r");
    read_content(fp, &str);
    fclose(fp);

    return str;
}

char *new_copy_charp(char *charp)
{
    mem_usage.allocated += sizeof(char) * (size_t)(strlen(charp) + 1);
    return strdup(charp);
}

string_t *new_copy_str(string_t *str)
{
    return new_str(str->data);
}

int get_str_size(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1;
    }

    return str->size;
}

char *get_str_charp(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return NULL;
    }

    return str->data;
}

int get_sa_size(string_array_t *str_array)
{
    if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return -1;
    }

    return str_array->size;
}

string_t *get_sa_index(string_array_t *str_array, int index)
{
    if (check_index(&index, str_array->size, __func__))
    {
        return NULL;
    }
    else if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return NULL;
    }

    return str_array->data_set[index];
}

bool sa_cmp_charp(char *char_str, string_array_t *str_array, int index)
{
    if (check_index(&index, str_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(char_str, (str_array->data_set[index]->data)));
}

bool sa_cmp(string_t *str, string_array_t *str_array, int index)
{
    if (check_index(&index, str_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(str->data, (str_array->data_set[index]->data)));
}

string_t *new_str(const char *str_literal)
{
    string_t *str = NULL;
    str = new_mem(sizeof(string_t));
    str->size = (int)strlen(str_literal);
    str->data = strdup(str_literal);

    mem_usage.allocated += sizeof(char) * (size_t)(str->size + 1);

    return str;
}

string_t *new_va_str(int size, ...)
{
    if (size < 1)
    {
        return NULL;
    }

    va_list args;
    va_start(args, size);

    string_t *str = new_str(va_arg(args, char*));

    for (int i = 1; i < size; i++)
    {
        append_str(str, va_arg(args, char*));
    }

    va_end(args);

    return str;
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

string_t *new_sub_str(string_t *str_src, int start, int end, int step)
{
    if (check_warnings(str_src, STR_NULL, __func__)
        || check_ranges(&start, &end, str_src->size, __func__))
    {
        return NULL;
    }
    else if (step <= 0)
    {
        printf("%s: %swarning:%sslice step cannot be less than or equal to zero%s\n", __func__, PURPLE, WHITE, RESET);
        return NULL;
    }

    string_t *str_dest = new_mem(sizeof(string_t));

    int step_counter = 0;
    int total_size = end - start;
    total_size = step >= total_size ? 1 : ceil_int(total_size, step);

    str_dest->data = new_mem(sizeof(char) * (size_t)(total_size+1));
    str_dest->size = total_size;

    step--;

    for (int i = 0; i < total_size; i++)
    {
        str_dest->data[i] = str_src->data[i + start + step_counter];
        step_counter += step;
    }

    str_dest->data[str_dest->size] = '\0';

    return str_dest;
}

void replace_str(string_t *str, const char *prev_charp, const char *latest_charp, int count)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int num_of_occurrences = count_occurrences_in_str(str->data, prev_charp, count, 0, str->size);

    if (num_of_occurrences == 0)
    {
        printf("%s: %swarning:%s could not find substring%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    int i = 0;
    int num_of_replacements = 0;
    int prev_charp_size = (int)strlen(prev_charp);
    int latest_charp_size = (int)strlen(latest_charp);
    int total_size = ((str->size - (prev_charp_size  * num_of_occurrences)) + (latest_charp_size * num_of_occurrences));

    const char *copy_str_data = str->data;
    char *replace_str = new_mem(sizeof(char) * (size_t)(total_size + 1));

    while (*copy_str_data != '\0')
    {
        if (num_of_replacements < num_of_occurrences && strstr(copy_str_data, prev_charp) == copy_str_data)
        {
            memcpy(&replace_str[i], latest_charp, latest_charp_size);
            num_of_replacements++;
            i += latest_charp_size;
            copy_str_data += prev_charp_size;
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
    int alloc_charp_size = str->size - (end - start);
    char *alloc_charp = new_mem(sizeof(char) * (size_t)(alloc_charp_size + 1));

    for (int i = 0; i < str->size; i++)
    {
        if (!(i >= start && i < end))
        {
            alloc_charp[j] = str->data[i];
            j++;
        }
    }

    alloc_charp[alloc_charp_size] = '\0';

    free_mem(str->data, sizeof(char) * (size_t)(str->size + 1));

    str->data = alloc_charp;
    str->size = alloc_charp_size;
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

    int alloc_charp_size = str->size;
    char *str_data = str->data;

    while (check_str_occurrences(characters, *str_data))
    {
        alloc_charp_size--;
        str_data++;
    }

    if (*str_data != '\0')
    {
        char *alloc_charp = new_mem(sizeof(char) * (size_t)(alloc_charp_size + 1));
        memcpy(alloc_charp, str_data, alloc_charp_size+1);

        free_mem(str->data, sizeof(char) * (size_t)(str->size + 1));

        str->data = alloc_charp;
        str->size = alloc_charp_size;
    }
}

void rstrip_str(string_t *str, char *characters)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int alloc_charp_size = str->size;
    const char *forward = str->data + alloc_charp_size;

    while (check_str_occurrences(characters, (*(forward-1))))
    { 
        forward--;
        alloc_charp_size--;
    }

    if (*forward != '\0')
    {
        char *alloc_charp = new_mem(sizeof(char) * (size_t)(alloc_charp_size + 1));
        memcpy(alloc_charp, str->data, alloc_charp_size);
        alloc_charp[alloc_charp_size] = '\0';

        free_mem(str->data, sizeof(char) * (size_t)(str->size + 1));

        str->data = alloc_charp;
        str->size = alloc_charp_size;
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

string_array_t *new_split_str(string_t *str, char *separator, int max_split)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return NULL;
    }

    int i = 0;
    char *split_str = "";
    char *alloc_charp = strdup(str->data);
    char *str_data = alloc_charp;
    int num_of_occurrences = count_occurrences_in_str(str->data, separator, max_split, 0, str->size);

    mem_usage.allocated += sizeof(char) * (size_t)(str->size + 1);

    string_array_t *str_array = new_mem(sizeof(string_array_t));
    str_array->size = num_of_occurrences + 1;
    str_array->data_set = new_mem((size_t)str_array->size * sizeof(string_t*));
    str_array_null_init(str_array);

    while (i < num_of_occurrences)
    {
        split_str = strsep_m(&str_data, separator);
        str_array->data_set[i] = new_str(split_str);
        i++;
    }

    str_array->data_set[i] = new_str(str_data);
    free_mem(alloc_charp, sizeof(char) * (size_t)(str->size + 1));

    return str_array;
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

void free_charp(char **charp)
{
    free_mem(*charp, sizeof(char) * (strlen(*charp)+1));
    *charp = NULL;
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
