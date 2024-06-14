#include "s21_decimal.h"

int comporator(s21_decimal value_1, s21_decimal value_2) {
  // 0 - ravna 1- val1 bolshe 2- val2 bolsh
  int res = 0;
  int scaleValue1 = set_string_scale(&value_1);
  int scaleValue2 = set_string_scale(&value_2);
  int toScale = scaleValue1 - scaleValue2;
  int sign1 = set_string_sign(&value_1);
  int sign2 = set_string_sign(&value_2);
  if (value_1.bits[0] == 0 && value_1.bits[1] == 0 && value_1.bits[2] == 0 &&
      value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0) {
    res = 0;
  } else {
    if (sign1 > set_string_sign(&value_2)) {
      res = 2;
    } else if (sign1 < sign2) {
      res = 1;
    } else if (sign1 == sign2) {
      if (toScale > 0) {
        normalizeTo(&value_2, toScale);
      } else if (toScale < 0) {
        normalizeTo(&value_1, -toScale);
      }
      // int step = 2147483648;
      for (int i = 2; i >= 0; i--) {
        if (value_1.bits[i] > value_2.bits[i]) {
          if (sign1 == 0) {
            res = 1;
          } else {
            res = 2;
          }
          break;
        } else if (value_1.bits[i] < value_2.bits[i]) {
          if (sign1 == 0) {
            res = 2;
          } else {
            res = 1;
          }
          break;
        }
      }
    }
  }
  return res;
}

int s21_is_less(s21_decimal value_1, s21_decimal value_2) {
  return comporator(value_1, value_2) == 2 ? 1 : 0;
}
int s21_is_less_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int res = comporator(value_1, value_2);
  return res == 2 || res == 0 ? 1 : 0;
}
int s21_is_greater(s21_decimal value_1, s21_decimal value_2) {
  int res = comporator(value_1, value_2);
  return res == 1 ? 1 : 0;
}
int s21_is_greater_or_equal(s21_decimal value_1, s21_decimal value_2) {
  int res = comporator(value_1, value_2);
  return res == 1 || res == 0 ? 1 : 0;
}
int s21_is_equal(s21_decimal value_1, s21_decimal value_2) {
  return comporator(value_1, value_2) == 0 ? 1 : 0;
}
int s21_is_not_equal(s21_decimal value_1, s21_decimal value_2) {
  int res = comporator(value_1, value_2);
  return res == 1 || res == 2 ? 1 : 0;
}

// 0111111111111111
// 0000000000000000

// int main() {
//   float num1 = 1.375342323523;
//   float num2 = 1.39;
//   s21_decimal dec1 = {0}, dec2 = {0};
//   s21_from_float_to_decimal(num1, &dec1);
//   s21_from_float_to_decimal(num2, &dec2);
//   char d1[100];
//   char d2[100];
//   from_decimal_to_charnum(d1, &dec1);
//   printf("%s\n", d1);
//   normalizeTo(&dec1, 3);
//   from_decimal_to_charnum(d1, &dec1);
//   printf("%s\n", d1);
//   from_decimal_to_charnum(d2, &dec2);

//   printf("%s\n", d2);
//   print_decimal(dec1);
//   print_decimal(dec2);
//   // int res = s21_is_less(dec1, dec2);
//   return 0;
// }