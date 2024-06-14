#include "s21_decimal.h"

int s21_bitAdd(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int res = 0;  // 0 - норма, 1 - переполнение
  s21_zero_decimal(result);
  int step = 1, inMind = 0, val1step = 0, val2step = 0;
  for (int i = 0; i < 3; i++) {
    while (step != 0) {
      val1step = value_1.bits[i] & step;
      val2step = value_2.bits[i] & step;

      result->bits[i] = result->bits[i] ^ val1step;
      result->bits[i] = result->bits[i] ^ val2step;
      result->bits[i] = result->bits[i] ^ inMind;

      if ((inMind == step && (val1step == step || val2step == step)) ||
          (val1step == step && val2step == step)) {
        if (step == 1 << 31) {
          inMind = 1;
        } else {
          inMind = step << 1;
        }
      } else {
        inMind = 0;
      }
      step = step << 1;
    }
    step = 1;
  }
  if (inMind) {
    res = 1;
  }
  return res;
}

int normalizeTo(s21_decimal *res, int to) {  // без учета переполнения
  int result = 0;
  int currentScale = set_string_scale(res);
  if (to > 0) {
    for (int j = 0; j < to; j++) {
      s21_decimal val1 = *res;
      s21_decimal val2 = *res;
      for (int i = 2; i >= 0; i--) {
        val1.bits[i] = val1.bits[i] << 1;
        if (i != 0) {
          val1.bits[i] |= val1.bits[i - 1] >> 31;
        }
      }
      for (int i = 2; i >= 0; i--) {
        val2.bits[i] = val2.bits[i] << 3;
        if (i != 0) {
          val2.bits[i] |= val2.bits[i - 1] >> 29;
        }
      }
      result = s21_bitAdd(val1, val2, res);
    }
  } else if (to < 0) {
    for (int j = 0; j < -to; j++) {
      s21_bitDiv(*res, (s21_decimal){{10, 0, 0, 0}}, res, false);
    }
  }
  set_scale(res, to + currentScale);
  return result;
}
int s21_bitRevers(s21_decimal *value_1, int mask_size) {
  int first_one = 0;
  int index = 0;
  if (mask_size == 0) {
    for (int i = 2; i >= 0; i--) {
      index = 31;
      while ((value_1->bits[i] >> index) & (1 == 0) && index != 0) {
        index--;
      }
      if (index != 0) {
        first_one = i;
        break;
      }
    }
  } else {
    index = mask_size % 32;
    first_one = mask_size / 32;
  }
  int sdvig = 31 - index;
  value_1->bits[first_one] = value_1->bits[first_one] << sdvig;
  value_1->bits[first_one] = ~(value_1->bits[first_one]);
  value_1->bits[first_one] = value_1->bits[first_one] >> sdvig;
  first_one--;
  while (first_one >= 0) {
    value_1->bits[first_one] = ~(value_1->bits[first_one]);
    index += 32;
    first_one--;
  }
  return index;
}

int s21_bitSub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_zero_decimal(result);
  int res = 0;
  int mask_size = s21_bitRevers(&value_1, 0);
  res = s21_bitAdd(value_2, value_1, result);
  s21_bitRevers(result, mask_size);
  return res;
}

int s21_forSubAndAdd(s21_decimal value_1, s21_decimal value_2,
                     s21_decimal *result, bool isSub) {
  s21_zero_decimal(result);
  int res = 0;

  int setScale = 0;
  int sign1 = set_string_sign(&value_1);
  int sign2 = set_string_sign(&value_2);
  int scaleValue1 = set_string_scale(&value_1);
  int scaleValue2 = set_string_scale(&value_2);
  int toScale = scaleValue1 - scaleValue2;
  if (toScale > 0) {
    s21_decimal value = value_2;
    res = normalizeTo(&value, toScale);
    if (res == 0) {
      setScale = scaleValue1;
      value_2 = value;
    } else {
      setScale = scaleValue2;
      res = normalizeTo(&value_1, -toScale);
      if (isSub) {
        s21_bitAdd(value_1, (s21_decimal){{1, 0, 0, 0}}, &value_1);
      }
    }
  } else if (toScale < 0) {
    s21_decimal value = value_1;
    res = normalizeTo(&value, -toScale);
    if (res == 0) {
      setScale = scaleValue2;
      value_1 = value;
    } else {
      setScale = scaleValue1;
      res = normalizeTo(&value_2, toScale);
      if (isSub) {
        s21_bitAdd(value_2, (s21_decimal){{1, 0, 0, 0}}, &value_2);
      }
    }
  } else {
    setScale = scaleValue1;
  }
  int setSign = sign1;
  if (sign1 == sign2) {
    setSign = sign1;
    res = s21_bitAdd(value_1, value_2, result);
  } else {
    value_1.bits[3] = value_1.bits[3] & 0b01111111111111111111111111111111;
    value_2.bits[3] = value_2.bits[3] & 0b01111111111111111111111111111111;
    if (!isSub) {
      if (s21_is_equal(value_1, (s21_decimal){{UINT_MAX, UINT_MAX, UINT_MAX,
                                               value_1.bits[3]}}) &&
          s21_is_equal(value_2, (s21_decimal){{0, 0, 0, value_2.bits[3]}})) {
        value_1.bits[0] = value_1.bits[0] << 1;
      } else if (s21_is_equal(value_2,
                              (s21_decimal){{UINT_MAX, UINT_MAX, UINT_MAX,
                                             value_2.bits[3]}}) &&
                 s21_is_equal(value_1,
                              (s21_decimal){{0, 0, 0, value_1.bits[3]}})) {
        value_2.bits[0] = value_2.bits[0] << 1;
      }
    }
    if (s21_is_greater_or_equal(value_1, value_2) == 1) {
      setSign = sign1;
      res = s21_bitSub(value_1, value_2, result);
    } else {
      setSign = sign2;
      res = s21_bitSub(value_2, value_1, result);
    }
  }
  set_sign(result, setSign);
  set_scale(result, setScale);

  if (result->bits[0] == 0 && result->bits[1] == 0 && result->bits[2] == 0) {
    s21_zero_decimal(result);
  }
  if (res == 1) {
    s21_zero_decimal(result);
    if (setSign == 1) {
      res = 2;
    }
  }

  return res;
}

int moveBitsTo(s21_decimal *result, int toMove) {
  int res = 0;
  if (toMove > 0) {
    for (int i = 0; i < toMove; i++) {
      if (result->bits[2] >> 31 == 1) {
        res = 1;
      }
      result->bits[2] = result->bits[2] << 1 | (result->bits[1] >> 31);
      result->bits[1] = result->bits[1] << 1 | (result->bits[0] >> 31);
      result->bits[0] = result->bits[0] << 1;
    }
  } else if (toMove < 0) {
    for (int i = -toMove; i > 0; i--) {
      result->bits[0] = result->bits[0] >> 1 | (result->bits[1] << 31);
      result->bits[1] = result->bits[1] >> 1 | (result->bits[2] << 31);
      result->bits[2] = result->bits[2] >> 1;
    }
  }
  return res;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  int sign2 = set_string_sign(&value_2);
  value_2.bits[3] &= 0x7FFFFFFF;
  set_sign(&value_2, sign2 == 0 ? 1 : 0);
  return s21_forSubAndAdd(value_1, value_2, result, true);
}

int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  return s21_forSubAndAdd(value_1, value_2, result, false);
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_zero_decimal(result);
  int res = 0;

  int scaleValue1 = set_string_scale(&value_1);
  int scaleValue2 = set_string_scale(&value_2);

  int sign1 = set_string_sign(&value_1);
  int sign2 = set_string_sign(&value_2);
  int setSign = sign1 == sign2 ? 0 : 1;
  int index = 0;
  for (int i = 0; i < 3; i++) {
    while (index <= 31) {
      if (((value_2.bits[i] >> index) & 1) == 1) {
        s21_decimal value_1_toMove = value_1;
        res = moveBitsTo(&value_1_toMove, index + i * 32);
        if (res != 1) {
          res = s21_bitAdd(*result, value_1_toMove, result);
          if (res == 1) {
            s21_zero_decimal(result);
            if (setSign == 1) {
              res = 2;
            }
            break;
          }
        } else {
          s21_zero_decimal(result);
          if (setSign == 1) {
            res = 2;
          }
          break;
        }
      }
      index++;
    }
    if (res == 1 || res == 2) {
      break;
    }
    index = 0;
  }
  if (res != 1 && res != 2) {
    set_sign(result, setSign);
    set_scale(result, scaleValue1 + scaleValue2);
  }
  return res;
}
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  s21_zero_decimal(result);
  int res = 0;

  if (value_2.bits[0] == 0 && value_2.bits[1] == 0 && value_2.bits[2] == 0) {
    res = 3;
  } else {
    int sign1 = set_string_sign(&value_1);
    int sign2 = set_string_sign(&value_2);
    int setSign = sign1 == sign2 ? 0 : 1;

    int scaleDifference =
        set_string_scale(&value_1) - set_string_scale(&value_2);
    if (scaleDifference < 0) {
      res = normalizeTo(&value_1, -scaleDifference);
    }
    value_1.bits[3] &= 0x7FFFFFFF;
    value_2.bits[3] &= 0x7FFFFFFF;
    if (s21_is_greater_or_equal(value_2, value_1)) {
      res = normalizeTo(&value_1, 1);
    }
    s21_decimal doubleValue;
    s21_bitAdd(value_1, value_1, &doubleValue);
    if (s21_is_greater_or_equal(value_2, doubleValue)) {
      res = normalizeTo(&value_1, 1);
    }
    int scaleValue1 = s21_bitDiv(value_1, value_2, result, true);
    set_sign(result, setSign);
    int scaleValue2 = set_string_scale(&value_2);
    set_scale(result, scaleValue1 - scaleValue2);
  }
  return res;
}

int getFirstOne(s21_decimal dec) {
  int first_one = 0;
  int index = 0;
  int founded = 0;
  for (int i = 2; i >= 0; i--) {
    index = 31;
    while (index != 0) {
      if (((dec.bits[i] >> index) & 1) != 0) {
        founded = 1;
        break;
      }
      index--;
    }
    if (founded) {
      first_one = index + i * 32;
      break;
    }
  }
  return first_one;
}

int s21_bitDiv(s21_decimal value_1, s21_decimal value_2, s21_decimal *result,
               bool isDiv) {
  s21_zero_decimal(result);
  int scale = set_string_scale(&value_1);
  value_1.bits[3] &= 0x0000;
  value_2.bits[3] &= 0x0000;
  int res = scale;

  int oneFirst_one = getFirstOne(value_1);
  int twoFirst_one = getFirstOne(value_2);
  int toMove = oneFirst_one - twoFirst_one;
  if (toMove >= 0) {
    s21_decimal origDenominator1 = value_1;
    s21_decimal origDenominator2 = value_2;
    moveBitsTo(&value_2, toMove);
    while (s21_is_greater_or_equal(value_2, origDenominator2)) {
      if (s21_is_greater_or_equal(value_1, value_2)) {
        s21_bitSub(value_1, value_2, &value_1);
        moveBitsTo(result, 1);
        result->bits[0] |= 1;
      } else {
        moveBitsTo(result, 1);
      }
      moveBitsTo(&value_2, -1);
    }
    if (isDiv) {
      if (s21_is_not_equal(value_1, (s21_decimal){{0, 0, 0, 0}})) {
        normalizeTo(&origDenominator1, 1);
        origDenominator1.bits[3] &= 0x0000;
        set_scale(&origDenominator1, scale + 1);
        res = s21_bitDiv(origDenominator1, origDenominator2, result, true);
      }
    }
  }
  return res;
}

// int main() {
//   s21_decimal value_1 = {{1, 0, 0, 0}};
//   set_sign(&value_1, 1);
//   s21_decimal value_2 = {{8, 0, 0, 0}};
//   set_sign(&value_2, 1);
//   s21_decimal result = {{0, 0, 0, 0}};
//   s21_decimal check = {{125, 0, 0, 0}};
//   set_scale(&check, 2);
//   int return_value = s21_div(value_1, value_2, &result);
//   print_decimal(check);
//   print_decimal(result);
// }

// int main() {
//   s21_decimal value_1 = {{0x88888888, 0x88888888, 0x88888888, 0}};
//   s21_decimal value_2 = {{0x2, 0, 0, 0}};
//   set_sign(&value_2, 1);
//   s21_decimal result = {{0, 0, 0, 0}};
//   s21_decimal check = {{0x44444444, 0x44444444, 0x44444444, 0}};
//   set_sign(&check, 1);

//   printf("val1\n");
//   print_decimal(value_1);
//   printf("val2\n");
//   print_decimal(value_2);
//   printf("need check\n");
//   print_decimal(check);
//   char v1[100], v2[100], ch[100], res[100];
//   int return_value = s21_div(value_1, value_2, &result);
//   printf("actual result\n");
//   print_decimal(result);
//   from_decimal_to_charnum(v1, &value_1);
//   from_decimal_to_charnum(v2, &value_2);
//   from_decimal_to_charnum(ch, &check);
//   from_decimal_to_charnum(res, &result);
//   printf("v1 = %s\n", v1);
//   printf("v2 = %s\n", v2);
//   printf("ch = %s\n", ch);
//   printf("res = %s\n", res);
// }