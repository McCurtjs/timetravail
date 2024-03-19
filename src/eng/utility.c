#include "types.h"

#include <stdlib.h>
#include <string.h>

// WASI doesn't support stof yet, which is annoying. Too lazy to make a function
// right now, pulled from Karl Knechtel at:
// https://stackoverflow.com/questions/4392665/converting-string-to-float-without-stof-in-c
float stof(const char* s) {
  float rez = 0, fact = 1;
  if (*s == '-'){
    s++;
    fact = -1;
  };
  for (int point_seen = 0; *s; s++){
    if (*s == '.'){
      point_seen = 1;
      continue;
    };
    int d = *s - '0';
    if (d >= 0 && d <= 9){
      if (point_seen) fact /= 10.0f;
      rez = rez * 10.0f + (float)d;
    };
  };
  return rez * fact;
}

int stoi(const char* s) {
  return atoi(s);
}

#define NUM_BUF_SIZE 20
static char number_result[NUM_BUF_SIZE + 1];

const char* itos(int i) {
  char* c = number_result;
  char* r = c;

  if (i < 0) {
    *c++ = '-';
    i *= -1;
    ++r;
  }

  while (i) {
    *(c++) = '0' + i % 10;
    i /= 10;
  }

  if (c == number_result) {
    *c++ = '0';
  }

  *c-- = '\0';

  while (r < c) {
    i = *r;
    *r++ = *c;
    *c-- = i;
  }

  return number_result;
}

static char float_result[NUM_BUF_SIZE + 1];

#define FLOAT_PRECISION 100000
const char* ftos(float f) {
  uint i = 0;
  const char* integer_part = itos((int)f);
  if (f < 0) f *= -1;

  while (integer_part[i]) {
    float_result[i] = integer_part[i];
    ++i;
  }
  float_result[i] = '\0';

  uint decimal = (uint)(f * FLOAT_PRECISION) % FLOAT_PRECISION;
  if (!decimal) return float_result;

  float_result[i++] = '.';

  const char* decimal_part = itos(decimal);
  uint j = 0;

  while (decimal_part[j]) {
    float_result[i++] = decimal_part[j++];
  }

  do {
    float_result[i--] = '\0';
  } while (float_result[i] == '0');

  return float_result;
}
