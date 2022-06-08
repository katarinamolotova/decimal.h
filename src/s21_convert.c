#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
    int ret_val = 0;
    if (dst) {
        s21_reset(dst);
        if (src >= 0)
            dst->bits[0] = src;
        if (src < 0) {
            src = -src;
            dst->bits[0] = src;
            s21_set_sign(dst, 1);
        }
    } else {
        ret_val = 1;
    }
    return ret_val;
}

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
    int res = 0, sign = s21_get_sign(src);
    *dst = 0;
    s21_truncate(src, &src);
    if (src.bits[1] == 0 && src.bits[2] == 0) {
        *dst = src.bits[0];
        if (sign == 1) *dst = -*dst;
    } else {
        res = 1;
    }
    if ((s21_get_bit(src, 31) == 1 && s21_get_sign(src) == 1)) {
        *dst = 0;
        res = 1;
    }
    return res;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
    int ret_val = 1;
    if (dst) {
        s21_reset(dst);
        if (src < 0) s21_set_sign(dst, 1);
        int exp = s21_get_float_exp(&src);
        double temp = (double)fabs(src);
        int scale = 0;
        for (; scale < 28 && (int)temp / (int)pow(2, 22) == 0; temp *= 10, scale++) {;}
        temp = round(temp);
        if (scale <= 28 && (exp > -94 && exp < 96)) {
            for (; fmod(temp, 10) == 0 && scale > 0; scale--, temp /= 10) {;}
            src = (float)temp;
            exp = s21_get_float_exp(&src);
            s21_set_bit(dst, exp, 1);
            for (int i = exp - 1, j = 22; j >= 0; i--, j--)
                if ((*(int*)&src & (1 << j)) != 0)
                    s21_set_bit(dst, i, 1);
            s21_set_scale(dst, scale);
            ret_val = 0;
        }
    }
    return ret_val;
}

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
    int ret_val = 1;
    if (dst != NULL) {
        double temp = 0;
        for (int i = 0; i < 96; i++)
            if (s21_get_bit(src, i))
                temp += pow(2, i);
        int scale = s21_get_scale(src);
        for (int i = scale; i > 0; i--)
            temp /= 10.0;
        if (s21_get_sign(src))
            temp *= -1;
        *dst = (float)temp;
        ret_val = 0;
    }
    return ret_val;
}

void s21_scale_equalization(s21_decimal* first, s21_decimal* second) {
    s21_decimal *bigger = NULL, *smaller = NULL;
    s21_decimal remainder = {{0, 0, 0, 0}};
    if (s21_get_scale(*first) > s21_get_scale(*second)) {
        bigger = first; smaller = second;
    } else {
        bigger = second; smaller = first;
    }
    if (s21_get_scale(*first) != s21_get_scale(*second)) {
        int sign1 = s21_get_sign(*bigger), sign2 = s21_get_sign(*smaller);
        s21_decimal ten = {{10, 0, 0, 0}};
        s21_set_sign(bigger, 0); s21_set_sign(smaller, 0);
        while (s21_get_scale(*smaller) != s21_get_scale(*bigger)
            && s21_last_bit(*smaller) < 93
            && s21_get_scale(*smaller) <= 28) {
            int flagInf = 0;
            int scaleSmall = s21_get_scale(*smaller);
            s21_set_scale(smaller, 0);
            flagInf = s21_mul(ten, *smaller, smaller);
            if (flagInf != 0) break;
            s21_set_scale(smaller, scaleSmall + 1);
        }
        while (s21_get_scale(*smaller) != s21_get_scale(*bigger)) {
            int flagInf = 0;
            int scaleBig = s21_get_scale(*bigger);
            if (s21_get_scale(*bigger) - s21_get_scale(*smaller) == 1) {
                if (bigger->bits[0] >= 5 && bigger->bits[0] < 10) {
                    bigger->bits[0] = 1;
                    s21_set_scale(bigger, scaleBig - 1);
                    break;
                }
            }
            flagInf = integer_division(*bigger, ten, bigger, &remainder, 1);
            if (flagInf != 0) break;
            s21_set_scale(bigger, scaleBig - 1);
        }
        s21_set_sign(bigger, sign1); s21_set_sign(smaller, sign2);
    }
}
