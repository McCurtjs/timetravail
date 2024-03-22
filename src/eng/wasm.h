#ifndef __WASM_DEFS_
#define __WASM_DEFS_

#include "types.h"

#ifdef __WASM__
// allgedly supposed to work, but doesn't
//#define export __attribute__((used))

// also supposed to work, but doesn't
//#define export __attribute__((visibility( "default" )))

// very clunky, but actually does work
#define export(fn_name) __attribute__((export_name(#fn_name))) fn_name
#else
#define export(fn_name) fn_name
#endif

void print(const char* str);
void print_int(int i);
void print_ptr(const void* p);
void print_float(float f);
void print_floats(const float* f, uint count);
void alert(const char* str);

#endif
