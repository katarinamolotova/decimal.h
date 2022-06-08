#include "s21_decimal.h"

int s21_get_bit(s21_decimal value, int i) {
    int res = 0;
    if (i / 32 < 4) {
        unsigned int mask = 1u << (i % 32);
        res = value.bits[i / 32] & mask;
    }
    return !!res;
}

void s21_set_bit(s21_decimal *src, int i, int value) {
    unsigned int mask = 1u << (i % 32);
    (value) ? (src->bits[i / 32] |= mask) : (src->bits[i / 32] &= ~mask);
}

void s21_set_sign(s21_decimal *src, int value) {
    s21_set_bit(src, 127, value);
}

int s21_get_sign(s21_decimal value) {
    return s21_get_bit(value, 127);
}

void s21_set_scale(s21_decimal *value, int scale) {
    int clearMask = ~(0xFF << 16);
    value->bits[3] &= clearMask;
    int mask = scale << 16;
    value->bits[3] |= mask;
}

int s21_get_scale(s21_decimal value) {
    return (char)(value.bits[3] >> 16);
}

int s21_get_float_exp(float *src) {
    return ((*(int*)src & ~0x80000000) >> 23) - 127;
}

void s21_bits_copy(s21_decimal src, s21_decimal *dest) {
    dest->bits[0] = src.bits[0];
    dest->bits[1] = src.bits[1];
    dest->bits[2] = src.bits[2];
    dest->bits[3] = src.bits[3];
}

void s21_reset(s21_decimal *value) {
    value->bits[0] = value->bits[1] = value->bits[2] = value->bits[3] = 0;
}

int s21_last_bit(s21_decimal number) {
    int s21_last_bit = 95;
    for (; s21_last_bit >= 0 && s21_get_bit(number, s21_last_bit) == 0; s21_last_bit--) {;}
    return s21_last_bit;
}

int s21_shift_left(s21_decimal *first, int shift) {
    int res_val = 1;
    for (int i = 0; i < shift; i++) {
        if (s21_get_bit(*first, 95)) break;
        int value_31bit = s21_get_bit(*first, 31);
        int value_63bit = s21_get_bit(*first, 63);
        first->bits[0] <<= 1;
        first->bits[1] <<= 1;
        first->bits[2] <<= 1;
        (value_31bit) ? s21_set_bit(first, 32, 1) : s21_set_bit(first, 32, 0);
        (value_63bit) ? s21_set_bit(first, 64, 1) : s21_set_bit(first, 64, 0);
        res_val = 0;
    }
    return res_val;
}

int s21_shift_right(s21_decimal *first, int shift) {
    int res_val = 1;
    for (int i = 0; i < shift; i++) {
        int value_32bit = s21_get_bit(*first, 32);
        int value_64bit = s21_get_bit(*first, 64);
        first->bits[0] >>= 1;
        first->bits[1] >>= 1;
        first->bits[2] >>= 1;
        value_32bit ? s21_set_bit(first, 31, 1) : s21_set_bit(first, 31, 0);
        value_64bit ? s21_set_bit(first, 63, 1) : s21_set_bit(first, 63, 0);
        s21_set_bit(first, 95, 0);
        res_val = 0;
    }
    return res_val;
}
