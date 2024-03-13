
#include "../wasm.h"

#include <string.h>

extern void js_log(const char* str, unsigned len);
extern void js_log_int(int i);
extern void js_log_num(float f);
extern void js_log_num_array(const float* ptr, uint count);
extern void js_alert(const char* str, unsigned len);

void print(const char* str) {
  js_log(str, strlen(str));
}

void print_int(int i) {
  js_log_int(i);
}

void print_float(float f) {
  js_log_num(f);
}

void print_floats(const float* f, uint count) {
  js_log_num_array(f, count);
}

void alert(const char* str) {
  js_alert(str, strlen(str));
}
