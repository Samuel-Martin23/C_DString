#include "dstring.h"

#define STR_NULL                    0x00000001u
#define STR_ALLOC                   0x00000002u
#define TURN_OFF_WARNING            0x00000004u

#define PURPLE                      "\033[1;95m"
#define RED                         "\033[1;91m"
#define WHITE                       "\033[1;97m"
#define RESET                       "\033[0m"

#define MAX_CHARS                   10000
#define DEFAULT_CAPACITY            16

typedef struct string
{
    int size;
    int capacity;
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

static char *strsep_m(char **data, char *separator) 
{
    if (*data == NULL)
    {
        return *data;
    }

    char *end = strstr(*data, separator);
    char *temp = *data;

    if (end == NULL)
    {
        *data = NULL;
        return temp;
    }

    *end = '\0';
    *data = end + strlen(separator);

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

static int get_start_index(int *start_opt, int size, bool is_step_neg)
{
    if (start_opt == NULL)
    {
        return is_step_neg ? (size - 1) : 0;
    }

    int start = *start_opt;

    if (start < 0)
    {
        start += size;
    }

    return start;
}

static int get_end_index(int *end_opt, int size, bool is_step_neg)
{
    if (end_opt == NULL)
    {
        return is_step_neg ? -1 : size;
    }

    int end = *end_opt;

    if (end < 0)
    {
        end += size;
    }
    else if (end > size)
    {
        end = size;
    }

    return end;
}

static int get_sub_size(int start, int end, bool is_step_neg)
{
    if (start < 0)
    {
        return 0;
    }

    int sub_str_size = is_step_neg ? (start - end) : (end - start);

    if (sub_str_size <= 0)
    {
        return 0;
    }

    return sub_str_size;
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

    int occurrences = 0;
    int search_val_size = (int)strlen(search_val);
    curr = strstr(curr + start, search_val);

    while (curr != NULL && occurrences < end)
    {
        curr += search_val_size;
        curr = strstr(curr, search_val);
        occurrences++;
    }

    if (count > 0 && count < occurrences)
    {
        occurrences = count;
    }

    return occurrences;
}

static void read_content(FILE *fp, string_t **str)
{
    *str = str_alloc("");
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
            str_append(*str, source_file);
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
        str_array->data_set[i] = str_alloc(split);
        i++;
    }

    str_array->data_set[i] = str_alloc(copy_curr);
    free_mem(curr_alloc, sizeof(char) * (size_t)(size + 1));

    return str_array;
}

static void update_capacity(string_t *str)
{
    while (str->size > str->capacity)
    {
        str->capacity *= 2;
    }
}

static int calculate_capacity(int size)
{
    int capacity = DEFAULT_CAPACITY;

    while (size > capacity)
    {
        capacity *= 2;
    }

    return capacity;
}

int str_get_size(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1;
    }

    return str->size;
}

int str_get_capacity(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1;
    }

    return str->capacity;
}

char *str_get_literal(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return NULL;
    }

    return str->data;
}

void str_set_size(string_t *str, int size)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str->size = size;
}

void str_set_capacity(string_t *str, int capacity)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str->capacity = capacity;
}

void str_set_literal(string_t *str, char *data)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str->data = data;
}

int sa_get_size(string_array_t *str_array)
{
    if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return -1;
    }

    return str_array->size;
}

string_t *sa_get_index(string_array_t *str_array, int index)
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

void sa_set_size(string_array_t *str_array, int size)
{
    if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    str_array->size = size;
}

void sa_set_index(string_array_t *str_array, int index, string_t *str)
{
    if (check_index(&index, str_array->size, __func__))
    {
        return;
    }
    else if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    str_array->data_set[index] = str;
}

string_t *str_alloc(const char *data)
{
    string_t *str = alloc_mem(sizeof(string_t));
    str->size = (int)strlen(data);
    str->capacity = DEFAULT_CAPACITY;

    update_capacity(str);

    str->data = alloc_mem(sizeof(char) * (size_t)(str->capacity + 1));
    memcpy(str->data, data, str->size+1);

    return str;
}

string_t *str_alloc_va(int size, ...)
{
    if (size < 1)
    {
        return NULL;
    }

    va_list args;
    va_start(args, size);

    string_t *str = str_alloc(va_arg(args, char*));

    for (int i = 1; i < size; i++)
    {
        str_append(str, va_arg(args, char*));
    }

    va_end(args);

    return str;
}

void str_append(string_t *str, const char *data)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    int old_capacity = 0;
    int size = (int)strlen(data);
    str->size += size;

    if (str->size > str->capacity)
    {
        old_capacity = str->capacity;

        update_capacity(str);
        str->data = realloc(str->data, sizeof(char) * (size_t)(str->capacity + 1));
        mem_usage.allocated += (u_int32_t)(sizeof(char) * (size_t)(str->capacity - old_capacity));
    }

    memcpy(&str->data[str->size-size], data, size+1);
}

void str_append_va(string_t *str, int size, ...)
{
    if (check_warnings(str, STR_NULL, __func__) || size < 1)
    {
        return;
    }

    va_list args;
    va_start(args, size);

    for (int i = 0; i < size; i++)
    {
        str_append(str, va_arg(args, char*));
    }

    va_end(args);
}

string_t *str_alloc_substr(string_t *str, int *start_opt, int *end_opt, int *step_opt)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return NULL;
    }

    int step = 0;

    if (step_opt == NULL)
    {
        step = 1;
    }
    else if (*step_opt == 0)
    {
        printf("%s: %swarning:%s slice step cannot be equal to zero%s\n", __func__, PURPLE, WHITE, RESET);
        return NULL;
    }
    else
    {
        step = *step_opt;
    }

    bool is_step_neg = (step < 0);
    int start = get_start_index(start_opt, str->size, is_step_neg);
    int end = get_end_index(end_opt, str->size, is_step_neg);
    int size = get_sub_size(start, end, is_step_neg);
    string_t *sub_str = alloc_mem(sizeof(string_t));

    if (size == 0)
    {
        sub_str->size = 0;
        sub_str->capacity = DEFAULT_CAPACITY;
        sub_str->data = alloc_mem(sizeof(char) * 1ul);
        sub_str->data[0] = '\0';
        return sub_str;
    }

    int abs_step = abs(step);

    size = abs_step >= size ? 1 : ceil_int(size, abs_step);
    sub_str->size = size;
    sub_str->capacity = DEFAULT_CAPACITY;

    update_capacity(sub_str);

    sub_str->data = alloc_mem(sizeof(char) * (size_t)(sub_str->capacity + 1));

    for (int i = 0; i < sub_str->size; i++)
    {
        sub_str->data[i] = str->data[start];
        start += step;
    }

    sub_str->data[sub_str->size] = '\0';

    return sub_str;
}

void str_replace(string_t *str, const char *old, const char *new)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str_replace_count(str, old, new, 0);
}

void str_replace_count(string_t *str, const char *old, const char *new, int count)
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

    int capacity = calculate_capacity(total_size);

    char *copy = str->data;
    char *replacement = alloc_mem(sizeof(char) * (size_t)(capacity + 1));

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

    str_data_free(str);

    str->size = total_size;
    str->capacity = capacity;
    str->data = replacement;
}

void str_erase(string_t *str, const char *data)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str_replace_count(str, data, "", 0);
}

void str_erase_count(string_t *str, const char *data, int count)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str_replace_count(str, data, "", count);
}

void str_erase_index(string_t *str, int start, int end)
{
    if (check_warnings(str, STR_NULL, __func__)
        || check_ranges(&start, &end, str->size, __func__))
    {
        return;
    }

    int j = 0;
    int conjoin_size = str->size - (end - start);
    int capacity = calculate_capacity(conjoin_size);
    char *conjoin = alloc_mem(sizeof(char) * (size_t)(capacity + 1));

    for (int i = 0; i < str->size; i++)
    {
        if (!(i >= start && i < end))
        {
            conjoin[j] = str->data[i];
            j++;
        }
    }

    conjoin[conjoin_size] = '\0';

    str_data_free(str);

    str->data = conjoin;
    str->size = conjoin_size;
    str->capacity = capacity;
}

int str_find(string_t *str, const char *search_val)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1;
    }

    char *found = strstr(str->data, search_val);

    return found ? (int)(found - str->data) : -1;
}

int str_count(string_t *str, const char *search_val, int start, int end)
{
    if (check_warnings(str, STR_NULL, __func__) || *search_val == '\0' 
        || check_ranges(&start, &end, str->size, __func__))
    {
        return 0;
    }

    return count_occurrences_in_str(str->data, search_val, 0, start, end);
}

void str_lstrip(string_t *str, const char *characters)
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

    int capacity = calculate_capacity(striped_size);

    if (*copy != '\0')
    {
        char *striped = alloc_mem(sizeof(char) * (size_t)(capacity + 1));
        memcpy(striped, copy, striped_size+1);

        str_data_free(str);

        str->data = striped;
        str->size = striped_size;
        str->capacity = capacity;
    }
}

void str_rstrip(string_t *str, const char *characters)
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

    int capacity = calculate_capacity(striped_size);

    if (*forward != '\0')
    {
        char *striped = alloc_mem(sizeof(char) * (size_t)(capacity + 1));
        memcpy(striped, str->data, striped_size);
        striped[striped_size] = '\0';

        str_data_free(str);

        str->data = striped;
        str->size = striped_size;
        str->capacity = capacity;
    }
}

void str_strip(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str_lstrip(str, "\n ");
    str_rstrip(str, "\n ");
}

void str_strip_chars(string_t *str, char *characters)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str_lstrip(str, characters);
    str_rstrip(str, characters);
}

string_array_t *str_alloc_split(string_t *str, char *separator, int max_split)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return NULL;
    }

    return split_str(str->data, str->size, separator, max_split);
}

string_array_t *str_alloc_split_c_str(const char *curr, char *separator, int max_split)
{
    if (curr == NULL)
    {
        return NULL;
    }

    return split_str(curr, (int)strlen(curr), separator, max_split);
}

bool sa_cmp_str(string_t *str, string_array_t *str_array, int index)
{
    if (check_index(&index, str_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(str->data, (str_array->data_set[index]->data)));
}

bool sa_cmp_c_str(const char *char_str, string_array_t *str_array, int index)
{
    if (check_index(&index, str_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(char_str, (str_array->data_set[index]->data)));
}

void str_upper(string_t *str)
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

void str_lower(string_t *str)
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

void str_swapcase(string_t *str)
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

void str_capitalize(string_t *str)
{  
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str->data[0] = (char)toupper(str->data[0]);
}

void str_title(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    str_capitalize(str);

    for (int i = (str->size-1); i > 0; i--)
    {
        if (!(isalpha(str->data[i-1])))
        {
            str->data[i] = (char)toupper(str->data[i]);
        }
    }
}

string_t *str_alloc_read_file(const char *path)
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

void str_write_file(string_t *str, const char *path, bool append_file)
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

string_t *str_alloc_sys_output(const char *cmd)
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

int str_int(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1;
    }

    return atoi(str->data);
}

double str_double(string_t *str)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return -1.0;
    }

    return atof(str->data);
}

char *c_str_alloc_copy(const char *c_str)
{
    mem_usage.allocated += sizeof(char) * (size_t)(strlen(c_str) + 1);
    return strdup(c_str);
}

string_t *str_alloc_copy(string_t *str)
{
    return str_alloc(str->data);
}

void str_print(string_t *str, const char *beginning, const char *end)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }

    printf("%s%s%s", beginning, str->data, end);
}

void str_array_print(string_array_t *str_array, const char *beginning, const char *end)
{
    if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    int i = 0;
    int last_index = (str_array->size-1);

    printf("%s{", beginning);

    for (i = 0; i < last_index; i++)
    {
        printf("\"%s\", ", str_array->data_set[i]->data);
    }

    printf("\"%s\"}%s", str_array->data_set[i]->data, end);
}

void c_str_free(char **curr)
{
    free_mem(*curr, sizeof(char) * (strlen(*curr)+1));
    *curr = NULL;
}

void str_free(string_t **str)
{
    if (check_warnings(*str, STR_NULL, __func__))
    {
        return;
    }

    str_data_free(*str);
    free_mem(*str, sizeof(string_t));
    *str = NULL;
}

void str_data_free(string_t *str)
{
    free_mem(str->data, sizeof(char) * (size_t)(str->capacity + 1));
}

void str_array_free(string_array_t **str_array)
{
    if (*str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    for (int i = 0; i < (*str_array)->size; i++)
    {
        str_free(&(*str_array)->data_set[i]);
    }

    free_mem((*str_array)->data_set, (size_t)(*str_array)->size * sizeof(string_t*));
    free_mem(*str_array, sizeof(string_array_t));
    *str_array = NULL;
}
