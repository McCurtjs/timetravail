#ifndef __WASM_DEFS_
#define __WASM_DEFS_

// allgedly supposed to work, but doesn't
//#define export __attribute__((used))

// also supposed to work, but doesn't
//#define export __attribute__((visibility( "default" )))

// very clunky, but actually does work
#define export(fn_name) __attribute__((export_name(#fn_name))) fn_name

void print(const char* str);
void print_int(int i);
void print_float(float f);
void alert(const char* str);

#ifndef SQRT2
#define SQRT2 1.41421356237
#endif

#ifndef PI
#define PI 3.14159265358979323846264338
#endif
#define TAU (2 * PI)

#endif
