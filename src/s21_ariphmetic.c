#include "s21_decimal.h"

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int res = 0;
    int res_flag = 0;

    s21_reset(result);
    s21_scale_equalization(&value_1, &value_2);

    int sign1 = s21_get_sign(value_1);
    int sign2 = s21_get_sign(value_2);
    if (sign1 != sign2) {
        if (sign1 == 1) {
            res_flag = 1;
        }
        if (sign2 == 1) {
            res_flag = 2;
        }
    }
    if (res_flag == 0) {
        int transfer = 0;
        for (int j = 0; j < 96 ; j++) {
            int tmp1 = s21_get_bit(value_1, j);
            int tmp2 = s21_get_bit(value_2, j);
            int tmp_res = tmp1 + tmp2 + transfer;
            if (tmp_res == 0) {
                transfer = 0;
                s21_set_bit(result, j, 0);
            } else if (tmp_res == 1) {
                transfer = 0;
                s21_set_bit(result, j, 1);
            } else if (tmp_res == 2) {
                if (j == 95) { res = 1; break; }
                transfer = 1;
                s21_set_bit(result, j, 0);
            } else if (tmp_res == 3) {
                if (j == 95) { res = 1; break; }
                transfer = 1;
                s21_set_bit(result, j, 1);
            }
        }
        s21_set_scale(result, s21_get_scale(value_1));
        if (sign1 == 1) {
            s21_set_sign(result, 1);
        }
    } else if (res_flag == 1) {
        s21_set_sign(&value_1, 0);
        res = s21_sub(value_2, value_1, result);
    } else if (res_flag == 2) {
        s21_set_sign(&value_2, 0);
        res = s21_sub(value_1, value_2, result);
    }
    if (s21_get_scale(*result) > 28 || (s21_get_scale(*result) == 28 &&
        result->bits[0] == 0 && result->bits[1] == 0 && result->bits[2] == 0))
        res = 2;
    if (res != 0) s21_reset(result);
    return res;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int res = 0;
    int res_flag = 0;

    s21_reset(result);
    s21_scale_equalization(&value_1, &value_2);

    int sign1 = s21_get_sign(value_1);
    int sign2 = s21_get_sign(value_2);
    if (sign1 != sign2) {
        if (sign1 == 0) {
            res_flag = 1;
        }
        if (sign2 == 0) {
            res_flag = 2;
        }
    }

    if (res_flag == 0) {
        if (sign1 == 0) {
            if (s21_is_less(value_1, value_2)) {
                s21_set_sign(result, 1);
                s21_decimal t = value_1;
                value_1 = value_2;
                value_2 = t;
            }
            for (int j = 0; j < 96 ; j++) {
                int tmp1 = s21_get_bit(value_1, j);
                int tmp2 = s21_get_bit(value_2, j);
                int tmp_res = tmp1 - tmp2;
                if (tmp_res == 0) {
                    s21_set_bit(result, j, 0);
                    continue;
                } else if (tmp_res == 1) {
                    s21_set_bit(result, j, 1);
                } else if (tmp_res == -1) {
                    int n = j + 1;
                    while (s21_get_bit(value_1, n) != 1) {
                        s21_set_bit(&value_1, n, 1);
                        n++;
                    }
                    s21_set_bit(&value_1, n, 0);
                    s21_set_bit(result, j, 1);
                }
            }
            s21_set_scale(result, s21_get_scale(value_1));
        } else if (sign1 == 1) {
            s21_set_bit(&value_1, 127, 0);
            s21_set_bit(&value_2, 127, 0);
            res = s21_sub(value_2, value_1, result);
        }
    } else if (res_flag == 1) {
        s21_set_bit(&value_2, 127, 0);
        res = s21_add(value_1, value_2, result);
    } else if (res_flag == 2) {
        s21_set_bit(&value_2, 127, 1);
        res = s21_add(value_1, value_2, result);
    }
    if (s21_get_scale(*result) > 28 || (s21_get_scale(*result) == 28 &&
        result->bits[0] == 0 && result->bits[1] == 0 && result->bits[2] == 0))
        res = 2;
    if (res != 0) s21_reset(result);
    return res;
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    int res = 0;
    s21_reset(result);
    s21_decimal tmp = {{0, 0, 0, 0}};
    s21_decimal ten = {{10, 0, 0, 0}};
    s21_decimal remainder = {{0, 0, 0, 0}};
    int sign1 = s21_get_sign(value_1), sign2 = s21_get_sign(value_2);
    s21_set_sign(&value_1, 0); s21_set_sign(&value_2, 0);

    int scale1 = s21_get_scale(value_1), scale2 = s21_get_scale(value_2);
    s21_set_scale(&value_1, 0);
    s21_set_scale(&value_2, 0);

    while (s21_get_scale(value_1) + s21_get_scale(value_2) > 28) {
        if (s21_get_scale(value_1) >= s21_get_scale(value_2) && s21_is_greater(value_1, ten)) {
            int scale_value1 = s21_get_scale(value_1);
            integer_division(value_1, ten, &value_1, &remainder, 1);
            s21_set_scale(&value_1, scale_value1 - 1);
            scale1 = scale_value1 - 1;
        } else {
            if (s21_is_less(value_2, ten)) break;
            int scale_value2 = s21_get_scale(value_2);
            integer_division(value_2, ten, &value_2, &remainder, 1);
            s21_set_scale(&value_2, scale_value2 - 1);
            scale2 = scale_value2 - 1;
        }
    }

    s21_bits_copy(value_1, &tmp);
    int n = s21_last_bit(value_2) + 1;
    if (n < 96) {
        for (int i = 0; i < n; i++) {
            if (s21_get_bit(value_2, i) != 0) {
                res = s21_add(*result, tmp, result);
                if (res != 0) break;
                s21_shift_left(&tmp, 1);
            } else {
                s21_shift_left(&tmp, 1);
            }
        }
        s21_set_scale(result, scale1 + scale2);
    } else {
        res = 1;
    }
    (sign1 != sign2) ? s21_set_sign(result, 1) : NULL;
    if (res != 0 || scale1 + scale2 > 28) s21_reset(result);
    return res;
}

int s21_mod(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    s21_reset(result);
    int ret_val = 0;
    s21_decimal tmp_del = {{0, 0, 0, 0}};
    int s2 = s21_get_sign(value_2), s1 = s21_get_sign(value_1);
    s21_set_sign(&value_1, 0); s21_set_sign(&value_2, 0);
    int scale1 = s21_get_scale(value_1), scale2 = s21_get_scale(value_2);
    if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0) {
        ret_val = 3;
    } else if (s21_get_scale(value_1) > 28 || s21_get_scale(value_2) > 28) {
        ret_val = 2;
    } else if (s21_is_less(value_1, value_2)) {
        if (s1 && !s2) {
            s21_sub(value_2, value_1, &tmp_del);
        } else if (!s1 && s2) {
            s21_sub(value_1, value_2, &tmp_del);
            s21_set_sign(&value_1, s1);
        } else if (s1 && s2 || (!s1 && !s2)) {
            s21_set_sign(&value_1, s1);
            s21_bits_copy(value_1, &tmp_del);
        }
    } else {
        s21_scale_equalization(&value_1, &value_2);
        integer_division(value_1, value_2, result, &tmp_del, 1);
        if (s2 && !s1) {
            s21_sub(value_2, tmp_del, &tmp_del);
            s21_set_sign(&tmp_del, 1);
        } else if (s1 && !s2) {
            s21_set_sign(&tmp_del, 1);
            s21_add(value_2, tmp_del, &tmp_del);
            s21_set_sign(&tmp_del, 0);
        } else if (s1 && s2) {
            s21_set_sign(&tmp_del, 1);
        }
        (scale1 > scale2) ? s21_set_scale(&tmp_del, scale1) : s21_set_scale(&tmp_del, scale2);
    }
    *result = tmp_del;
    if (ret_val != 0) s21_reset(result);
    return ret_val;
}
