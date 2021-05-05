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

typedef struct string string_t;
typedef struct string_array string_array_t;

int str_get_size(string_t *str);
int str_get_capacity(string_t *str);
char *str_get_literal(string_t *str);
void str_set_size(string_t *str, int size);
void str_set_capacity(string_t *str, int capacity);
void str_set_literal(string_t *str, char *data);

int sa_get_size(string_array_t *str_array);
string_t *sa_get_index(string_array_t *str_array, int index);
void sa_set_size(string_array_t *str_array, int size);
void sa_set_index(string_array_t *str_array, int index, string_t *str);

string_t *str_alloc(const char *data);
string_t *str_alloc_va(int size, ...);
void str_append(string_t *str, const char *data);
void str_append_va(string_t *str, int size, ...);
string_t *str_add(string_t *curr_str, string_t *newest_str);
string_t *str_add_va(int size, ...);
void str_add_equals(string_t *curr_str, string_t *newest_str);
void str_add_equals_va(string_t *str, int size, ...);
string_t *str_alloc_substr(string_t *str, int *start_opt, int *end_opt, int *step_opt);
void str_replace(string_t *str, const char *old, const char *new);
void str_replace_count(string_t *str, const char *old, const char *new, int count);
void str_erase(string_t *str, const char *data);
void str_erase_count(string_t *str, const char *data, int count);
void str_erase_index(string_t *str, int start, int end);
int str_find(string_t *str, const char *search_val);
int str_count(string_t *str, const char *search_val, int start, int end);
void str_lstrip(string_t *str, const char *characters);
void str_rstrip(string_t *str, const char *characters);
void str_strip(string_t *str);
void str_strip_chars(string_t *str, char *characters);
string_array_t *str_alloc_split(string_t *str, char *separator, int max_split);
string_array_t *c_str_alloc_split(const char *data, char *separator, int max_split);
bool sa_cmp_str(string_t *str, string_array_t *str_array, int index);
bool sa_cmp_c_str(const char *data, string_array_t *str_array, int index);
void str_upper(string_t *str);
void str_lower(string_t *str);
void str_swapcase(string_t *str);
void str_capitalize(string_t *str);
void str_title(string_t *str);
string_t *str_alloc_read_keyboard(const char *output_message);
string_t *str_alloc_read_file(const char *path);
void str_write_file(string_t *str, const char *path, bool append_file);
string_t *str_alloc_sys_output(const char *cmd);
int str_int(string_t *str);
double str_double(string_t *str);
char *c_str_alloc(const char *data);
string_t *str_alloc_copy(string_t *str);
void str_print(string_t *str, const char *beginning, const char *end);
void str_array_print(string_array_t *str_array, const char *beginning, const char *end);
void c_str_free(char **curr);
void str_free(string_t **str);
void str_data_free(string_t *str);
void str_array_free(string_array_t **str_array);

#endif /* DSTRING_H */
