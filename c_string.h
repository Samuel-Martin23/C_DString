#ifndef C_STRING
#define C_STRING

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include "../C_Allocation_Metrics/allocation_metrics.h"

typedef struct string string_t;
typedef struct string_array string_array_t;

void print_str(string_t *str, const char *beginning, const char *end);
void print_str_array(string_array_t *str_array, const char *beginning, const char *end);

string_t *alloc_read_file_str(const char *path);
void write_file_str(string_t *str, const char *path, bool append_file);
string_t *alloc_sys_output_str(const char *cmd);

char *alloc_copy_c_str(const char *c_str);
string_t *alloc_copy_str(string_t *str);
int get_str_size(string_t *str);
char *get_str_c_str(string_t *str);
int get_sa_size(string_array_t *str_array);
string_t *get_sa_index(string_array_t *str_array, int index);
bool sa_cmp_c_str(const char *char_str, string_array_t *str_array, int index);
bool sa_cmp_str(string_t *str, string_array_t *str_array, int index);

string_t *alloc_str(const char *value);
string_t *alloc_va_str(int size, ...);
void append_str(string_t *str, const char *value);
void append_va_str(string_t *str, int size, ...);
string_t *alloc_sub_str(string_t *str, const char *start_opt, const char *end_opt, const char *step_opt);
void replace_str(string_t *str, const char *old, const char *new);
void replace_str_count(string_t *str, const char *old, const char *new, int count);
void erase_str(string_t *str, const char *value);
void erase_str_count(string_t *str, const char *value, int count);
void erase_str_index(string_t *str, int start, int end);
int find_str(string_t *str, const char *search_val);
int count_str(string_t *str, const char *search_val, int start, int end);
void lstrip_str(string_t *str, const char *characters);
void rstrip_str(string_t *str, const char *characters);
void strip_str(string_t *str);
void strip_str_chars(string_t *str, char *characters);
string_array_t *alloc_split_str(string_t *str, char *separator, int max_split);
string_array_t *alloc_split_c_str(const char *curr, char *separator, int max_split);
void upper_str(string_t *str);
void lower_str(string_t *str);
void swapcase_str(string_t *str);
void capitalize_str(string_t *str);
void title_str(string_t *str);
int int_str(string_t *str);
double double_str(string_t *str);
void free_c_str(char **curr);
void free_str(string_t **str);
void free_str_array(string_array_t **str_array);

#endif /* C_STRING */
