#include "calc.h"
#include <stdio.h>
#include <string.h>

#define alpha_base 10
#define binary_base 2
#define ascii_diff 48

// Computes the power given a base and exponent
int power(int base, int exponent) {
  int result = 1;
  for (int i = 0; i < exponent; i++) {
    result *= base;
  }
  return result;
}

// Converts a string to an int when provided a base
int string_to_int(char *string, int base) {
  int result = 0;
  int running_value = 0;

  for (int i = strlen(string) - 1; i >= 0; i--) {
    char c = string[i];
    if (base == binary_base) {
      if (c == '1') {
        result += power(base, running_value);
      }
    } else {
      if (c >= '0' && c <= '9') {
        result += power(base, running_value) * (c - ascii_diff);
      } else if (c >= 'a' && c <= 'f') {
        result += power(base, running_value) * ((c - 'a') + alpha_base);
      } else if (c >= 'A' && c <= 'F') {
        result += power(base, running_value) * ((c - 'A') + alpha_base);
      }
    }

    running_value += 1;
  }

  return result;
}
