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

static char *strsep_m(char **value, char *separator) 
{
    if (*value == NULL)
    {
        return *value;
    }

    char *end = strstr(*value, separator);
    char *temp = *value;

    if (end == NULL)
    {
        *value = NULL;
        return temp;
    }

    *end = '\0';
    *value = end + strlen(separator);

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

static bool check_str_occurrences(const char *characters, char value)
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

static int count_occurrences_in_str(const char *curr, const char *search_val, int count, int start, int end)
{
    if (*search_val == '\0' || count < 0)
    {
        return 0;
    }

    int i = 0;
    int occurrences = 0;
    int search_val_size = (int)strlen(search_val);
    curr = strstr(curr + start, search_val);

    while (curr != NULL && i < end)
    {
        curr += search_val_size;
        curr = strstr(curr, search_val);
        occurrences++;
        i++;
    }

    if (count > 0 && count < occurrences)
    {
        occurrences = count;
    }

    return occurrences;
}

static void read_content(FILE *fp, string_t **str)
{
    *str = alloc_str("");
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

static string_array_t *split_str(const char *curr, int size, char *separator, int max_split)
{
    int i = 0;
    char *split = NULL;
    char *curr_alloc = strdup(curr);
    char *copy_curr = curr_alloc;
    int num_of_occurrences = count_occurrences_in_str(copy_curr, separator, max_split, 0, size);

    mem_usage.allocated += sizeof(char) * (size_t)(size + 1);

    string_array_t *str_array = alloc_mem(sizeof(string_array_t));
    str_array->size = num_of_occurrences + 1;
    str_array->data_set = alloc_mem((size_t)str_array->size * sizeof(string_t*));

    while (i < num_of_occurrences)
    {
        split = strsep_m(&copy_curr, separator);
        str_array->data_set[i] = alloc_str(split);
        i++;
    }

    str_array->data_set[i] = alloc_str(copy_curr);
    free_mem(curr_alloc, sizeof(char) * (size_t)(size + 1));

    return str_array;
}

static void free_str_data(string_t *str)
{
    free_mem(str->data, sizeof(char) * (size_t)(str->size + 1));
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

string_t *alloc_read_file_str(const char *path)
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

void write_file_str(string_t *str, const char *path, bool append_file)
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

string_t *alloc_sys_output_str(const char *cmd)
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

char *alloc_copy_c_str(const char *c_str)
{
    mem_usage.allocated += sizeof(char) * (size_t)(strlen(c_str) + 1);
    return strdup(c_str);
}

string_t *alloc_copy_str(string_t *str)
{
    return alloc_str(str->data);
}

int get_str_size(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1;
    }

    return str->size;
}

char *get_str_c_str(string_t *str)
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

bool sa_cmp_c_str(const char *char_str, string_array_t *str_array, int index)
{
    if (check_index(&index, str_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(char_str, (str_array->data_set[index]->data)));
}

bool sa_cmp_str(string_t *str, string_array_t *str_array, int index)
{
    if (check_index(&index, str_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(str->data, (str_array->data_set[index]->data)));
}

string_t *alloc_str(const char *value)
{
    string_t *str = NULL;
    str = alloc_mem(sizeof(string_t));
    str->size = (int)strlen(value);
    str->data = strdup(value);

    mem_usage.allocated += sizeof(char) * (size_t)(str->size + 1);

    return str;
}

string_t *alloc_va_str(int size, ...)
{
    if (size < 1)
    {
        return NULL;
    }

    va_list args;
    va_start(args, size);

    string_t *str = alloc_str(va_arg(args, char*));

    for (int i = 1; i < size; i++)
    {
        append_str(str, va_arg(args, char*));
    }

    va_end(args);

    return str;
}

void append_str(string_t *str, const char *value)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int size = (int)strlen(value);
    str->size += size;

    mem_usage.allocated += (u_int32_t)size;

    str->data = realloc(str->data, sizeof(char) * (size_t)(str->size + 1));
    memcpy(&str->data[str->size-size], value, size+1);
}

void append_va_str(string_t *str, int size, ...)
{
    if (check_warnings(str, STR_NULL, __func__) || size < 1)
    {
        return;
    }

    va_list args;
    va_start(args, size);

    for (int i = 0; i < size; i++)
    {
        append_str(str, va_arg(args, char*));
    }

    va_end(args);
}

string_t *alloc_sub_str(string_t *str, int start, int end, int step)
{
    if (check_warnings(str, STR_NULL, __func__)
        || check_ranges(&start, &end, str->size, __func__))
    {
        return NULL;
    }
    else if (step <= 0)
    {
        printf("%s: %swarning:%sslice step cannot be less than or equal to zero%s\n", __func__, PURPLE, WHITE, RESET);
        return NULL;
    }

    string_t *sub_str = alloc_mem(sizeof(string_t));

    int step_counter = 0;
    int total_size = end - start;
    total_size = step >= total_size ? 1 : ceil_int(total_size, step);

    sub_str->data = new_mem(sizeof(char) * (size_t)(total_size+1));
    sub_str->size = total_size;

    step--;

    for (int i = 0; i < total_size; i++)
    {
        sub_str->data[i] = str->data[i + start + step_counter];
        step_counter += step;
    }

    sub_str->data[sub_str->size] = '\0';

    return sub_str;
}

void replace_str(string_t *str, const char *old, const char *new)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    replace_str_count(str, old, new, 0);
}

void replace_str_count(string_t *str, const char *old, const char *new, int count)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int num_of_occurrences = count_occurrences_in_str(str->data, old, count, 0, str->size);

    if (num_of_occurrences == 0)
    {
        printf("%s: %swarning:%s could not find substring%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    int i = 0;
    int num_of_replacements = 0;
    int old_size = (int)strlen(old);
    int new_size = (int)strlen(new);
    int total_size = ((str->size - (old_size  * num_of_occurrences)) + (new_size * num_of_occurrences));

    char *copy = str->data;
    char *replacement = alloc_mem(sizeof(char) * (size_t)(total_size + 1));

    while (*copy != '\0')
    {
        if (num_of_replacements < num_of_occurrences && strstr(copy, old) == copy)
        {
            memcpy(&replacement[i], new, new_size);
            num_of_replacements++;
            i += new_size;
            copy += old_size;
        }
        else
        {
            replacement[i++] = *copy++;
        }
    }

    replacement[i] = '\0';

    free_str_data(str);

    str->size = total_size;
    str->data = replacement;
}

void erase_str(string_t *str, const char *value)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    replace_str_count(str, value, "", 0);
}

void erase_str_count(string_t *str, const char *value, int count)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    replace_str_count(str, value, "", count);
}

void erase_str_index(string_t *str, int start, int end)
{
    if (check_warnings(str, STR_NULL, __func__)
        || check_ranges(&start, &end, str->size, __func__))
    {
        return;
    }

    int j = 0;
    int conjoin_size = str->size - (end - start);
    char *conjoin = alloc_mem(sizeof(char) * (size_t)(conjoin_size + 1));

    for (int i = 0; i < str->size; i++)
    {
        if (!(i >= start && i < end))
        {
            conjoin[j] = str->data[i];
            j++;
        }
    }

    conjoin[conjoin_size] = '\0';

    free_str_data(str);

    str->data = conjoin;
    str->size = conjoin_size;
}

int find_str(string_t *str, const char *search_val)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1;
    }

    char *found = strstr(str->data, search_val);

    return found ? (int)(found - str->data) : -1;
}

int count_str(string_t *str, const char *search_val, int start, int end)
{
    if (check_warnings(str, STR_NULL, __func__) || *search_val == '\0' 
        || check_ranges(&start, &end, str->size, __func__))
    {
        return 0;
    }

    return count_occurrences_in_str(str->data, search_val, 0, start, end);
}

void lstrip_str(string_t *str, const char *characters)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int striped_size = str->size;
    char *copy = str->data;

    while (check_str_occurrences(characters, *copy))
    {
        striped_size--;
        copy++;
    }

    if (*copy != '\0')
    {
        char *striped = alloc_mem(sizeof(char) * (size_t)(striped_size + 1));
        memcpy(striped, copy, striped_size+1);

        free_str_data(str);

        str->data = striped;
        str->size = striped_size;
    }
}

void rstrip_str(string_t *str, const char *characters)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int striped_size = str->size;
    const char *forward = str->data + striped_size;

    while (check_str_occurrences(characters, (*(forward-1))))
    { 
        forward--;
        striped_size--;
    }

    if (*forward != '\0')
    {
        char *striped = alloc_mem(sizeof(char) * (size_t)(striped_size + 1));
        memcpy(striped, str->data, striped_size);
        striped[striped_size] = '\0';

        free_str_data(str);

        str->data = striped;
        str->size = striped_size;
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

string_array_t *alloc_split_str(string_t *str, char *separator, int max_split)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return NULL;
    }

    return split_str(str->data, str->size, separator, max_split);
}

string_array_t *alloc_split_c_str(const char *curr, char *separator, int max_split)
{
    if (curr == NULL)
    {
        return NULL;
    }

    return split_str(curr, (int)strlen(curr), separator, max_split);
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
        str->data[i] = isupper(str->data[i]) ? (char)tolower(str->data[i]) : (char)toupper(str->data[i]);
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

void free_c_str(char **curr)
{
    free_mem(*curr, sizeof(char) * (strlen(*curr)+1));
    *curr = NULL;
}

void free_str(string_t **str)
{
    if (check_warnings(*str, STR_NULL, __func__))
    {
        return;
    }

    free_str_data(*str);
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
