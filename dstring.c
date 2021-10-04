#include "dstring.h"

#define PURPLE                      "\033[1;95m"
#define RED                         "\033[1;91m"
#define WHITE                       "\033[1;97m"
#define RESET                       "\033[0m"

#define DEFAULT_CAPACITY            32
#define STR_MAX_CHARS               100000

typedef struct dstr
{
    size_t size;
    size_t capacity;
    char *data;
} dstr_t;

typedef struct dstr_arr
{
    size_t size;
    dstr_t **data_set;
} dstr_arr_t;

static bool is_dstr_null(dstr_t *dstr, const char *func_name)
{
    if (dstr == NULL)
    {
        printf("%s: %swarning:%s dstr is NULL%s\n", func_name, PURPLE, WHITE, RESET);
        return true;
    }

    return false;
}

static bool is_str_null(const char *data, const char *func_name)
{
    bool is_null = (data == NULL);

    if (is_null)
    {
        printf("%s: %swarning:%s string literal input is NULL%s\n", func_name, PURPLE, WHITE, RESET);
    }

    return is_null;
}

static bool is_not_valid_str(const char *data, const char *func_name)
{
    bool is_valid_str = ((data != NULL) && (data[0] != '\0'));

    if (!(is_valid_str))
    {
        printf("%s: %swarning:%s string literal input is not a valid string%s\n", func_name, PURPLE, WHITE, RESET);
    }

    return !(is_valid_str);
}

static bool is_dstr_arr_null(dstr_arr_t *dstr_array, const char *func_name)
{
    bool is_null = (dstr_array == NULL);

    if (is_null)
    {
        printf("%s: %swarning:%s dstr_arr is NULL%s\n", func_name, PURPLE, WHITE, RESET);
    }

    return is_null;
}

static bool is_size_zero(size_t size, const char *func_name)
{
    bool is_valid_size = (size == 0);

    if (is_valid_size)
    {
        printf("%s: %swarning:%s size is equal to zero%s\n", func_name, PURPLE, WHITE, RESET);
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

static bool check_index(int64_t *index, size_t size, const char *func_name)
{
    int64_t index_copy = *index;

    if (*index < 0)
    {
        *index += size;
    }

    if (*index < 0 || *index >= (int64_t)size)
    {
        printf("%s: %swarning:%s index %lld is out of range%s\n", func_name, PURPLE, WHITE, index_copy, RESET);
        return true;
    }

    return false;
}

static bool check_ranges(int64_t *start, int64_t *end, size_t size, const char *func_name)
{
    int64_t size_copy = (int64_t)size;

    if (*start >= size_copy || *end > size_copy)
    {
        printf("%s: %swarning:%s indices are out of range%s\n", func_name, PURPLE, WHITE, RESET);
        return true;
    }

    if (*start < 0)
    {
        *start += size;
    }

    if (*end == 0)
    {
        *end = size_copy;
    }
    else if (*end < 0)
    {
        *end += size_copy;
    }

    if (*start < 0 || *end > size_copy || (*end - *start) <= 0)
    {
        printf("%s: %swarning:%s indices are out of range%s\n", func_name, PURPLE, WHITE, RESET);
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

static int64_t get_start_index(int64_t *start_opt, size_t size, bool is_step_neg)
{
    if (start_opt == NULL)
    {
        return is_step_neg ? (int64_t)(size - 1) : 0;
    }

    int64_t start = *start_opt;

    if (start < 0)
    {
        start += size;
    }

    return start;
}

static int64_t get_end_index(int64_t *end_opt, size_t size, bool is_step_neg)
{
    int64_t size_copy = (int64_t)size;

    if (end_opt == NULL)
    {
        return is_step_neg ? -1 : size_copy;
    }

    int64_t end = *end_opt;

    if (end < 0)
    {
        end += size_copy;
    }
    else if (end > size_copy)
    {
        end = size_copy;
    }

    return end;
}

static size_t get_sub_size(int64_t start, int64_t end, bool is_step_neg)
{
    if (start < 0)
    {
        return 0;
    }

    size_t sub_str_size = is_step_neg ? (size_t)(start - end) : (size_t)(end - start);

    if (sub_str_size == 0)
    {
        return 0;
    }

    return sub_str_size;
}

static size_t ceil_lu(size_t x, size_t y) 
{
    return (size_t)ceil(x / (double)y);
}

static size_t count_occurrences_in_str(const char *data, const char *search_val, size_t count, size_t start, size_t end)
{
    if (*search_val == '\0')
    {
        return 0;
    }

    size_t data_size = strlen(data);
    char *data_alloc = strdup(data);
    char *data_copy = data_alloc;

    add_to_allocated(sizeof(char) * (data_size + 1));

    data_copy = &data_copy[start];
    data_copy[end-start] = '\0';

    size_t occurrences = 0;
    size_t search_val_size = strlen(search_val);
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

    free_mem(data_alloc, sizeof(char) * (data_size + 1));

    return occurrences;
}

static dstr_t *alloc_read_file_content(FILE *fp)
{
    char file_line[STR_MAX_CHARS];
    dstr_t *dstr = dstr_alloc("");

    if (fp == NULL) 
    {
        printf("%s: %swarning:%s failed to read content%s\n", __func__, PURPLE, WHITE, RESET);
        return NULL;
    }

    while (!feof(fp))
    {
        if (fgets(file_line, STR_MAX_CHARS, fp) != NULL)
        {
            dstr_append(dstr, file_line);
        }
    }

    return dstr;
}

static dstr_arr_t *alloc_split_str(const char *data, size_t size, const char *separator, size_t max_split)
{
    size_t i = 0;
    char *data_alloc = strdup(data);
    char *data_copy = data_alloc;
    size_t num_of_occurrences = count_occurrences_in_str(data_copy, separator, max_split, 0, size);

    add_to_allocated(sizeof(char) * (size + 1));

    dstr_arr_t *dstr_array = alloc_mem(sizeof(dstr_arr_t));
    dstr_array->size = num_of_occurrences + 1;
    dstr_array->data_set = alloc_mem(dstr_array->size * sizeof(dstr_t*));

    while (i < num_of_occurrences)
    {
        dstr_array->data_set[i] = dstr_alloc(strsep_m(&data_copy, separator));
        i++;
    }

    dstr_array->data_set[i] = dstr_alloc(data_copy);
    free_mem(data_alloc, sizeof(char) * (size + 1));

    return dstr_array;
}

static size_t update_capacity(size_t size, size_t capacity)
{
    while (size > capacity)
    {
        capacity *= 2;
    }

    return capacity;
}

static size_t calculate_capacity(size_t size)
{
    return update_capacity(size, DEFAULT_CAPACITY);
}

static void set_empty_dstr(dstr_t *dstr)
{
    dstr->size = 0;
    dstr->capacity = DEFAULT_CAPACITY;
    dstr->data = alloc_mem(sizeof(char) * (dstr->capacity + 1));
    dstr->data[0] = '\0';
}

static dstr_t *alloc_substr(const char *data, size_t data_size, int64_t *start_opt, int64_t *end_opt, int64_t *step_opt, const char *func_name)
{
    if (is_size_zero(data_size, __func__))
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
        printf("%s: %swarning:%s slice step cannot be equal to zero%s\n", func_name, PURPLE, WHITE, RESET);
        return NULL;
    }
    else
    {
        step = *step_opt;
    }

    bool is_step_neg = (step < 0);
    int64_t start = get_start_index(start_opt, data_size, is_step_neg);
    int64_t end = get_end_index(end_opt, data_size, is_step_neg);
    size_t size = get_sub_size(start, end, is_step_neg);
    dstr_t *dsub_str = alloc_mem(sizeof(dstr_t));

    if (size == 0 || start >= (int64_t)data_size)
    {
        set_empty_dstr(dsub_str);
        return dsub_str;
    }

    size_t abs_step = (step < 0) ? (size_t)(step * -1) : (size_t)step;

    dsub_str->size = (abs_step >= size) ? 1 : ceil_lu(size, abs_step);
    dsub_str->capacity = calculate_capacity(dsub_str->size);
    dsub_str->data = alloc_mem(sizeof(char) * (dsub_str->capacity + 1));

    for (size_t i = 0; i < dsub_str->size; i++)
    {
        dsub_str->data[i] = data[start];
        start += step;
    }

    dsub_str->data[dsub_str->size] = '\0';

    return dsub_str;
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

static size_t str_realloc_capacity(dstr_t *dstr, const char *data)
{
    size_t old_capacity = 0;
    size_t data_size = strlen(data);
    dstr->size += data_size;

    if (dstr->size > dstr->capacity)
    {
        old_capacity = dstr->capacity;
        dstr->capacity = update_capacity(dstr->size, dstr->capacity);
        dstr->data = realloc(dstr->data, sizeof(char) * (dstr->capacity + 1));

        add_to_allocated(sizeof(char) * (dstr->capacity - old_capacity));
    }

    return data_size;
}

static void dstr_data_free(dstr_t *dstr)
{
    free_mem(dstr->data, sizeof(char) * (dstr->capacity + 1));
}

static dstr_t *alloc_setup_capacity(size_t file_size)
{
    dstr_t *dstr = alloc_mem(sizeof(dstr_t));

    dstr->size = file_size;
    dstr->capacity = calculate_capacity(file_size);
    dstr->data = alloc_mem(sizeof(char) * (dstr->capacity + 1));

    return dstr;
}

size_t str_ascii_total(const char *data)
{
    if (is_not_valid_str(data, __func__))
    {
        return 0;
    }

    size_t ascii_total = 0;

    while (*data != '\0')
    {
        ascii_total += (size_t)*data;
        data++;
    }

    return ascii_total;
}

/*
void dstr_set_size(dstr_t *dstr, int64_t size)
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

void dstr_set_capacity(dstr_t *dstr, int64_t capacity)
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

void dstr_set_literal(dstr_t *dstr, char *data)
{
    if (check_warnings(str, STR_NULL, __func__))
    {
        return;
    }
    else if (is_not_valid_str(data, __func__))
    {
        return;
    }

    str->data = data;
}
*/

size_t dstr_get_size(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return 0;
    }

    return dstr->size;
}

size_t dstr_get_capacity(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return 0;
    }

    return dstr->capacity;
}

char *dstr_get_literal(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return NULL;
    }

    return dstr->data;
}

dstr_t *dstr_alloc(const char *data)
{
    if (is_str_null(data, __func__))
    {
        return NULL;
    }

    dstr_t *dstr = alloc_mem(sizeof(dstr_t));

    dstr->size = strlen(data);
    dstr->capacity = calculate_capacity(dstr->size);
    dstr->data = alloc_mem(sizeof(char) * (dstr->capacity + 1));

    memcpy(dstr->data, data, dstr->size + 1);

    return dstr;
}

dstr_t *dstr_alloc_va(size_t size, ...)
{
    if (is_size_zero(size, __func__))
    {
        return NULL;
    }

    va_list args;
    va_start(args, size);

    dstr_t *dstr = dstr_alloc(va_arg(args, char*));

    for (size_t i = 1; i < size; i++)
    {
        dstr_append(dstr, va_arg(args, char*));
    }

    va_end(args);

    return dstr;
}

void dstr_append(dstr_t *dstr, const char *data)
{
    if (is_dstr_null(dstr, __func__)
        || is_not_valid_str(data, __func__))
    {
        return;
    }

    size_t data_size = str_realloc_capacity(dstr, data);

    memcpy(&dstr->data[dstr->size - data_size], data, data_size + 1);
}

void dstr_append_va(dstr_t *dstr, size_t size, ...)
{
    if (is_dstr_null(dstr, __func__)
        || is_size_zero(size, __func__))
    {
        return;
    }

    va_list args;
    va_start(args, size);

    for (size_t i = 0; i < size; i++)
    {
        dstr_append(dstr, va_arg(args, char*));
    }

    va_end(args);
}

dstr_t *dstr_add(dstr_t *curr_dstr, dstr_t *newest_dstr)
{
    return dstr_add_va(2, curr_dstr, newest_dstr);
}

dstr_t *dstr_add_va(size_t size, ...)
{
    if (is_size_zero(size, __func__))
    {
        return NULL;
    }

    va_list args;
    va_start(args, size);

    dstr_t *temp_dstr = NULL;
    dstr_t *total_dstr = dstr_alloc_copy(va_arg(args, dstr_t*));

    for (size_t i = 1; i < size; i++)
    {
        temp_dstr = va_arg(args, dstr_t*);

        if (temp_dstr == NULL)
        {
            va_end(args);
            return total_dstr;
        }

        dstr_append(total_dstr, temp_dstr->data);
    }

    va_end(args);

    return total_dstr;
}

void dstr_add_equals(dstr_t *curr_dstr, dstr_t *newest_dstr)
{
    dstr_add_equals_va(curr_dstr, 1, newest_dstr);
}

void dstr_add_equals_va(dstr_t *dstr, size_t size, ...)
{
    if (is_dstr_null(dstr, __func__)
        || is_size_zero(size, __func__))
    {
        return;
    }

    va_list args;
    va_start(args, size);

    dstr_t *temp_dstr = NULL;

    for (size_t i = 0; i < size; i++)
    {
        temp_dstr = va_arg(args, dstr_t*);

        if (temp_dstr == NULL)
        {
            va_end(args);
            return;
        }

        dstr_append(dstr, temp_dstr->data);
    }

    va_end(args);
}

void dstr_before(dstr_t *dstr, const char *data)
{
    if (is_dstr_null(dstr, __func__)
        || is_not_valid_str(data, __func__))
    {
        return;
    }

    size_t data_size = str_realloc_capacity(dstr, data);

    memcpy(&dstr->data[data_size], dstr->data, dstr->size+1);
    memcpy(dstr->data, data, data_size);
}

dstr_t *dstr_alloc_substr(const char *data, int64_t *start_opt, int64_t *end_opt, int64_t *step_opt)
{
    if (is_not_valid_str(data, __func__))
    {
        return NULL;
    }

    return alloc_substr(data, strlen(data), start_opt, end_opt, step_opt, __func__);
}

dstr_t *dstr_alloc_subdstr(dstr_t *dstr, int64_t *start_opt, int64_t *end_opt, int64_t *step_opt)
{
    if (is_dstr_null(dstr, __func__))
    {
        return NULL;
    }

    return alloc_substr(dstr->data, dstr->size, start_opt, end_opt, step_opt, __func__);
}

void dstr_replace(dstr_t *dstr, const char *old_str, const char *new_str)
{
    if (is_dstr_null(dstr, __func__)
        || is_str_null(old_str, __func__)
        || is_str_null(new_str, __func__))
    {
        return;
    }

    dstr_replace_count(dstr, old_str, new_str, 0);
}

void dstr_replace_count(dstr_t *dstr, const char *old_str, const char *new_str, size_t count)
{
    if (is_dstr_null(dstr, __func__)
        || is_str_null(old_str, __func__)
        || is_str_null(new_str, __func__))
    {
        return;
    }

    size_t num_of_occurrences = count_occurrences_in_str(dstr->data, old_str, count, 0, dstr->size);

    if (num_of_occurrences == 0)
    {
        printf("%s: %swarning:%s could not find substring%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    size_t i = 0;
    size_t num_of_replacements = 0;
    size_t old_str_size = strlen(old_str);
    size_t new_str_size = strlen(new_str);
    size_t total_size = ((dstr->size - (old_str_size  * num_of_occurrences)) + (new_str_size * num_of_occurrences));

    size_t capacity = calculate_capacity(total_size);

    char *copy = dstr->data;
    char *replacement = alloc_mem(sizeof(char) * (capacity + 1));

    while (*copy != '\0')
    {
        if (num_of_replacements < num_of_occurrences && strstr(copy, old_str) == copy)
        {
            memcpy(&replacement[i], new_str, new_str_size);
            num_of_replacements++;
            i += new_str_size;
            copy += old_str_size;
        }
        else
        {
            replacement[i++] = *copy++;
        }
    }

    replacement[i] = '\0';

    dstr_data_free(dstr);

    dstr->size = total_size;
    dstr->capacity = capacity;
    dstr->data = replacement;
}

void dstr_erase(dstr_t *dstr, const char *data)
{
    if (is_dstr_null(dstr, __func__)
        || is_str_null(data, __func__))
    {
        return;
    }

    dstr_replace_count(dstr, data, "", 0);
}

void str_erase_count(dstr_t *dstr, const char *data, size_t count)
{
    if (is_dstr_null(dstr, __func__)
        || is_str_null(data, __func__))
    {
        return;
    }

    dstr_replace_count(dstr, data, "", count);
}

void dstr_erase_index(dstr_t *dstr, int64_t start, int64_t end)
{
    if (is_dstr_null(dstr, __func__)
        || check_ranges(&start, &end, dstr->size, __func__))
    {
        return;
    }

    size_t conjoin_data_size = dstr->size - (size_t)(end - start);
    size_t capacity = calculate_capacity(conjoin_data_size);
    char *conjoin_data = alloc_mem(sizeof(char) * (capacity + 1));

    memcpy(conjoin_data, dstr->data, start);
    memcpy(&conjoin_data[start], &dstr->data[end], (conjoin_data_size - (size_t)start));
    conjoin_data[conjoin_data_size] = '\0';

    dstr_data_free(dstr);

    dstr->data = conjoin_data;
    dstr->size = conjoin_data_size;
    dstr->capacity = capacity;
}

size_t dstr_find(dstr_t *dstr, const char *search_val)
{
    if (is_dstr_null(dstr, __func__)
        || is_not_valid_str(search_val, __func__))
    {
        return 0;
    }

    char *found = strstr(dstr->data, search_val);

    if (found == NULL)
    {
        printf("%s: %swarning:%s could not find the searched string%s\n", __func__, PURPLE, WHITE, RESET);
        return 0;
    }

    return (size_t)(found - dstr->data);
}

size_t dstr_count(dstr_t *dstr, const char *search_val, int64_t start, int64_t end)
{
    if (is_dstr_null(dstr, __func__)
        || is_not_valid_str(search_val, __func__) 
        || check_ranges(&start, &end, dstr->size, __func__))
    {
        return 0;
    }

    return count_occurrences_in_str(dstr->data, search_val, 0, (size_t)start, (size_t)end);
}

void dstr_lstrip(dstr_t *dstr, const char *characters)
{
    if (is_dstr_null(dstr, __func__)
        || is_not_valid_str(characters, __func__))
    {
        return;
    }

    char *copy = dstr->data;

    while (strchr(characters, *copy))
    {
        copy++;
    }

    if (copy == dstr->data)
    {
        return;
    }
    else if (*copy != '\0')
    {
        size_t striped_size = dstr->size - (size_t)(copy - dstr->data);
        size_t capacity = calculate_capacity(striped_size);
        char *striped = alloc_mem(sizeof(char) * (capacity + 1));

        memcpy(striped, copy, striped_size + 1);
        dstr_data_free(dstr);

        dstr->data = striped;
        dstr->size = striped_size;
        dstr->capacity = capacity;
    }
    else
    {
        dstr_data_free(dstr);
        set_empty_dstr(dstr);
    }
}

void dstr_rstrip(dstr_t *dstr, const char *characters)
{
    if (is_dstr_null(dstr, __func__)
        || is_not_valid_str(characters, __func__))
    {
        return;
    }

    char *forward = dstr->data + dstr->size;

    while (strchr(characters, (*(forward-1))))
    {
        forward--;
    }

    if (forward == (dstr->data + dstr->size))
    {
        return;
    }
    else if (forward != dstr->data)
    {
        size_t striped_size = (size_t)(forward - dstr->data);
        size_t capacity = calculate_capacity(striped_size);
        char *striped = alloc_mem(sizeof(char) * (capacity + 1));

        memcpy(striped, dstr->data, striped_size);
        dstr_data_free(dstr);
        striped[striped_size] = '\0';

        dstr->data = striped;
        dstr->size = striped_size;
        dstr->capacity = capacity;
    }
    else
    {
        dstr_data_free(dstr);
        set_empty_dstr(dstr);
    }
}

void dstr_strip(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return;
    }

    dstr_lstrip(dstr, "\n ");
    dstr_rstrip(dstr, "\n ");
}

void dstr_strip_chars(dstr_t *dstr, const char *characters)
{
    if (is_dstr_null(dstr, __func__)
        || is_not_valid_str(characters, __func__))
    {
        return;
    }

    dstr_lstrip(dstr, characters);
    dstr_rstrip(dstr, characters);
}

void dstr_upper(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return;
    }

    size_t i = 0;

    while (dstr->data[i] != '\0')
    {
        dstr->data[i] = (char)toupper(dstr->data[i]);
        i++;
    }
}

void dstr_lower(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return;
    }

    size_t i = 0;

    while (dstr->data[i] != '\0')
    {
        dstr->data[i] = (char)tolower(dstr->data[i]);
        i++;
    }
}

void dstr_swapcase(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return;
    }

    size_t i = 0;

    while (dstr->data[i] != '\0')
    {
        dstr->data[i] = isupper(dstr->data[i]) ? (char)tolower(dstr->data[i]) : (char)toupper(dstr->data[i]);
        i++;
    }
}

void dstr_capitalize(dstr_t *dstr)
{  
    if (is_dstr_null(dstr, __func__))
    {
        return;
    }

    dstr->data[0] = (char)toupper(dstr->data[0]);
}

void dstr_title(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return;
    }

    dstr_capitalize(dstr);

    for (size_t i = (dstr->size-1); i > 0; i--)
    {
        if (!(isalpha(dstr->data[i-1])))
        {
            dstr->data[i] = (char)toupper(dstr->data[i]);
        }
    }
}

dstr_t *dstr_alloc_read_keyboard(const char *output_message)
{
    if (is_str_null(output_message, __func__))
    {
        return NULL;
    }

    char input[STR_MAX_CHARS];
    dstr_t *dstr = alloc_mem(sizeof(dstr_t));

    dstr->size = prompt(output_message, "%s", input, sizeof(input));
    dstr->capacity = calculate_capacity(dstr->size);
    dstr->data = alloc_mem(sizeof(char) * (dstr->capacity + 1));

    memcpy(dstr->data, input, dstr->size + 1);

    return dstr;
}

dstr_t *dstr_alloc_read_file(const char *path, const char *mode)
{
    if (is_not_valid_str(path, __func__)
        || is_not_valid_str(mode, __func__))
    {
        return NULL;
    }

    if (strstr(mode, "w") != NULL || strstr(mode, "a") != NULL)
    {
        printf("%s: %swarning:%s file can not be written%s\n", __func__, PURPLE, WHITE, RESET);
        return NULL;
    }

    FILE *fp = fopen(path, mode);
    dstr_t *dstr = NULL;

    if (strstr(mode, "b"))
    {
        fseek(fp, 0, SEEK_END);
        size_t file_size = (size_t)ftell(fp);
        fseek(fp, 0, SEEK_SET);
    
        dstr = alloc_setup_capacity(file_size);

        fread(dstr->data, dstr->size + 1, 1, fp);
    }
    else
    {
        dstr = alloc_read_file_content(fp);
    }

    fclose(fp);

    return dstr;
}

void str_write_file(dstr_t *dstr, const char *path, const char *mode)
{
    if (is_dstr_null(dstr, __func__)
        || is_not_valid_str(path, __func__)
        || is_not_valid_str(mode, __func__))
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
        fwrite(dstr->data, dstr->size + 1, 1, fp);
    }
    else
    {
        fprintf(fp, "%s", dstr->data);
    }

    fclose(fp);
}

dstr_t *dstr_alloc_sys_output(const char *cmd)
{
    if (is_not_valid_str(cmd, __func__))
    {
        return NULL;
    }

    FILE *fp = popen(cmd, "r");
    dstr_t *dstr = alloc_read_file_content(fp);
    fclose(fp);

    return dstr;
}

size_t dstr_ascii_total(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return 0;
    }

    return str_ascii_total(dstr->data);
}

int64_t dstr_ll(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return 0;
    }

    return strtoll(dstr->data, NULL, 10);
}

double dstr_double(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return 0.0;
    }

    return strtod(dstr->data, NULL);
}

dstr_t *dstr_alloc_ll_to_dstr(int64_t number)
{
    dstr_t *digits = dstr_alloc("");

    while (number > 0)
    {
        dstr_before(digits, get_str_number((number % 10)));
        number /= 10;
    }

    return digits;
}

dstr_t *dstr_alloc_ll_to_binary_dstr(int64_t number, size_t bits_shown)
{
    dstr_t *bi_num = dstr_alloc("");

    while (number > 0)
    {
        dstr_before(bi_num, get_str_number(number % 2));
        number /= 2;
    }

    if (bits_shown > bi_num->size)
    {
        bits_shown -= bi_num->size;

        for (size_t i = 0; i < bits_shown; i++)
        {
            dstr_before(bi_num, "0");
        }
    }

    return bi_num;
}

dstr_t *dstr_alloc_str_to_binary_dstr(const char *number, size_t bits_shown)
{
    if (is_not_valid_str(number, __func__))
    {
        return NULL;
    }

    return dstr_alloc_ll_to_binary_dstr(strtoll(number, NULL, 10), bits_shown);
}

dstr_t *dstr_alloc_dstr_to_binary_dstr(dstr_t *dstr, size_t bits_shown)
{
    if (is_dstr_null(dstr, __func__))
    {
        return NULL;
    }

    return dstr_alloc_ll_to_binary_dstr(dstr_ll(dstr), bits_shown);
}

dstr_t *dstr_alloc_copy(dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__))
    {
        return NULL;
    }

    return dstr_alloc(dstr->data);
}

void dstr_print(dstr_t *dstr, const char *beginning, const char *end)
{
    if (is_dstr_null(dstr, __func__)
        || is_str_null(beginning, __func__)
        || is_str_null(end, __func__))
    {
        return;
    }

    printf("%s%s%s", beginning, dstr->data, end);
}

void dstr_free(dstr_t **dstr)
{
    if (is_pointer_null(dstr, __func__)
        || is_dstr_null(*dstr, __func__))
    {
        return;
    }

    dstr_data_free(*dstr);
    free_mem(*dstr, sizeof(dstr_t));
    *dstr = NULL;
}


/*
void sa_set_size(dstr_arr_t *dstr_array, int64_t size)
{
    if (str_array == NULL)
    {
        printf("%s: %swarning:%s string_array is NULL%s\n", __func__, PURPLE, WHITE, RESET);
        return;
    }

    str_array->size = size;
}

void sa_set_index(dstr_arr_t d*str_array, int64_t index, dstr_t *dstr)
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

void dstr_arr_set_dstr(dstr_arr_t *dstr_array, int64_t index, dstr_t *dstr_input)
{
    if (is_dstr_arr_null(dstr_array, __func__)
        || check_index(&index, dstr_array->size, __func__)
        || is_dstr_null(dstr_input, __func__))
    {
        return;
    }

    dstr_free(&dstr_array->data_set[index]);
    dstr_array->data_set[index] = dstr_input;
}

void dstr_arr_set(dstr_arr_t *dstr_array, int64_t index, const char *data)
{
    if (is_dstr_arr_null(dstr_array, __func__)
        || check_index(&index, dstr_array->size, __func__)
        || is_str_null(data, __func__))
    {
        return;
    }

    dstr_free(&dstr_array->data_set[index]);
    dstr_array->data_set[index] = dstr_alloc(data);
}

size_t dstr_arr_get_size(dstr_arr_t *dstr_array)
{
    if (is_dstr_arr_null(dstr_array, __func__))
    {
        return 0;
    }

    return dstr_array->size;
}

dstr_t *dstr_arr_get_index(dstr_arr_t *dstr_array, int64_t index)
{
    if (is_dstr_arr_null(dstr_array, __func__)
        || check_index(&index, dstr_array->size, __func__))
    {
        return NULL;
    }

    return dstr_array->data_set[index];
}

dstr_arr_t *dstr_arr_alloc(size_t size)
{
    if (is_size_zero(size, __func__))
    {
        return NULL;
    }

    dstr_arr_t *dstr_array = alloc_mem(sizeof(dstr_arr_t));
    dstr_array->size = size;
    dstr_array->data_set = alloc_mem(dstr_array->size * sizeof(dstr_t*));

    for (size_t i = 0; i < dstr_array->size; i++)
    {
        dstr_array->data_set[i] = dstr_alloc("");
    }

    return dstr_array;
}

dstr_arr_t *dstr_arr_alloc_strs(size_t size, ...)
{
    if (is_size_zero(size, __func__))
    {
        return NULL;
    }

    dstr_arr_t *dstr_array = alloc_mem(sizeof(dstr_arr_t));
    dstr_array->size = size;
    dstr_array->data_set = alloc_mem(dstr_array->size * sizeof(dstr_t*));

    va_list args;
    va_start(args, size);

    for (size_t i = 0; i < dstr_array->size; i++)
    {
        dstr_array->data_set[i] = dstr_alloc(va_arg(args, char*));
    }

    va_end(args);

    return dstr_array;
}

dstr_arr_t *dstr_arr_alloc_dstrs(size_t size, ...)
{
    if (is_size_zero(size, __func__))
    {
        return NULL;
    }

    dstr_arr_t *dstr_array = alloc_mem(sizeof(dstr_arr_t));
    dstr_array->size = size;
    dstr_array->data_set = alloc_mem(dstr_array->size * sizeof(dstr_t*));

    va_list args;
    va_start(args, size);

    for (size_t i = 0; i < dstr_array->size; i++)
    {
        dstr_array->data_set[i] = va_arg(args, dstr_t*);
    }

    va_end(args);

    return dstr_array;
}

dstr_arr_t *dstr_alloc_splitstr(const char *data, const char *separator, size_t max_split)
{
    if (is_not_valid_str(data, __func__)
        || is_not_valid_str(separator, __func__))
    {
        return NULL;
    }

    return alloc_split_str(data, strlen(data), separator, max_split);
}

dstr_arr_t *dstr_alloc_splitdstr(dstr_t *dstr, const char *separator, size_t max_split)
{
    if (is_dstr_null(dstr, __func__)
        || is_not_valid_str(separator, __func__))
    {
        return NULL;
    }

    return alloc_split_str(dstr->data, dstr->size, separator, max_split);
}

dstr_arr_t *dstr_arr_alloc_read_keyboard(size_t size, ...)
{
    if (is_size_zero(size, __func__))
    {
        return NULL;
    }

    dstr_arr_t *dstr_array = dstr_arr_alloc(size);

    va_list args;
    va_start(args, size);

    for (size_t i = 0; i < dstr_array->size; i++)
    {
        dstr_arr_set_dstr(dstr_array, (int64_t)i, dstr_alloc_read_keyboard(va_arg(args, char*)));
    }

    va_end(args);

    return dstr_array;
}

bool dstr_arr_cmp(dstr_arr_t *dstr_array, int64_t index, const char *data)
{
    if (is_str_null(data, __func__)
        || is_dstr_arr_null(dstr_array, __func__)
        || check_index(&index, dstr_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(data, (dstr_array->data_set[index]->data)));
}

bool dstr_arr_cmp_dstr(dstr_arr_t *dstr_array, int64_t index, dstr_t *dstr)
{
    if (is_dstr_null(dstr, __func__)
        || is_dstr_arr_null(dstr_array, __func__)
        || check_index(&index, dstr_array->size, __func__))
    {
        return false;
    }

    return !(strcmp(dstr->data, (dstr_array->data_set[index]->data)));
}

void dstr_arr_print(dstr_arr_t *dstr_array, const char *beginning, const char *end)
{
    if (is_dstr_arr_null(dstr_array, __func__)
        || is_str_null(beginning, __func__)
        || is_str_null(end, __func__))
    {
        return;
    }

    size_t i;
    size_t last_index = (dstr_array->size-1);

    printf("%s{", beginning);

    for (i = 0; i < last_index; i++)
    {
        printf("\"%s\", ", dstr_array->data_set[i]->data);
    }

    printf("\"%s\"}%s", dstr_array->data_set[i]->data, end);
}

void dstr_arr_free(dstr_arr_t **dstr_array)
{
    if (is_pointer_null(dstr_array, __func__)
        || is_dstr_arr_null(*dstr_array, __func__))
    {
        return;
    }

    for (size_t i = 0; i < (*dstr_array)->size; i++)
    {
        dstr_free(&(*dstr_array)->data_set[i]);
    }

    free_mem((*dstr_array)->data_set, (*dstr_array)->size * sizeof(dstr_t*));
    free_mem(*dstr_array, sizeof(dstr_arr_t));
    *dstr_array = NULL;
}
