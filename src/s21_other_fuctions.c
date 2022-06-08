#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
    int ret_val = 1;
    if (result != NULL) {
        s21_reset(result);
        s21_decimal one = {{1, 0, 0, 0}};
        s21_truncate(value, &value);
        if (s21_get_sign(value))
            s21_sub(value, one, &value);
        *result = value;
        ret_val = 0;
    }
    return ret_val;
}

int s21_round(s21_decimal value, s21_decimal *result) {
    int ret_val = 1;
    if (result != NULL) {
        s21_reset(result);
        s21_decimal one = {{1, 0, 0, 0}};
        s21_decimal five = {{5, 0, 0, 0}};
        s21_set_scale(&five, 1);

        int sign = s21_get_sign(value);
        s21_set_sign(&value, 0);

        s21_decimal trunc = {{0, 0, 0, 0}};
        s21_truncate(value, &trunc);
        s21_decimal buffer = {{0, 0, 0, 0}};
        s21_sub(value, trunc, &buffer);
        *result = trunc;

        if (s21_is_greater_or_equal(buffer, five))
            s21_add(*result, one, result);
        s21_set_sign(result, sign);
        ret_val = 0;
    }
    return ret_val;
}

int s21_truncate(s21_decimal value, s21_decimal *result) {
    int ret_val = 1;
    if (result != NULL) {
        s21_decimal ten = {{10, 0, 0, 0}};
        s21_decimal remainder = {{0, 0, 0, 0}};
        int scale = s21_get_scale(value), sign = s21_get_sign(value);
        s21_set_sign(&value, 0);
        s21_set_scale(&value, 0);
        s21_bits_copy(value, result);
        for (int i = scale; i > 0; i--) {
            integer_division(value, ten, result, &remainder, 1);
            value = *result;
        }
        s21_set_sign(result, sign);
        s21_set_scale(result, 0);
        ret_val = 0;
    }
    return ret_val;
}

int s21_negate(s21_decimal value, s21_decimal *result) {
    int ret_val = 1;
    if (result != NULL) {
        s21_bits_copy(value, result);
        s21_get_sign(value) ? s21_set_sign(result, 0) : s21_set_sign(result, 1);
        ret_val = 0;
    }
    return ret_val;
}
