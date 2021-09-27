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

typedef struct dstr dstr_t;
typedef struct dstr_arr dstr_arr_t;

int64_t prompt(const char *output_message, char *input, const int64_t MAX_SIZE);
/*
int scanf_flush(const char *format, ...);
*/

char *str_alloc(const char *data);
int64_t str_ascii_total(const char *data);
void str_free(char **curr);

/*
void dstr_set_size(dstr_t *str, int64_t size);
void dstr_set_capacity(dstr_t *str, int64_t capacity);
void dstr_set_literal(dstr_t *str, char *data);
*/
int64_t dstr_get_size(dstr_t *dstr);
int64_t dstr_get_capacity(dstr_t *dstr);
char *dstr_get_literal(dstr_t *dstr);

dstr_t *dstr_alloc(const char *data);
dstr_t *dstr_alloc_va(int64_t size, ...);
void dstr_append(dstr_t *dstr, const char *data);
void dstr_append_va(dstr_t *dstr, int64_t size, ...);
dstr_t *dstr_add(dstr_t *curr_dstr, dstr_t *newest_dstr);
dstr_t *dstr_add_va(int64_t size, ...);
void dstr_add_equals(dstr_t *curr_dstr, dstr_t *newest_dstr);
void dstr_add_equals_va(dstr_t *str, int64_t size, ...);
void dstr_before(dstr_t *dstr, const char *data);
dstr_t *dstr_alloc_substr(const char *data, int64_t *start_opt, int64_t *end_opt, int64_t *step_opt);
dstr_t *dstr_alloc_subdstr(dstr_t *dstr, int64_t *start_opt, int64_t *end_opt, int64_t *step_opt);
void dstr_replace(dstr_t *dstr, const char *old_str, const char *new_str);
void dstr_replace_count(dstr_t *dstr, const char *old_str, const char *new_str, int64_t count);
void dstr_erase(dstr_t *dstr, const char *data);
void dstr_erase_count(dstr_t *dstr, const char *data, int64_t count);
void dstr_erase_index(dstr_t *dstr, int64_t start, int64_t end);
int64_t dstr_find(dstr_t *dstr, const char *search_val);
int64_t dstr_count(dstr_t *dstr, const char *search_val, int64_t start, int64_t end);
void dstr_lstrip(dstr_t *dstr, const char *characters);
void dstr_rstrip(dstr_t *dstr, const char *characters);
void dstr_strip(dstr_t *dstr);
void dstr_strip_chars(dstr_t *dstr, const char *characters);
void dstr_upper(dstr_t *dstr);
void dstr_lower(dstr_t *dstr);
void dstr_swapcase(dstr_t *dstr);
void dstr_capitalize(dstr_t *dstr);
void dstr_title(dstr_t *dstr);
dstr_t *dstr_alloc_read_keyboard(const char *output_message);
dstr_t *dstr_alloc_read_file(const char *path, const char *mode);
void dstr_write_file(dstr_t *dstr, const char *path, const char *mode);
dstr_t *dstr_alloc_sys_output(const char *cmd);
int64_t dstr_ascii_total(dstr_t *dstr);
int64_t dstr_ll(dstr_t *dstr);
double dstr_double(dstr_t *dstr);
dstr_t *dstr_alloc_ll_to_dstr(int64_t number);
dstr_t *dstr_alloc_ll_to_binary_dstr(int64_t number, int64_t bits_shown);
dstr_t *dstr_alloc_str_to_binary_dstr(const char *number, int64_t bits_shown);
dstr_t *dstr_alloc_dstr_to_binary_dstr(dstr_t *dstr, int64_t bits_shown);
dstr_t *dstr_alloc_copy(dstr_t *dstr);
void dstr_print(dstr_t *dstr, const char *beginning, const char *end);
void dstr_free(dstr_t **dstr);

/*
void sa_set_size(dstr_arr_t *dstr_array, int64_t size);
void sa_set_index(dstr_arr_t *dstr_array, int64_t index, dstr_t *str);
*/
void dstr_arr_set(dstr_arr_t *dstr_array, int64_t index, const char *data);
void dstr_arr_set_dstr(dstr_arr_t *dstr_array, int64_t index, dstr_t *dstr_input);
int64_t dstr_arr_get_size(dstr_arr_t *dstr_array);
dstr_t *dstr_arr_get_index(dstr_arr_t *dstr_array, int64_t index);

dstr_arr_t *dstr_arr_alloc(int64_t size);
dstr_arr_t *dstr_arr_alloc_strs(int64_t size, ...);
dstr_arr_t *dstr_arr_alloc_dstrs(int64_t size, ...);
dstr_arr_t *dstr_alloc_split_str(const char *data, const char *separator, int64_t max_split);
dstr_arr_t *dstr_alloc_split_dstr(dstr_t *dstr, const char *separator, int64_t max_split);
dstr_arr_t *dstr_arr_alloc_read_keyboard(int64_t size, ...);
bool dstr_arr_cmp(dstr_arr_t *dstr_array, int64_t index, const char *data);
bool dstr_arr_cmp_dstr(dstr_arr_t *dstr_array, int64_t index, dstr_t *dstr);
void dstr_arr_print(dstr_arr_t *dstr_array, const char *beginning, const char *end);
void dstr_arr_free(dstr_arr_t **dstr_array);

#endif /* DSTRING_H */
