#include "s21_decimal.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  s21_zero_decimal(result);
  char valueChar[100] = {0};
  from_decimal_to_charnum(valueChar, &value);
  int intDigit;
  int flag = 0;
  if (valueChar[0] == '-') {
    intDigit = kolvo_digit(valueChar) + 1;
    int index = intDigit;
    while (valueChar[index] != '\0') {
      if (valueChar[index] != '0') flag = 1;
      index++;
    }
    if (flag == 0) {
      s21_truncate(value, result);
    } else {
      valueChar[intDigit - 1] = (char)valueChar[intDigit - 1] + 1;
      valueChar[intDigit] = '\0';
      from_charnum_to_decimal(valueChar, result);
    }
  } else {
    s21_truncate(value, result);
  }
  return 0;
}
int s21_round(s21_decimal value, s21_decimal *result) {
  s21_zero_decimal(result);
  char valueChar[100] = {0};
  from_decimal_to_charnum(valueChar, &value);
  int intDigit;
  if (valueChar[0] == '-') {
    intDigit = kolvo_digit(valueChar) + 1;
  } else {
    intDigit = kolvo_digit(valueChar);
  }
  if (valueChar[intDigit + 1] >= '5') {
    valueChar[intDigit - 1] = (char)valueChar[intDigit - 1] + 1;
    valueChar[intDigit] = '\0';
    from_charnum_to_decimal(valueChar, result);
  } else {
    s21_truncate(value, result);
  }
  return 0;
}
int s21_truncate(s21_decimal value, s21_decimal *result) {
  s21_zero_decimal(result);
  char valueChar[100] = {0};
  from_decimal_to_charnum(valueChar, &value);
  int intDigit = kolvo_digit(valueChar);
  char intValueChar[100] = {0};
  strncpy(intValueChar, valueChar, intDigit + 1);
  from_charnum_to_decimal(intValueChar, result);
  return 0;
}
int s21_negate(s21_decimal value, s21_decimal *result) {
  s21_zero_decimal(result);
  int sign = value.bits[3] >> 31;
  result->bits[0] = value.bits[0];
  result->bits[1] = value.bits[1];
  result->bits[2] = value.bits[2];
  result->bits[3] = value.bits[3];
  if (sign == 0) {
    result->bits[3] |= 0x80000000;  // 10000000000000000000000000000000
  } else {
    result->bits[3] &= 0x7FFFFFFF;  // 01111111111111111111111111111111
  }
  return 0;
}