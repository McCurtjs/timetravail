
mkdir -p build

#clang -O2 -DNDEBUG --target=wasm32-wasi -fno-trapping-math -Wall -Wextra -Werror -Wno-null-pointer-arithmetic -Wno-unused-parameter -Wno-sign-compare -Wno-unused-variable -Wno-unused-function -Wno-ignored-attributes -Wno-missing-braces -Wno-ignored-pragmas -Wno-unused-but-set-variable -Wno-unknown-warning-option -mthread-model single -isystem lib/wasi-libc/sysroot/include/wasm32-wasi -Ilib/wasi-libc/sysroot/include/wasm32-wasi -MD -MP -o build/test.o -c src/test.c

clang -Oz --target=wasm32-wasi --no-standard-libraries -Wl,--no-entry \
  -isystem lib/wasi-libc/sysroot/include/wasm32-wasi \
  -Wl,--allow-undefined \
  -o build/test.wasm \
  src/*.c \
  src/wasm/*.c \
  src/sdl/*.c \
  lib/wasi-libc/sysroot/lib/wasm32-wasi/libc.a

#clang -O0 --target=wasm32 --no-standard-libraries -Wl,--no-entry \
#  -Wl,--allow-undefined \
#  -o build/test.wasm \
#  src/test.c

cp build/test.wasm web/test.wasm
