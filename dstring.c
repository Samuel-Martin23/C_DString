#include "dstring.h"

#define PURPLE                      "\033[1;95m"
#define RED                         "\033[1;91m"
#define WHITE                       "\033[1;97m"
#define RESET                       "\033[0m"

#define DEFAULT_CAPACITY            32

typedef struct string
{
    int64_t size;
    int64_t capacity;
    char *data;
} string_t;

typedef struct string_array
{
    int64_t size;
    string_t **data_set;
} string_array_t;

static bool is_str_null(string_t *str, const char *function_name)
{
    if (str == NULL)
    {
        printf("%s: %swarning:%s string is NULL%s\n", function_name, PURPLE, WHITE, RESET);
        return true;
    }

    return false;
}

static bool is_c_str_null(const char *data, const char *func_name)
{
    bool is_null = (data == NULL);

    if (is_null)
    {
        printf("%s: %swarning:%s string literal input is NULL%s\n", func_name, PURPLE, WHITE, RESET);
    }

    return is_null;
}

static bool is_not_valid_c_str(const char *data, const char *func_name)
{
    bool is_valid_str = ((data != NULL) && (data[0] != '\0'));

    if (!(is_valid_str))
    {
        printf("%s: %swarning:%s string literal input is not a valid string%s\n", func_name, PURPLE, WHITE, RESET);
    }

    return !(is_valid_str);
}

static bool is_str_array_null(string_array_t *str_array, const char *func_name)
{
    bool is_null = (str_array == NULL);

    if (is_null)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", func_name, PURPLE, WHITE, RESET);
    }

    return is_null;
}

static bool is_size_less_or_zero(int64_t size, const char *func_name)
{
    bool is_valid_size = (size <= 0);

    if (is_valid_size)
    {
        printf("%s: %swarning:%s size is less than or equal to zero%s\n", func_name, PURPLE, WHITE, RESET);
    }

    return is_valid_size;
}

static bool is_pointer_null(void *data, const char *func_name)
{
    bool is_not_valid_pointer = (data == NULL);

    if (is_not_valid_pointer)
    {
        printf("%s: %swarning:%s pointer input is NULL%s\n", func_name, PURPLE, WHITE, RESET);
    }

    return is_not_valid_pointer;
}

static bool check_index(int64_t *index, int64_t size, const char *function_name)
{
    if (*index < 0)
    {
        *index += size;
    }

    if (*index < 0 || *index >= size)
    {
        printf("%s: %swarning:%s index %lld is out of range%s\n", function_name, PURPLE, WHITE, *index, RESET);
        return true;
    }

    return false;
}

static bool check_ranges(int64_t *start, int64_t *end, int64_t size, const char *function_name)
{
    if (*start >= size || *end > size)
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


static char *strsep_m(char **data, const char *separator) 
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

static int64_t get_start_index(int64_t *start_opt, int64_t size, bool is_step_neg)
{
    if (start_opt == NULL)
    {
        return is_step_neg ? (size - 1) : 0;
    }

    int64_t start = *start_opt;

    if (start < 0)
    {
        start += size;
    }

    return start;
}

static int64_t get_end_index(int64_t *end_opt, int64_t size, bool is_step_neg)
{
    if (end_opt == NULL)
    {
        return is_step_neg ? -1 : size;
    }

    int64_t end = *end_opt;

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

static int64_t get_sub_size(int64_t start, int64_t end, bool is_step_neg)
{
    if (start < 0)
    {
        return 0;
    }

    int64_t sub_str_size = is_step_neg ? (start - end) : (end - start);

    if (sub_str_size <= 0)
    {
        return 0;
    }

    return sub_str_size;
}

static int64_t ceil_ll(int64_t x, int64_t y) 
{
    return (int64_t)ceil(x / (double)y);
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

static int64_t count_occurrences_in_str(const char *data, const char *search_val, int64_t count, int64_t start, int64_t end)
{
    if (*search_val == '\0' || count < 0)
    {
        return 0;
    }

    int64_t data_size = (int64_t)strlen(data);
    char *data_alloc = strdup(data);
    char *data_copy = data_alloc;

    mem_usage.allocated += sizeof(char) * (size_t)(data_size + 1);

    data_copy = &data_copy[start];
    data_copy[end-start] = '\0';

    int64_t occurrences = 0;
    int64_t search_val_size = (int64_t)strlen(search_val);
    data_copy = strstr(data_copy, search_val);

    while (data_copy != NULL && occurrences < end)
    {
        data_copy += search_val_size;
        data_copy = strstr(data_copy, search_val);
        occurrences++;
    }

    if (count > 0 && count < occurrences)
    {
        occurrences = count;
    }

    free_mem(data_alloc, sizeof(char) * (size_t)(data_size + 1));

    return occurrences;
}

static string_t *alloc_read_file_content(FILE *fp)
{
    char file_line[STR_MAX_CHARS];
    string_t *str = str_alloc("");

    if (fp == NULL) 
    {
        printf("%s: %swarning:%s failed to read content%s\n", __func__, PURPLE, WHITE, RESET);
        return NULL;
    }

    while (!feof(fp))
    {
        if (fgets(file_line, STR_MAX_CHARS, fp) != NULL)
        {
            str_append(str, file_line);
        }
    }

    return str;
}

static string_array_t *alloc_split_str(const char *data, int64_t size, const char *separator, int64_t max_split)
{
    int64_t i = 0;
    char *split = NULL;
    char *data_alloc = strdup(data);
    char *data_copy = data_alloc;
    int64_t num_of_occurrences = count_occurrences_in_str(data_copy, separator, max_split, 0, size);

    mem_usage.allocated += sizeof(char) * (size_t)(size + 1);

    string_array_t *str_array = alloc_mem(sizeof(string_array_t));
    str_array->size = num_of_occurrences + 1;
    str_array->data_set = alloc_mem((size_t)str_array->size * sizeof(string_t*));

    while (i < num_of_occurrences)
    {
        split = strsep_m(&data_copy, separator);
        str_array->data_set[i] = str_alloc(split);
        i++;
    }

    str_array->data_set[i] = str_alloc(data_copy);
    free_mem(data_alloc, sizeof(char) * (size_t)(size + 1));

    return str_array;
}

static int64_t update_capacity(int64_t size, int64_t capacity)
{
    while (size > capacity)
    {
        capacity *= 2;
    }

    return capacity;
}

static int64_t calculate_capacity(int64_t size)
{
    return update_capacity(size, DEFAULT_CAPACITY);
}

static void set_empty_str(string_t *str)
{
    str->size = 0;
    str->capacity = DEFAULT_CAPACITY;
    str->data = alloc_mem(sizeof(char) * (size_t)(str->capacity + 1));
    str->data[0] = '\0';
}

static string_t *alloc_substr(const char *data, int64_t data_size, int64_t *start_opt, int64_t *end_opt, int64_t *step_opt, const char *function_name)
{
    if (is_size_less_or_zero(data_size, __func__))
    {
        return NULL;
    }

    int64_t step = 0;

    if (step_opt == NULL)
    {
        step = 1;
    }
    else if (*step_opt == 0)
    {
        printf("%s: %swarning:%s slice step cannot be equal to zero%s\n", function_name, PURPLE, WHITE, RESET);
        return NULL;
    }
    else
    {
        step = *step_opt;
    }

    bool is_step_neg = (step < 0);
    int64_t start = get_start_index(start_opt, data_size, is_step_neg);
    int64_t end = get_end_index(end_opt, data_size, is_step_neg);
    int64_t size = get_sub_size(start, end, is_step_neg);
    string_t *sub_str = alloc_mem(sizeof(string_t));

    if (size == 0 || start >= data_size)
    {
        set_empty_str(sub_str);
        return sub_str;
    }

    int64_t abs_step = llabs(step);

    sub_str->size = (abs_step >= size) ? 1 : ceil_ll(size, abs_step);
    sub_str->capacity = calculate_capacity(sub_str->size);
    sub_str->data = alloc_mem(sizeof(char) * (size_t)(sub_str->capacity + 1));

    for (int64_t i = 0; i < sub_str->size; i++)
    {
        sub_str->data[i] = data[start];
        start += step;
    }

    sub_str->data[sub_str->size] = '\0';

    return sub_str;
}

static char *get_str_number(int8_t number)
{
    switch (number)
    {
        case 0:
            return "0";
        case 1:
            return "1";
        case 2:
            return "2";
        case 3:
            return "3";
        case 4:
            return "4";
        case 5:
            return "5";
        case 6:
            return "6";
        case 7:
            return "7";
        case 8:
            return "8";
        case 9:
            return "9";
    }

    return "";
}

static int64_t str_realloc_capacity(string_t *str, const char *data)
{
    int64_t old_capacity = 0;
    int64_t data_size = (int64_t)strlen(data);
    str->size += data_size;

    if (str->size > str->capacity)
    {
        old_capacity = str->capacity;
        str->capacity = update_capacity(str->size, str->capacity);
        str->data = realloc(str->data, sizeof(char) * (size_t)(str->capacity + 1));

        mem_usage.allocated += (u_int32_t)(sizeof(char) * (size_t)(str->capacity - old_capacity));
    }

    return data_size;
}

static void str_data_free(string_t *str)
{
    free_mem(str->data, sizeof(char) * (size_t)(str->capacity + 1));
}

static string_t *alloc_setup_capacity(int64_t file_size)
{
    string_t *str = alloc_mem(sizeof(string_t));

    str->size = file_size;
    str->capacity = calculate_capacity(file_size);
    str->data = alloc_mem(sizeof(char) * (size_t)(str->capacity + 1));

    return str;
}

int64_t str_get_size(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return -1;
    }

    return str->size;
}

int64_t str_get_capacity(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return -1;
    }

    return str->capacity;
}

char *str_get_literal(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return NULL;
    }

    return str->data;
}

/*
void str_set_size(string_t *str, int64_t size)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }
    else if (size < 0)
    {
        printf("%s: %swarning:%s size is less than 0%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    str->size = size;
}

void str_set_capacity(string_t *str, int64_t capacity)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }
    else if (capacity < 0)
    {
        printf("%s: %swarning:%s capacity is less than 0%s\n", __func__, PURPLE, WHITE, RESET);
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
    else if (is_not_valid_c_str(data, __func__))
    {
        return;
    }

    str->data = data;
}
*/

string_t *str_alloc(const char *data)
{
    if (is_c_str_null(data, __func__))
    {
        return NULL;
    }

    string_t *str = alloc_mem(sizeof(string_t));

    str->size = (int64_t)strlen(data);
    str->capacity = calculate_capacity(str->size);
    str->data = alloc_mem(sizeof(char) * (size_t)(str->capacity + 1));

    memcpy(str->data, data, str->size + 1);

    return str;
}

string_t *str_alloc_va(int64_t size, ...)
{
    if (is_size_less_or_zero(size, __func__))
    {
        return NULL;
    }

    va_list args;
    va_start(args, size);

    string_t *str = str_alloc(va_arg(args, char*));

    for (int64_t i = 1; i < size; i++)
    {
        str_append(str, va_arg(args, char*));
    }

    va_end(args);

    return str;
}

void str_append(string_t *str, const char *data)
{
    if (is_str_null(str, __func__)
        || is_not_valid_c_str(data, __func__))
    {
        return;
    }

    int64_t data_size = str_realloc_capacity(str, data);

    memcpy(&str->data[str->size - data_size], data, data_size + 1);
}

void str_append_va(string_t *str, int64_t size, ...)
{
    if (is_str_null(str, __func__))
    {
        return;
    }

    if (is_size_less_or_zero(size, __func__))
    {
        return;
    }

    va_list args;
    va_start(args, size);

    for (int64_t i = 0; i < size; i++)
    {
        str_append(str, va_arg(args, char*));
    }

    va_end(args);
}

string_t *str_add(string_t *curr_str, string_t *newest_str)
{
    return str_add_va(2, curr_str, newest_str);
}

string_t *str_add_va(int64_t size, ...)
{
    if (is_size_less_or_zero(size, __func__))
    {
        return NULL;
    }

    va_list args;
    va_start(args, size);

    string_t *temp = NULL;
    string_t *total_str = str_alloc_copy(va_arg(args, string_t*));

    for (int64_t i = 1; i < size; i++)
    {
        temp = va_arg(args, string_t*);

        if (temp == NULL)
        {
            va_end(args);
            return total_str;
        }

        str_append(total_str, temp->data);
    }

    va_end(args);

    return total_str;
}

void str_add_equals(string_t *curr_str, string_t *newest_str)
{
    str_add_equals_va(curr_str, 1, newest_str);
}

void str_add_equals_va(string_t *str, int64_t size, ...)
{
    if (is_str_null(str, __func__))
    {
        return;
    }

    if (is_size_less_or_zero(size, __func__))
    {
        return;
    }

    va_list args;
    va_start(args, size);

    string_t *temp = NULL;

    for (int64_t i = 0; i < size; i++)
    {
        temp = va_arg(args, string_t*);

        if (temp == NULL)
        {
            va_end(args);
            return;
        }

        str_append(str, temp->data);
    }

    va_end(args);
}

void str_before(string_t *str, const char *data)
{
    if (is_str_null(str, __func__)
        || is_not_valid_c_str(data, __func__))
    {
        return;
    }

    int64_t data_size = str_realloc_capacity(str, data);

    memcpy(&str->data[data_size], str->data, str->size+1);
    memcpy(str->data, data, data_size);
}

string_t *str_alloc_substr(string_t *str, int64_t *start_opt, int64_t *end_opt, int64_t *step_opt)
{
    if (is_str_null(str, __func__))
    {
        return NULL;
    }

    return alloc_substr(str->data, str->size, start_opt, end_opt, step_opt, __func__);
}


string_t *str_alloc_c_substr(const char *data, int64_t *start_opt, int64_t *end_opt, int64_t *step_opt)
{
    if (is_not_valid_c_str(data, __func__))
    {
        return NULL;
    }

    return alloc_substr(data, (int64_t)strlen(data), start_opt, end_opt, step_opt, __func__);
}

void str_replace(string_t *str, const char *old_c_str, const char *new_c_str)
{
    if (is_str_null(str, __func__)
        || is_c_str_null(old_c_str, __func__)
        || is_c_str_null(new_c_str, __func__))
    {
        return;
    }

    str_replace_count(str, old_c_str, new_c_str, 0);
}

void str_replace_count(string_t *str, const char *old_c_str, const char *new_c_str, int64_t count)
{
    if (is_str_null(str, __func__)
        || is_c_str_null(old_c_str, __func__)
        || is_c_str_null(new_c_str, __func__))
    {
        return;
    }

    int64_t num_of_occurrences = count_occurrences_in_str(str->data, old_c_str, count, 0, str->size);

    if (num_of_occurrences == 0)
    {
        printf("%s: %swarning:%s could not find substring%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    int64_t i = 0;
    int64_t num_of_replacements = 0;
    int64_t old_c_str_size = (int64_t)strlen(old_c_str);
    int64_t new_c_str_size = (int64_t)strlen(new_c_str);
    int64_t total_size = ((str->size - (old_c_str_size  * num_of_occurrences)) + (new_c_str_size * num_of_occurrences));

    int64_t capacity = calculate_capacity(total_size);

    char *copy = str->data;
    char *replacement = alloc_mem(sizeof(char) * (size_t)(capacity + 1));

    while (*copy != '\0')
    {
        if (num_of_replacements < num_of_occurrences && strstr(copy, old_c_str) == copy)
        {
            memcpy(&replacement[i], new_c_str, new_c_str_size);
            num_of_replacements++;
            i += new_c_str_size;
            copy += old_c_str_size;
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
    if (is_str_null(str, __func__)
        || is_c_str_null(data, __func__))
    {
        return;
    }

    str_replace_count(str, data, "", 0);
}

void str_erase_count(string_t *str, const char *data, int64_t count)
{
    if (is_str_null(str, __func__)
        || is_c_str_null(data, __func__))
    {
        return;
    }

    str_replace_count(str, data, "", count);
}

void str_erase_index(string_t *str, int64_t start, int64_t end)
{
    if (is_str_null(str, __func__)
        || check_ranges(&start, &end, str->size, __func__))
    {
        return;
    }

    int64_t conjoin_data_size = str->size - (end - start);
    int64_t capacity = calculate_capacity(conjoin_data_size);
    char *conjoin_data = alloc_mem(sizeof(char) * (size_t)(capacity + 1));

    memcpy(conjoin_data, str->data, start);
    memcpy(&conjoin_data[start], &str->data[end], (conjoin_data_size - start));
    conjoin_data[conjoin_data_size] = '\0';

    str_data_free(str);

    str->data = conjoin_data;
    str->size = conjoin_data_size;
    str->capacity = capacity;
}

int64_t str_find(string_t *str, const char *search_val)
{
    if (is_str_null(str, __func__)
        || is_not_valid_c_str(search_val, __func__))
    {
        return -1;
    }

    char *found = strstr(str->data, search_val);

    return found ? (int64_t)(found - str->data) : -1;
}

int64_t str_count(string_t *str, const char *search_val, int64_t start, int64_t end)
{
    if (is_str_null(str, __func__)
        || is_not_valid_c_str(search_val, __func__) 
        || check_ranges(&start, &end, str->size, __func__))
    {
        return 0;
    }

    return count_occurrences_in_str(str->data, search_val, 0, start, end);
}

void str_lstrip(string_t *str, const char *characters)
{
    if (is_str_null(str, __func__)
        || is_not_valid_c_str(characters, __func__))
    {
        return;
    }

    char *copy = str->data;

    while (check_str_occurrences(characters, *copy))
    {
        copy++;
    }

    if (copy == str->data)
    {
        return;
    }
    else if (*copy != '\0')
    {
        int64_t striped_size = str->size - (copy - str->data);
        int64_t capacity = calculate_capacity(striped_size);
        char *striped = alloc_mem(sizeof(char) * (size_t)(capacity + 1));

        memcpy(striped, copy, striped_size + 1);
        str_data_free(str);

        str->data = striped;
        str->size = striped_size;
        str->capacity = capacity;
    }
    else
    {
        str_data_free(str);
        set_empty_str(str);
    }
}

void str_rstrip(string_t *str, const char *characters)
{
    if (is_str_null(str, __func__)
        || is_not_valid_c_str(characters, __func__))
    {
        return;
    }

    char *forward = str->data + str->size;

    while (check_str_occurrences(characters, (*(forward-1))))
    {
        forward--;
    }

    if (forward == (str->data + str->size))
    {
        return;
    }
    else if (forward != str->data)
    {
        int64_t striped_size = forward - str->data;
        int64_t capacity = calculate_capacity(striped_size);
        char *striped = alloc_mem(sizeof(char) * (size_t)(capacity + 1));

        memcpy(striped, str->data, striped_size);
        str_data_free(str);
        striped[striped_size] = '\0';

        str->data = striped;
        str->size = striped_size;
        str->capacity = capacity;
    }
    else
    {
        str_data_free(str);
        set_empty_str(str);
    }
}

void str_strip(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return;
    }

    str_lstrip(str, "\n ");
    str_rstrip(str, "\n ");
}

void str_strip_chars(string_t *str, const char *characters)
{
    if (is_str_null(str, __func__)
        || is_not_valid_c_str(characters, __func__))
    {
        return;
    }

    str_lstrip(str, characters);
    str_rstrip(str, characters);
}

void str_upper(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return;
    }

    int64_t i = 0;

    while (str->data[i] != '\0')
    {
        str->data[i] = (char)toupper(str->data[i]);
        i++;
    }
}

void str_lower(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return;
    }

    int64_t i = 0;

    while (str->data[i] != '\0')
    {
        str->data[i] = (char)tolower(str->data[i]);
        i++;
    }
}

void str_swapcase(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return;
    }

    int64_t i = 0;

    while (str->data[i] != '\0')
    {
        str->data[i] = isupper(str->data[i]) ? (char)tolower(str->data[i]) : (char)toupper(str->data[i]);
        i++;
    }
}

void str_capitalize(string_t *str)
{  
    if (is_str_null(str, __func__))
    {
        return;
    }

    str->data[0] = (char)toupper(str->data[0]);
}

void str_title(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return;
    }

    str_capitalize(str);

    for (int64_t i = (str->size-1); i > 0; i--)
    {
        if (!(isalpha(str->data[i-1])))
        {
            str->data[i] = (char)toupper(str->data[i]);
        }
    }
}

string_t *str_alloc_read_keyboard(const char *output_message)
{
    if (is_c_str_null(output_message, __func__))
    {
        return NULL;
    }

    printf("%s", output_message);

    int64_t input_size = 0;
    char input[STR_MAX_CHARS];
    string_t *str = alloc_mem(sizeof(string_t));

    fgets(input, STR_MAX_CHARS, stdin);
    input_size = (int64_t)(strlen(input) - 1);
    input[input_size] = '\0';

    str->size = input_size;
    str->capacity = calculate_capacity(str->size);
    str->data = alloc_mem(sizeof(char) * (size_t)(str->capacity + 1));

    memcpy(str->data, input, str->size + 1);

    return str;
}

string_t *str_alloc_read_file(const char *path, const char *mode)
{
    if (is_not_valid_c_str(path, __func__)
        || is_not_valid_c_str(mode, __func__))
    {
        return NULL;
    }

    if (strstr(mode, "w") != NULL || strstr(mode, "a") != NULL)
    {
        printf("%s: %swarning:%s file can not be written%s\n", __func__, PURPLE, WHITE, RESET);
        return NULL;
    }

    FILE *fp = fopen(path, mode);
    string_t *str = NULL;

    if (strstr(mode, "b"))
    {
        fseek(fp, 0, SEEK_END);
        int64_t file_size = (int64_t)ftell(fp);
        fseek(fp, 0, SEEK_SET);
    
        str = alloc_setup_capacity(file_size);

        fread(str->data, (size_t)str->size + 1, 1, fp);
    }
    else
    {
        str = alloc_read_file_content(fp);
    }

    fclose(fp);

    return str;
}

void str_write_file(string_t *str, const char *path, const char *mode)
{
    if (is_str_null(str, __func__)
        || is_not_valid_c_str(path, __func__)
        || is_not_valid_c_str(mode, __func__))
    {
        return;
    }

    if (strstr(mode, "r") != NULL || strstr(mode, "+") != NULL)
    {
        printf("%s: %swarning:%s file can not be read%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    FILE *fp = fopen(path, mode);

    if (fp == NULL) 
    {
        printf("%s: %swarning:%s failed to write content%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    if (strstr(mode, "b"))
    {
        fwrite(str->data, (size_t)str->size + 1, 1, fp);
    }
    else
    {
        fprintf(fp, "%s", str->data);
    }

    fclose(fp);
}

string_t *str_alloc_sys_output(const char *cmd)
{
    if (is_not_valid_c_str(cmd, __func__))
    {
        return NULL;
    }

    FILE *fp = popen(cmd, "r");
    string_t *str = alloc_read_file_content(fp);
    fclose(fp);

    return str;
}

int64_t str_ascii_total(string_t *str)
{
    return c_str_ascii_total(str->data);
}

int64_t c_str_ascii_total(const char *data)
{
    if (is_not_valid_c_str(data, __func__))
    {
        return -1;
    }

    int64_t ascii_total = 0;

    while (*data != '\0')
    {
        ascii_total += *data;
        data++;
    }

    return ascii_total;
}

int64_t str_ll(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return -1;
    }

    return atoll(str->data);
}

double str_double(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return -1.0;
    }

    return atof(str->data);
}

string_t *str_alloc_ll_to_binary_str(int64_t number, int64_t bits_shown)
{
    string_t *bi_num = str_alloc("");

    while (number > 0)
    {
        str_before(bi_num, get_str_number(number % 2));
        number /= 2;
    }

    if (bits_shown > bi_num->size)
    {
        bits_shown -= bi_num->size;

        for (int64_t i = 0; i < bits_shown; i++)
        {
            str_before(bi_num, "0");
        }
    }

    return bi_num;
}

string_t *str_alloc_c_str_to_binary_str(const char *number, int64_t bits_shown)
{
    if (is_not_valid_c_str(number, __func__))
    {
        return NULL;
    }

    return str_alloc_ll_to_binary_str(atoll(number), bits_shown);
}

string_t *str_alloc_str_to_binary_str(string_t *str, int64_t bits_shown)
{
    if (is_str_null(str, __func__))
    {
        return NULL;
    }

    return str_alloc_ll_to_binary_str(str_ll(str), bits_shown);
}

string_t *str_alloc_ll_to_str(int64_t number)
{
    string_t *digits = str_alloc("");

    while (number > 0)
    {
        str_before(digits, get_str_number((number % 10)));
        number /= 10;
    }

    return digits;
}

char *c_str_alloc(const char *data)
{
    if (is_c_str_null(data, __func__))
    {
        return NULL;
    }

    mem_usage.allocated += sizeof(char) * (strlen(data) + 1);
    return strdup(data);
}

string_t *str_alloc_copy(string_t *str)
{
    if (is_str_null(str, __func__))
    {
        return NULL;
    }

    return str_alloc(str->data);
}

void str_print(string_t *str, const char *beginning, const char *end)
{
    if (is_str_null(str, __func__)
        || is_c_str_null(beginning, __func__)
        || is_c_str_null(end, __func__))
    {
        return;
    }

    printf("%s%s%s", beginning, str->data, end);
}

void c_str_free(char **data)
{
    if (is_pointer_null(data, __func__) || 
        is_c_str_null(*data, __func__))
    {
        return;
    }

    free_mem(*data, sizeof(char) * (strlen(*data)+1));
    *data = NULL;
}

void str_free(string_t **str)
{
    if (is_pointer_null(str, __func__)
        || is_str_null(*str, __func__))
    {
        return;
    }

    str_data_free(*str);
    free_mem(*str, sizeof(string_t));
    *str = NULL;
}


/*
void sa_set_size(string_array_t *str_array, int64_t size)
{
    if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    str_array->size = size;
}

void sa_set_index(string_array_t *str_array, int64_t index, string_t *str)
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
*/

void str_array_set(string_array_t *str_array, int64_t index, string_t *str)
{
    if (is_str_array_null(str_array, __func__)
        || check_index(&index, str_array->size, __func__)
        || is_str_null(str, __func__))
    {
        return;
    }

    str_free(&str_array->data_set[index]);
    str_array->data_set[index] = str;
}

void str_array_set_c_str(string_array_t *str_array, int64_t index, const char *data)
{
    if (is_str_array_null(str_array, __func__)
        || check_index(&index, str_array->size, __func__)
        || is_c_str_null(data, __func__))
    {
        return;
    }

    str_free(&str_array->data_set[index]);
    str_array->data_set[index] = str_alloc(data);
}

int64_t str_array_get_size(string_array_t *str_array)
{
    if (is_str_array_null(str_array, __func__))
    {
        return -1;
    }

    return str_array->size;
}

string_t *str_array_get_index(string_array_t *str_array, int64_t index)
{
    if (is_str_array_null(str_array, __func__)
        || check_index(&index, str_array->size, __func__))
    {
        return NULL;
    }

    return str_array->data_set[index];
}

string_array_t *str_array_alloc(int64_t size)
{
    if (is_size_less_or_zero(size, __func__))
    {
        return NULL;
    }

    string_array_t *str_array = alloc_mem(sizeof(string_array_t));
    str_array->size = size;
    str_array->data_set = alloc_mem((size_t)str_array->size * sizeof(string_t*));

    for (int64_t i = 0; i < size; i++)
    {
        str_array->data_set[i] = str_alloc("");
    }

    return str_array;
}

string_array_t *str_array_alloc_strs(int64_t size, ...)
{
    if (is_size_less_or_zero(size, __func__))
    {
        return NULL;
    }

    string_array_t *str_array = alloc_mem(sizeof(string_array_t));
    str_array->size = size;
    str_array->data_set = alloc_mem((size_t)str_array->size * sizeof(string_t*));

    va_list args;
    va_start(args, size);

    for (int64_t i = 0; i < size; i++)
    {
        str_array->data_set[i] = va_arg(args, string_t*);
    }

    va_end(args);

    return str_array;
}

string_array_t *str_array_alloc_c_strs(int64_t size, ...)
{
    if (is_size_less_or_zero(size, __func__))
    {
        return NULL;
    }

    string_array_t *str_array = alloc_mem(sizeof(string_array_t));
    str_array->size = size;
    str_array->data_set = alloc_mem((size_t)str_array->size * sizeof(string_t*));

    va_list args;
    va_start(args, size);

    for (int64_t i = 0; i < size; i++)
    {
        str_array->data_set[i] = str_alloc(va_arg(args, char*));
    }

    va_end(args);

    return str_array;
}

string_array_t *str_alloc_split(string_t *str, const char *separator, int64_t max_split)
{
    if (is_str_null(str, __func__)
        || is_not_valid_c_str(separator, __func__))
    {
        return NULL;
    }

    return alloc_split_str(str->data, str->size, separator, max_split);
}

string_array_t *str_alloc_c_str_split(const char *data, const char *separator, int64_t max_split)
{
    if (is_not_valid_c_str(data, __func__)
        || is_not_valid_c_str(separator, __func__))
    {
        return NULL;
    }

    return alloc_split_str(data, (int64_t)strlen(data), separator, max_split);
}

string_array_t *str_array_alloc_read_keyboard(int64_t size, ...)
{
    if (is_size_less_or_zero(size, __func__))
    {
        return NULL;
    }

    string_array_t *str_array = str_array_alloc(size);

    va_list args;
    va_start(args, size);

    for (int64_t i = 0; i < size; i++)
    {
        str_array_set(str_array, i, str_alloc_read_keyboard(va_arg(args, char*)));
    }

    va_end(args);

    return str_array;
}

bool str_array_cmp(string_array_t *str_array, int64_t index, string_t *str)
{
    if (is_str_null(str, __func__)
        || is_str_array_null(str_array, __func__)
        || check_index(&index, str_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(str->data, (str_array->data_set[index]->data)));
}

bool str_array_cmp_c_str(string_array_t *str_array, int64_t index, const char *data)
{
    if (is_c_str_null(data, __func__)
        || is_str_array_null(str_array, __func__)
        || check_index(&index, str_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(data, (str_array->data_set[index]->data)));
}

void str_array_print(string_array_t *str_array, const char *beginning, const char *end)
{
    if (is_str_array_null(str_array, __func__)
        || is_c_str_null(beginning, __func__)
        || is_c_str_null(end, __func__))
    {
        return;
    }

    int64_t i;
    int64_t last_index = (str_array->size-1);

    printf("%s{", beginning);

    for (i = 0; i < last_index; i++)
    {
        printf("\"%s\", ", str_array->data_set[i]->data);
    }

    printf("\"%s\"}%s", str_array->data_set[i]->data, end);
}

void str_array_free(string_array_t **str_array)
{
    if (is_pointer_null(str_array, __func__)
        || is_str_array_null(*str_array, __func__))
    {
        return;
    }

    for (int64_t i = 0; i < (*str_array)->size; i++)
    {
        str_free(&(*str_array)->data_set[i]);
    }

    free_mem((*str_array)->data_set, (size_t)(*str_array)->size * sizeof(string_t*));
    free_mem(*str_array, sizeof(string_array_t));
    *str_array = NULL;
}
