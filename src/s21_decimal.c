#include "s21_decimal.h"

// void print_binary(int number) {
//   for (int i = 31; i >= 0; i--) {
//     printf("%d", (number >> i) & 1);
//   }
//   printf("\n");
// }

// void print_decimal(s21_decimal number) {
//   for (int i = 3; i >= 0; i--) {
//     printf("bits[%d] = ", i);
//     print_binary(number.bits[i]);
//   }
//   printf("\n");
// }

void set_sign(s21_decimal *num, int sign) { num->bits[3] |= sign << 31; }

int set_string_sign(s21_decimal *num) {
  int sign = num->bits[3] >> 31;
  return sign;
}

void set_scale(s21_decimal *num, int scale) { num->bits[3] |= scale << 16; }

int set_string_scale(s21_decimal *num) {
  int scale = num->bits[3] >> 16 & 0b00000000000000000000000011111111;
  return scale;
}
void s21_set_bit(s21_decimal *dst, int index, int bit) {
  int mask = 1u << (index % 32);
  if (bit == 0)
    dst->bits[index / 32] = dst->bits[index / 32] & ~mask;
  else
    dst->bits[index / 32] = dst->bits[index / 32] | mask;
}
char *strreverse(char *str) {
  if (!str) return NULL;

  int len = strlen(str);
  char *end = str + len - 1;

  for (int i = 0; i < len / 2; i++) {
    char temp = str[i];
    str[i] = *end;
    *end = temp;
    end--;
  }

  return str;
}
char *divide_by_two(const char *decimal_str) {
  int length = strlen(decimal_str);
  char *result = (char *)malloc(length + 1);  // +1 for null terminator
  int carry = 0;
  int index = 0;
  int digit;

  for (int i = 0; i < length; i++) {
    digit = carry * 10 + (decimal_str[i] - '0');
    result[index++] = (digit / 2) + '0';
    carry = digit % 2;
  }
  result[index] = '\0';

  // Удаляем ведущие нули
  index = 0;
  while (result[index] == '0' && index < length - 1) {
    index++;
  }

  // char* trimmed_result = strdup(result + index);
  char *trimmed_result =
      (char *)malloc(sizeof(char) * (strlen(result) - index + 1));
  strcpy(trimmed_result, result + index);
  free(result);

  return trimmed_result;
}

// Функция для преобразования большого десятичного числа в двоичное
char *decimal_to_binary(const char *decimal_str) {
  char *binary_str = (char *)malloc(1);
  strcpy(binary_str, "");
  // char* temp_decimal_str = strdup(decimal_str);
  char *temp_decimal_str =
      (char *)malloc(sizeof(char) * (strlen(decimal_str) + 1));
  strcpy(temp_decimal_str, decimal_str);

  while (strcmp(temp_decimal_str, "0") != 0) {
    int length = strlen(temp_decimal_str);
    // Добавляем бит к двоичной строке
    char *new_binary_str = (char *)malloc(
        strlen(binary_str) + 2);  // +1 for new bit, +1 for null terminator
    sprintf(new_binary_str, "%d%s", (temp_decimal_str[length - 1] - '0') % 2,
            binary_str);
    free(binary_str);
    binary_str = new_binary_str;

    // Делим десятичное число на 2
    char *new_temp_decimal_str = divide_by_two(temp_decimal_str);
    free(temp_decimal_str);
    temp_decimal_str = new_temp_decimal_str;
  }

  free(temp_decimal_str);
  return binary_str;
}
unsigned long long int from_charbits_to_intbits(char *charbits) {
  int i = 0;
  unsigned long long int value = 0;
  strreverse(charbits);
  while (charbits[i] != '\0') {
    value += pow(2, i) * ((char)charbits[i] - '0');
    i++;
  }
  return value;
}
void remove_char(char *str, char char_to_remove) {
  char *read = str;
  char *write = str;

  while (*read) {
    if (*read != char_to_remove) {
      *write++ = *read;
    }
    read++;
  }
  *write = '\0';
}
void from_charnum_to_decimal(char *numChar, s21_decimal *num) {
  int zapyataya_number = 0;
  while (numChar[zapyataya_number] != '\0') {
    if (numChar[zapyataya_number] == '.') {
      break;
    }
    zapyataya_number++;
  }
  if (zapyataya_number == (int)strlen(numChar)) {
    zapyataya_number = 0;
  } else {
    zapyataya_number = strlen(numChar) - zapyataya_number - 1;
    remove_char(numChar, '.');
  }
  set_scale(num, zapyataya_number);

  if (numChar[0] == '-') {
    set_sign(num, 1);
    remove_char(numChar, '-');
  }
  char *binary_str = decimal_to_binary(numChar);
  int raznitsa = 96 - strlen(binary_str);
  strreverse(binary_str);
  binary_str = realloc(binary_str, sizeof(char) * 97);
  strncat(binary_str,
          "00000000000000000000000000000000000000000000000000000000000000000000"
          "0000000000000000000000000000000000000000000000000000000000",
          raznitsa);
  strreverse(binary_str);
  char charbits2[33] = {0};
  char charbits1[33] = {0};
  char charbits0[33] = {0};
  strncpy(charbits2, binary_str, 32);
  charbits2[32] = '\0';
  strncpy(charbits1, binary_str + 32, 32);
  charbits1[32] = '\0';
  strncpy(charbits0, binary_str + 64, 32);
  charbits0[32] = '\0';
  free(binary_str);
  num->bits[2] = from_charbits_to_intbits(charbits2);
  num->bits[1] = from_charbits_to_intbits(charbits1);
  num->bits[0] = from_charbits_to_intbits(charbits0);
  // 0-31 bits[0]
  // 2^31 + 2^30 +...+ 2^0 = 4294967295
  // 2^31 = 2147483648

  // 32-63 bits[1]                  v
  // 2^63 + 2^32 +...+ 2^0 = 18446744073709551615
  // 2^63 = 9223372036854775808
  // ????

  // 64-95 bits[2] 10000000000000000000000000000000
  // 2^95 + 2^94 +...+ 2^0 = 79228162514264337593543950335
  // 2^95 = 39614081257132168796771975168
  // ????
}
void multiply_by_two(char *num) {
  int carry = 0;
  for (int i = strlen(num) - 1; i >= 0; i--) {
    int digit = (num[i] - '0') * 2 + carry;
    num[i] = (digit % 10) + '0';
    carry = digit / 10;
  }
  if (carry > 0) {
    memmove(num + 1, num, strlen(num) + 1);
    num[0] = carry + '0';
  }
}

// Функция для добавления 1 к десятичной строке
void add_one(char *num) {
  int carry = 1;
  for (int i = strlen(num) - 1; i >= 0 && carry; i--) {
    int digit = (num[i] - '0') + carry;
    num[i] = (digit % 10) + '0';
    carry = digit / 10;
  }
  if (carry > 0) {
    memmove(num + 1, num, strlen(num) + 1);
    num[0] = carry + '0';
  }
}

// Функция для преобразования бинарной строки в десятичную
void binary_to_decimal_string(const char *binary_str, char *decimal_str) {
  decimal_str[0] = '0';  // Инициализируем десятичную строку нулём
  decimal_str[1] = '\0';

  for (int i = 0; binary_str[i] != '\0'; i++) {
    multiply_by_two(decimal_str);  // Умножаем десятичное число на 2
    if (binary_str[i] == '1') {
      add_one(decimal_str);  // Прибавляем 1, если в бинарной строке стоит 1
    }
  }
}
void from_intbits_to_charbits(char *charbits, unsigned int bits) {
  int i = 0;
  while (bits > 0) {
    charbits[i] = bits % 2 + '0';
    bits = bits / 2;
    i++;
  }
  if (strlen(charbits) < 32) {
    strncat(charbits, "00000000000000000000000000000000",
            32 - strlen(charbits));
  }
  strreverse(charbits);
}
void from_decimal_to_charnum(char *numChar, s21_decimal *num) {
  char charbits2[33] = {'\0'};
  char charbits1[33] = {'\0'};
  char charbits0[33] = {'\0'};
  char binaryStr[97] = {'\0'};
  unsigned int bits0 = num->bits[0];
  unsigned int bits1 = num->bits[1];
  unsigned int bits2 = num->bits[2];
  from_intbits_to_charbits(charbits0, bits0);
  from_intbits_to_charbits(charbits1, bits1);
  from_intbits_to_charbits(charbits2, bits2);
  strncat(binaryStr, charbits2, strlen(charbits2));
  strncat(binaryStr, charbits1, strlen(charbits1));
  strncat(binaryStr, charbits0, strlen(charbits0));

  binary_to_decimal_string(binaryStr, numChar);
  int scale = 0;
  if (set_string_scale(num) != 0) {
    if (set_string_scale(num) >= (int)strlen(numChar)) {
      int add_0 = set_string_scale(num) - strlen(numChar) + 1;
      strreverse(numChar);
      strncat(numChar, "00000000000000000000000000000", add_0);
      strreverse(numChar);
    }
    scale = strlen(numChar) - set_string_scale(num);
    memmove(numChar + scale + 1, numChar + scale, strlen(numChar) - scale + 1);
    numChar[scale] = '.';
  }
  if (set_string_sign(num) != 0) {
    memmove(numChar + 1, numChar, strlen(numChar) + 1);
    numChar[0] = '-';
  }
}
void s21_zero_decimal(s21_decimal *dst) {
  dst->bits[0] = dst->bits[1] = dst->bits[2] = dst->bits[3] = 0;
}
int kolvo_digit(char *numberStr) {
  int intPartCount = 0;
  int fracPartCount = 0;
  int i = 0;
  int isFractionalPart = 0;
  int countfract0 = 0;
  if (numberStr[i] == '-' || numberStr[i] == '+') {
    i++;
  }

  while (numberStr[i] != '\0') {
    if (numberStr[i] == '.' || numberStr[i] == ',') {
      isFractionalPart = 1;
    } else if (isFractionalPart) {
      if (numberStr[i] == '0') {
        countfract0++;
      }
      fracPartCount++;
    } else {
      intPartCount++;
    }
    i++;
  }
  // printf("%d\n", countfract0);
  // printf("%d\n", intPartCount);
  // printf("%d\n", fracPartCount);
  return intPartCount;
}

// int read_zapyataya(int* znach_stepen, s21_decimal* target_dec) {
//   // int mask = 1;
//   // mask = mask << 9;
//   // int i = 9;
//   // printf("maska %d\n", mask);
//   // while(i <= 16){
//   //   *znach_stepen = target_dec->bits[3] & mask;
//   //   mask << 1;
//   //   i++;
//   // }

//   *znach_stepen = target_dec->bits[3] >> 9;
//   return 0;
// }

// int main() {
//   float src = -2147483648;
//   float ressrc;
//   s21_decimal dst = {0};
//   char numChar[34] = {'\0'};
//   // s21_from_int_to_decimal(src, &dst);
//   s21_from_float_to_decimal(src, &dst);
//   print_decimal(dst);
//   s21_from_decimal_to_float(dst, &ressrc);
//   printf("%f\n", ressrc);
//   from_decimal_to_charnum(numChar, &dst);
//   printf("numchar = %s\n", numChar);
//   // printf("VVedi chislo = ");
//   // char numChar[32] = {'\0'};
//   // // scanf("%s", numChar);

//   // s21_decimal num = {123, 0, 0, 0};
//   // set_scale(&num, 5);
//   // set_sign(&num, 1);
//   // int scale = set_string_scale(&num);
//   // printf("scale = %d\n", scale);
//   // int sign = set_string_sign(&num);
//   // printf("sign = %d\n", sign);
//   // // from_charnum_to_decimal(numChar, &num);
//   // from_decimal_to_charnum(numChar, &num);
//   // printf("numchar = %s\n", numChar);
//   //     printf("numbits0 = %d\n", num.bits[0]);
//   // num.bits[3] = 1536;
//   // s21_decimal num2 = {0, 0, 0, 0};
//   // printf("VVedi vtoroe chislo = ");
//   // char numChar2[30] = "000000000000000000000000000000";
//   // scanf("%s", numChar2);
//   // from_charnum_to_decimal(numChar2, &num2);

//   // s21_decimal result = {0, 0, 0, 0};
//   // int val = s21_add(num, num2, &result);
//   //  int zapyataya = 0;
//   //   read_zapyataya(&zapyataya, &num);
//   //   printf("zapyataya %d \n", zapyataya);
//   //    printf("num :\n");
//   // print_decimal(num);
//   // printf("num2 :\n");
//   // print_decimal(num2);
//   // // printf("res :\n");
//   // print_decimal(result);
//   // for (int i = 0; i < 4; i++) {
//   //   printf("%u ", result.bits[i]);
//   // }

//   return 0;
// }
