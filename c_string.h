#ifndef C_STRING
#define C_STRING

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include "../C_Data_Structures/C_Allocation_Metrics/allocation_metrics.h"

typedef struct string string_t;
typedef struct string_array string_array_t;

void print_str(string_t *str, const char *beginning, const char *end);
void print_str_array(string_array_t *str_array, const char *beginning, const char *end);

int get_str_size(string_t *str);
char *get_char_str(string_t *str);
int get_str_array_size(string_array_t *str_array);
string_t *get_str_array_index(string_array_t *str_array, int index);

void init_str(string_t **str, const char *str_literal);
void append_str(string_t *str, const char *str_value);
void sub_str(string_t **str_dest, string_t *str_src, int start, int end, int step);
void copy_str(string_t **str_dest, string_t *str_src);
void replace_str(string_t *str, const char *old_str, const char *new_str, int count);
void erase_str(string_t *str, const char *value);
void erase_str_count(string_t *str, const char *value, int count);
void erase_str_index(string_t *str, int start, int end);
int find_str(string_t *str, const char *value);
int count_str(string_t *str, const char *value, int start, int end);
void lstrip_str(string_t *str, char *characters);
void rstrip_str(string_t *str, char *characters);
void strip_str(string_t *str);
void strip_str_chars(string_t *str, char *characters);
void upper_str(string_t *str);
void lower_str(string_t *str);
void swapcase_str(string_t *str);
void capitalize_str(string_t *str);
void title_str(string_t *str);
int int_str(string_t *str);
double double_str(string_t *str);
void split_str(string_array_t **str_array, string_t *str, const char *separator, int max_split);
void free_str(string_t **str);
void free_str_array(string_array_t **str_array);

#endif /* C_STRING */
