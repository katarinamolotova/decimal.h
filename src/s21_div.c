#include "s21_decimal.h"

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
    s21_decimal remainder = {{1, 0, 0, 0}};
    s21_decimal tmp_res = {{0, 0, 0, 0}};
    int ret_val = 0, count = 0;
    int scale1 = s21_get_scale(value_1), scale2 = s21_get_scale(value_2), scale = 0;
    int sign1 = s21_get_sign(value_1), sign2 = s21_get_sign(value_2);
    s21_set_sign(&value_1, 0); s21_set_sign(&value_2, 0);
    (scale1 >= scale2) ? scale = scale1 - scale2 : s21_scale_equalization(&value_1, &value_2);
    if (s21_is_greater(value_2, value_1) && scale == 0) {
        s21_decimal ten = {{10, 0, 0, 0}};
        while (s21_is_greater(value_2, value_1)) {
            s21_mul(value_1, ten, &value_1);
            scale++;
            if (scale == 28) break;
        }
    }
    s21_set_scale(&value_1, scale);
    while (count < 10) {
        *result = tmp_res;
        ret_val = integer_division(value_1, value_2, &tmp_res, &remainder, 0);
        if (s21_get_scale(tmp_res) == 0 && s21_last_bit(tmp_res) > 93) { ret_val = 1; break; }
        s21_add(tmp_res, *result, &tmp_res);
        if (remainder.bits[0] == 0 && remainder.bits[1] == 0 && remainder.bits[2] == 0) break;
        value_1 = remainder;
        s21_set_scale(&value_1, scale + count);
        count++;
    }
    *result = tmp_res;
    (sign1 != sign2) ? s21_set_sign(result, 1) : NULL;
    if (ret_val != 0) s21_reset(result);
    return ret_val;
}

int integer_division(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
    s21_decimal *remainder, int flagInt) {
    s21_decimal tmp_div = {{0, 0, 0, 0}};
    s21_decimal tmp_del = {{0, 0, 0, 0}};
    s21_decimal tmp_mod = {{0, 0, 0, 0}};
    s21_decimal tmp_res = {{0, 0, 0, 0}};
    s21_decimal tmp_buf = {{0, 0, 0, 0}};

    int scale_c = s21_get_scale(value_1);
    int index_res = 95, ret_val = 0, discharge = 1;

    int sign1 = s21_get_sign(value_1), sign2 = s21_get_sign(value_2);
    int scale1 = s21_get_scale(value_1), scale2 = s21_get_scale(value_2);
    s21_bits_copy(value_1, &tmp_div);

    s21_set_scale(&tmp_div, 0);
    s21_set_scale(&value_2, 0);

    s21_set_sign(&tmp_div, 0);
    s21_set_sign(&value_2, 0);

    if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0) {
        ret_val = 3;
    } else if ((value_1.bits[0] == 0 && value_1.bits[1] == 0 && value_1.bits[2] == 0)
        || scale1 > 28 || scale2 > 28) {
        ret_val = 2;
    } else {
        if (s21_is_greater_or_equal(tmp_div, value_2) && flagInt)  {
            first_prepare(tmp_div, &tmp_mod, &tmp_del, value_2, &discharge);
        } else if (!flagInt) {
            first_step(&tmp_div, value_2, &scale_c, &tmp_res, &index_res);
            first_prepare(tmp_div, &tmp_mod, &tmp_del, value_2, &discharge);
        }
        while (discharge != -1 && index_res >= 0 && scale_c <= 28) {
            if (s21_is_greater_or_equal(tmp_del, value_2)) {
                ret_val = s21_sub(tmp_del, value_2, &tmp_buf);
                s21_set_bit(&tmp_res, index_res, 1);
                index_res--;
                blue_electrical_tape(tmp_buf, &tmp_del, &tmp_mod, &discharge);
            } else {
                blue_electrical_tape(tmp_del, &tmp_del, &tmp_mod, &discharge);
                s21_set_bit(&tmp_res, index_res, 0);
                index_res--;
            }
        }
        s21_reset(result);
        for (int i = 95; i > index_res; i--) {
            s21_get_bit(tmp_res, i) == 1 ? s21_set_bit(result, i - index_res - 1, 1) :
            s21_set_bit(result, i - index_res - 1, 0);
        }
        (sign1 != sign2) ? s21_set_sign(result, 1) : NULL;
        s21_set_scale(result, scale_c);
        s21_shift_right(&tmp_del, 1);
        *remainder = tmp_del;
    }
    if (ret_val != 0) s21_reset(result);
    return ret_val;
}

void blue_electrical_tape(s21_decimal tmp_buf, s21_decimal *tmp_div, s21_decimal* tmp_mod, int* discharge) {
    s21_bits_copy(tmp_buf, tmp_div);
    s21_shift_left(tmp_div, 1);
    s21_get_bit(*tmp_mod, *discharge - 1) == 1 ? s21_set_bit(tmp_div, 0, 1) : s21_set_bit(tmp_div, 0, 0);
    s21_set_bit(tmp_mod, *discharge - 1, 0);
    (*discharge)--;
}

void first_step(s21_decimal *tmp_div, s21_decimal value_2, int * scale_c, s21_decimal * tmp_res,
    int * index_res) {
    s21_decimal ten = {{10, 0, 0, 0}};
    int x = s21_is_greater_or_equal(*tmp_div, value_2);
    while (x != 1) {
        s21_set_bit(tmp_res, *index_res, 0);
        (*index_res)--;
        s21_mul(*tmp_div, ten, tmp_div);
        (*scale_c)++;
        x = s21_is_greater_or_equal(*tmp_div, value_2);
    }
}

void first_prepare(s21_decimal tmp_div, s21_decimal *tmp_mod, s21_decimal *tmp_del,
    s21_decimal value_2, int * discharge) {
    *discharge = 0;
    int shift = s21_last_bit(tmp_div) - s21_last_bit(value_2);
    int n = 0;
    while (1) {
        s21_bits_copy(tmp_div, tmp_del);
        s21_shift_right(tmp_del, shift - n);
        if (s21_is_greater_or_equal(*tmp_del, value_2) == 1) {
            break;
        } else {
            n++;
        }
    }

    s21_bits_copy(tmp_div, tmp_mod);
    *discharge = s21_last_bit(tmp_div) - s21_last_bit(*tmp_del);
    for (int i = 95; i > s21_last_bit(tmp_div) - s21_last_bit(*tmp_del) - 1; i--) {
        s21_set_bit(tmp_mod, i, 0);
    }
}
