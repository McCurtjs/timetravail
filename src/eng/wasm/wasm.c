
#include "../wasm.h"

#include <string.h>

extern void js_log(const char* str, unsigned len);
extern void js_log_int(int i);
extern void js_log_num(float f);
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

void alert(const char* str) {
  js_alert(str, strlen(str));
}
