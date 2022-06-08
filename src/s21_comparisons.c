#include "s21_decimal.h"

int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
    int flag = 0;
    s21_scale_equalization(&value_1, &value_2);
    if (s21_get_sign(value_1) == s21_get_sign(value_2)) {
        int all_fine = 0;
        for (int i = 95; i >= 0; i--) {
            if (s21_get_bit(value_1, i) == s21_get_bit(value_2, i)) {
                continue;
            } else if (s21_get_bit(value_1, i) > s21_get_bit(value_2, i)) {
                all_fine = (s21_get_sign(value_1) == 0) ? 1 : 0;
                break;
            } else if (s21_get_bit(value_1, i) < s21_get_bit(value_2, i)) {
                all_fine = (s21_get_sign(value_1) == 0) ? 0 : 1;
                break;
            }
        }
        flag = (all_fine == 1) ? 1 : 0;
    } else if (s21_get_sign(value_1) < s21_get_sign(value_2)) {
        flag = 1;
    }
    return flag;
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
    int flag = 0;
    s21_scale_equalization(&value_1, &value_2);
    if (s21_get_sign(value_1) == s21_get_sign(value_2)) {
        int all_fine = 0;
        for (int i = 95; i >= 0; i--) {
            if (s21_get_bit(value_1, i) == s21_get_bit(value_2, i)) {
                continue;
            } else if (s21_get_bit(value_1, i) > s21_get_bit(value_2, i)) {
                all_fine = (s21_get_sign(value_1) == 0) ? 0 : 1;
                break;
            } else if (s21_get_bit(value_1, i) < s21_get_bit(value_2, i)) {
                all_fine = (s21_get_sign(value_1) == 0) ? 1 : 0;
                break;
            }
        }
        flag = (all_fine == 1) ? 1 : 0;
    } else if (s21_get_sign(value_1) > s21_get_sign(value_2)) {
        flag = 1;
    }
    return flag;
}

int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
    int flag = 1;
    s21_scale_equalization(&value_1, &value_2);
    if (s21_get_sign(value_1) == s21_get_sign(value_2)) {
        for (int i = 95; i >= 0; i--) {
            if ((s21_get_bit(value_1, i) == s21_get_bit(value_2, i)) && flag) {
            } else {
                flag = 0;
                break;
            }
        }
    } else {
        flag = 0;
    }
    return flag;
}

int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
    return (!s21_is_equal(value_1, value_2));
}

int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
    return (s21_is_equal (value_1, value_2) == 1 || s21_is_less (value_1, value_2) == 1) ? 1 : 0;
}

int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
    return (s21_is_equal (value_1, value_2) == 1 || s21_is_greater (value_1, value_2) == 1) ? 1 : 0;
}
