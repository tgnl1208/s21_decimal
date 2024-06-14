#ifndef S21_DECIMAL
#define S21_DECIMAL

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DECIMAL_MAX 79228162514264337593543950335
#define DECIMAL_MIN -79228162514264337593543950335
#define MAX_BITS 4294967295
#define MAX_DEC powf(2.0f, 96.0f) - 1.0
#define EXPONENT_PLUS_1 65536
#define EXPONENT_PLUS_2 196608

typedef enum bool { false, true } bool;

typedef struct s21_decimal {
  unsigned int bits[4];
} s21_decimal;

// dopfunctions
int kolvo_digit(char* numberStr);
void print_binary(int number);
void print_decimal(s21_decimal number);
void set_sign(s21_decimal* num, int sign);
int set_string_sign(s21_decimal* num);
void set_scale(s21_decimal* num, int scale);
int set_string_scale(s21_decimal* num);
char* strreverse(char* str);
char* divide_by_two(const char* decimal_str);
char* decimal_to_binary(const char* decimal_str);
unsigned long long int from_charbits_to_intbits(char* charbits);
void remove_char(char* str, char char_to_remove);
void multiply_by_two(char* num);
void add_one(char* num);
int kolvo_digit(char* numberStr);
int kolvo_fracdigit(char* numberStr);
int normalizeTo(s21_decimal* res, int to);
int s21_bitAdd(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int comporator(s21_decimal value_1, s21_decimal value_2);
void s21_set_bit(s21_decimal* dst, int index, int bit);
int moveBitsTo(s21_decimal* result, int toMove);
int getFirstOne(s21_decimal dec);

// convertors ne po zadaniy
void from_charnum_to_decimal(char* numChar, s21_decimal* num);
void binary_to_decimal_string(const char* binary_str, char* decimal_str);
void from_intbits_to_charbits(char* charbits, unsigned int bits);
void from_decimal_to_charnum(char* numChar, s21_decimal* num);
void s21_zero_decimal(s21_decimal* dst);

// convertors
int s21_from_int_to_decimal(int src, s21_decimal* dst);
int s21_from_float_to_decimal(float src, s21_decimal* dst);
int s21_from_decimal_to_int(s21_decimal src, int* dst);
int s21_from_decimal_to_float(s21_decimal src, float* dst);

// Arithmetic Operators
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_bitSub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_bitRevers(s21_decimal* value_1, int mask_size);
int s21_bitDiv(s21_decimal value_1, s21_decimal value_2, s21_decimal* result,
               bool isDiv);

// other functions
int s21_floor(s21_decimal value, s21_decimal* result);
int s21_round(s21_decimal value, s21_decimal* result);
int s21_truncate(s21_decimal value, s21_decimal* result);
int s21_negate(s21_decimal value, s21_decimal* result);

// Comparisons
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);

#endif

// 79228162514264337593543950335