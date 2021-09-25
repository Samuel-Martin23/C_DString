#ifndef DSTRING_H
#define DSTRING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include "../C_Allocation_Metrics/allocation_metrics.h"

#define STR_MAX_CHARS           100000

typedef struct string string_t;
typedef struct string_array string_array_t;

int64_t c_str_input(char *input, const int64_t MAX_SIZE);

/*
void str_set_size(string_t *str, int64_t size);
void str_set_capacity(string_t *str, int64_t capacity);
void str_set_literal(string_t *str, char *data);
*/

int64_t str_get_size(string_t *str);
int64_t str_get_capacity(string_t *str);
char *str_get_literal(string_t *str);

string_t *str_alloc(const char *data);
string_t *str_alloc_va(int64_t size, ...);
void str_append(string_t *str, const char *data);
void str_append_va(string_t *str, int64_t size, ...);
string_t *str_add(string_t *curr_str, string_t *newest_str);
string_t *str_add_va(int64_t size, ...);
void str_add_equals(string_t *curr_str, string_t *newest_str);
void str_add_equals_va(string_t *str, int64_t size, ...);
void str_before(string_t *str, const char *data);
string_t *str_alloc_substr(string_t *str, int64_t *start_opt, int64_t *end_opt, int64_t *step_opt);
string_t *str_alloc_c_substr(const char *data, int64_t *start_opt, int64_t *end_opt, int64_t *step_opt);
void str_replace(string_t *str, const char *old_c_str, const char *new_c_str);
void str_replace_count(string_t *str, const char *old_c_str, const char *new_c_str, int64_t count);
void str_erase(string_t *str, const char *data);
void str_erase_count(string_t *str, const char *data, int64_t count);
void str_erase_index(string_t *str, int64_t start, int64_t end);
int64_t str_find(string_t *str, const char *search_val);
int64_t str_count(string_t *str, const char *search_val, int64_t start, int64_t end);
void str_lstrip(string_t *str, const char *characters);
void str_rstrip(string_t *str, const char *characters);
void str_strip(string_t *str);
void str_strip_chars(string_t *str, const char *characters);
void str_upper(string_t *str);
void str_lower(string_t *str);
void str_swapcase(string_t *str);
void str_capitalize(string_t *str);
void str_title(string_t *str);
string_t *str_alloc_read_keyboard(const char *output_message);
string_t *str_alloc_read_file(const char *path, const char *mode);
void str_write_file(string_t *str, const char *path, const char *mode);
string_t *str_alloc_sys_output(const char *cmd);
int64_t str_ascii_total(string_t *str);
int64_t c_str_ascii_total(const char *data);
int64_t str_ll(string_t *str);
double str_double(string_t *str);
string_t *str_alloc_ll_to_binary_str(int64_t number, int64_t bits_shown);
string_t *str_alloc_c_str_to_binary_str(const char *number, int64_t bits_shown);
string_t *str_alloc_str_to_binary_str(string_t *str, int64_t bits_shown);
string_t *str_alloc_ll_to_str(int64_t number);
char *c_str_alloc(const char *data);
string_t *str_alloc_copy(string_t *str);
void str_print(string_t *str, const char *beginning, const char *end);
void c_str_free(char **curr);
void str_free(string_t **str);

/*
void sa_set_size(string_array_t *str_array, int64_t size);
void sa_set_index(string_array_t *str_array, int64_t index, string_t *str);
*/
void str_array_set_str(string_array_t *str_array, int64_t index, string_t *input);
void str_array_set_c_str(string_array_t *str_array, int64_t index, const char *data);
int64_t str_array_get_size(string_array_t *str_array);
string_t *str_array_get_index(string_array_t *str_array, int64_t index);

string_array_t *str_array_alloc(int64_t size);
string_array_t *str_array_alloc_strs(int64_t size, ...);
string_array_t *str_array_alloc_c_strs(int64_t size, ...);
string_array_t *str_alloc_split(string_t *str, const char *separator, int64_t max_split);
string_array_t *str_alloc_c_str_split(const char *data, const char *separator, int64_t max_split);
string_array_t *str_array_alloc_read_keyboard(int64_t size, ...);
bool str_array_cmp(string_array_t *str_array, int64_t index, string_t *str);
bool str_array_cmp_c_str(string_array_t *str_array, int64_t index, const char *data);
void str_array_print(string_array_t *str_array, const char *beginning, const char *end);
void str_array_free(string_array_t **str_array);

#endif /* DSTRING_H */
