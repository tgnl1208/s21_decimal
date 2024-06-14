#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  s21_zero_decimal(dst);
  int res = 0;
  long src1 = src;
  // int  = [−2 147 483 648, +2 147 483 647]
  if (src1 > 2147483647 || src1 < -2147483648) {  // надо проверить диапозоны
    res = 1;
  }
  // char srcChar[33] = {'\0'};
  // sprintf(srcChar, "%d", src);
  // from_charnum_to_decimal(srcChar, dst);
  if (src < 0) set_sign(dst, 1);
  dst->bits[0] = abs(src);
  return res;
}
int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  s21_zero_decimal(dst);
  if (src == (1.0 / 0.0) || src == (-1.0 / 0.0)) {
    return 1;
  } else if ((fabs(src) > 7.9228162514264337593543950335 * powl(10, 28)) ||
             (fabs(src) < 1e-28 && fabs(src) > 0)) {
    s21_zero_decimal(dst);
    return 1;  // вернуть ошибку и значение, равное 0 как вернуть два
               // значения ?
  }

  // (0 < |x| < 1e-28)
  // 0 < abs(x) 0.0000000000000000000000000001
  // char checkZapyatayaSrc[100] = {0};
  // sprintf(checkZapyatayaSrc, "%.28f", src);
  // printf("checChar = %s\n", checkZapyatayaSrc);
  char srcChar[100] = {0};
  char newSrcChar[1000] = {0};
  sprintf(srcChar, "%.28e", fabsf(src));
  int i = 0;
  if (src < 0) {
    set_sign(dst, 1);
  }
  if (!strchr(srcChar, '+')) {
    while (srcChar[i] != 'e') {
      if (srcChar[i] != '.') {
        strncat(newSrcChar, &srcChar[i], 1);
      }
      i++;
    }
    int scale = 0;
    int add_0 = 0;
    int check = 0;
    scale += (srcChar[strlen(srcChar) - 1] - '0') * pow(10, 0);
    scale += (srcChar[strlen(srcChar) - 2] - '0') * pow(10, 1);

    if (srcChar[strlen(srcChar) - 3] == '+') {
      add_0 = scale - 28;
      scale = 28 - scale;
      if (scale < 0) scale = 0;
    } else {
      scale = 28 + scale;
    }
    scale = check == scale ? 0 : scale;
    set_scale(dst, scale);
    //}

    // 2147483648000000000000000000
    if (add_0 > 0)
      strncat(newSrcChar,
              "0000000000000000000000000000000000000000000000000000000", add_0);
    // int scale = 0;
    // scale += (srcChar[strlen(srcChar) - 1] - '0') * pow(10, 0);
    // scale += (srcChar[strlen(srcChar) - 2] - '0') * pow(10, 1);
    // char srcchar_tmp[100] = {0};
    // sprintf(srcchar_tmp, "%g", src);
    // // for (int i = strlen(srcchar_tmp) - 1; i >= 0; i--) {
    // //   if (srcchar_tmp[i] == '0') {
    // //     srcchar_tmp[i] = '\0';
    // //   } else {
    // //     break;
    // //   }
    // // }
    // // int fracDigit = kolvo_fracdigit(srcchar_tmp);
    // printf("s %s\n", srcchar_tmp);
    // src = round(src * powf(10, 5));
    // printf("%g\n", src);
  } else {
    sprintf(newSrcChar, "%.27f", src);
    for (int index = strlen(newSrcChar) - 1; index >= 0; index--) {
      if (newSrcChar[index] == '0') {
        newSrcChar[index] = '\0';
      } else {
        break;
      }
    }
  }

  // if (scale < 28) {
  //   int size = strlen(newSrcChar);
  //   while (newSrcChar[size] == '0' && size >= 0) {
  //     // newSrcChar[size] = '\0';
  //     check++;
  //     size--;
  //   }
  // }

  from_charnum_to_decimal(newSrcChar, dst);
  return 0;
}
// 2147483604019534888960000000
int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int res = 0;
  int scale = set_string_scale(&src);
  int sign = set_string_sign(&src);
  if (src.bits[1] || src.bits[2]) {
    res = 1;
  } else {
    *dst = src.bits[0];
    if (scale > 0 && scale <= 28) {
      *dst /= pow(10, scale);
    }
  }
  if (sign == 1) *dst = *dst * (-1);
  return res;
}
int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  *dst = 0.0;
  double tmp = 0.0;
  int sign = set_string_sign(&src);
  int scale = set_string_scale(&src);

  char charbits2[34] = {'\0'};
  char charbits1[34] = {'\0'};
  char charbits0[34] = {'\0'};
  from_intbits_to_charbits(charbits2, src.bits[2]);
  from_intbits_to_charbits(charbits1, src.bits[1]);
  from_intbits_to_charbits(charbits0, src.bits[0]);
  char binaryStr[97] = {0};
  strncat(binaryStr, charbits2, 32);
  strncat(binaryStr, charbits1, 32);
  strncat(binaryStr, charbits0, 32);
  strreverse(binaryStr);
  for (int i = 0; i < 96; i++) {
    if (binaryStr[i] != '0') {
      tmp += pow(2.0, i);
    }
  }
  double scaleDec = pow(10, scale);
  tmp /= scaleDec;

  // printf("tmpaft %.27f\n", tmp);
  if (sign == 1) {
    tmp *= -1.0;
  }

  *dst = (float)tmp;
  return 0;
}

// int main() {
//   float f = 1.2345678;
//   s21_decimal d = {0};
//   s21_from_float_to_decimal(f, &d);
//   print_decimal(d);
//   return 0;
// }