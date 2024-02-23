
#include "../wasm.h"

#include <string.h>

extern void js_log(const char* str, unsigned len);
extern void js_log_int(const int i);
extern void js_alert(const char* str, unsigned len);

void print(const char* str) {
  js_log(str, strlen(str));
}

void print_int(const int i) {
  js_log_int(i);
}

void alert(const char* str) {
  js_alert(str, strlen(str));
}
