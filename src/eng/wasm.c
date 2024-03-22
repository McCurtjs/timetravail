#include "wasm.h"

#define FN_SIGNATURE_PRINT void print(const char* str)

#ifdef __WASM__
extern void js_log(const char* str, unsigned len);
extern void js_log_int(long long int i);
extern void js_log_num(float f);
extern void js_log_num_array(const float* ptr, uint count);
extern void js_alert(const char* str, unsigned len);

#include <string.h> // strlen

FN_SIGNATURE_PRINT {
  js_log(str, strlen(str));
}

#else
#include <stdio.h> // printf

FN_SIGNATURE_PRINT {
  printf("%s\n", str);
}
#endif

void print_int(int i) {
  #ifdef __WASM__
  js_log_int(i);
  #else
  printf("%d\n", i);
  #endif
}

void print_ptr(const void* p) {
  #ifdef __WASM__
  js_log_int((size_t)p);
  #else
  printf("%d\n", p);
  #endif
}

void print_float(float f) {
  #ifdef __WASM__
  js_log_num(f);
  #else
  printf("%f\n", f);
  #endif
}

void print_floats(const float* f, uint count) {
  #ifdef __WASM__
  js_log_num_array(f, count);
  #else
  printf("[");
  for (uint i = 0; i < count; ++i) {
    printf("%f", f[i]);
    if (i != count -1) printf(",");
  }
  printf("]\n");
  #endif
}

void alert(const char* str) {
  #ifdef __WASM__
  js_alert(str, strlen(str));
  #else
  fprintf(stderr, "%s", str);
  #endif
}
