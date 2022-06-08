#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <stdio.h>
#include <math.h>
#include <limits.h>

typedef struct {
    int bits[4];
} s21_decimal;  // {{0, 0, 0, 0}};

// операторы сравнения
int s21_is_less(s21_decimal value_1, s21_decimal value_2);
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater(s21_decimal value_1, s21_decimal value_2);
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_equal(s21_decimal value_1, s21_decimal value_2);
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2);

// преобразователи
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
void s21_scale_equalization(s21_decimal* first, s21_decimal* second);

// арифметика
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// другие функции
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

// вспомогательные функции
int s21_get_sign(s21_decimal value);
int s21_get_bit(s21_decimal value, int i);
void s21_set_bit(s21_decimal *src, int i, int value);
void s21_set_sign(s21_decimal *src, int value);
void s21_set_scale(s21_decimal *value, int scale);
int s21_get_scale(s21_decimal value);
int s21_get_float_exp(float *src);
void s21_bits_copy(s21_decimal src, s21_decimal *dest);
int s21_last_bit(s21_decimal number);
int s21_shift_left(s21_decimal *first, int shift);
int s21_shift_right(s21_decimal *first, int shift);
void s21_reset(s21_decimal *value);

// доп. функции для деления
int integer_division(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
    s21_decimal *remainder, int flagInt);
void blue_electrical_tape(s21_decimal tmp_buf, s21_decimal *tmp_div, s21_decimal* tmp_mod,
    int* discharge);
void first_step(s21_decimal *tmp_div, s21_decimal value_2, int * scale_c,
    s21_decimal * tmp_res, int * index_res);
void first_prepare(s21_decimal tmp_div, s21_decimal *tmp_mod, s21_decimal *tmp_del,
    s21_decimal value_2, int * discharge);

#endif  // SRC_S21_DECIMAL_H_
