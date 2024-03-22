#!/bin/bash

build_target="wasm"

while [ "$1" != "" ]; do
  case "$1" in
    --target | -t)
      build_target="$2"
      shift 1
      ;;
    --help | -h)
      echo " - -- Options"
      echo " h help                     : prints this message"
      echo " t target [wasm|mingw|msvc] : sets build target"
      exit
      ;;
    *)
      echo "Unknown parameter: $1"
      ;;
  esac
  shift 1
done

if [ "$build_target" = "wasm" ]; then
  mkdir -p build/wasm

  #clang -O2 -DNDEBUG --target=wasm32-wasi -fno-trapping-math -Wall -Wextra -Werror -Wno-null-pointer-arithmetic -Wno-unused-parameter -Wno-sign-compare -Wno-unused-variable -Wno-unused-function -Wno-ignored-attributes -Wno-missing-braces -Wno-ignored-pragmas -Wno-unused-but-set-variable -Wno-unknown-warning-option -mthread-model single -isystem lib/wasi-libc/sysroot/include/wasm32-wasi -Ilib/wasi-libc/sysroot/include/wasm32-wasi -MD -MP -o build/test.o -c src/test.c

  # -nostdinc doesn't work because wasi doesn't ship with stddef for some reason
  clang --target=wasm32-wasi -Oz -flto -D__WASM__ \
    --no-standard-libraries \
    -Wall -Wextra -Wno-missing-braces \
    -Wl,--allow-undefined -Wl,--no-entry -Wl,--lto-O3\
    -isystem lib/wasi-libc/sysroot/include/wasm32-wasi \
    -isystem src/eng/wasm \
    -I src -I src/eng \
    -o build/wasm/test.wasm \
    src/*.c \
    src/game/*.c \
    src/levels/*.c \
    src/eng/*.c \
    src/eng/wasm/*.c \
    lib/wasi-libc/sysroot/lib/wasm32-wasi/libc.a

  #clang -O0 --target=wasm32 --no-standard-libraries -Wl,--no-entry \
  #  -Wl,--allow-undefined \
  #  -o build/test.wasm \
  #  src/test.c

  cp build/wasm/test.wasm web/test.wasm
elif [ "$build_target" = "mingw" ]; then

  cmake -G "MinGW Makefiles" -S . -B build/cmake/mingw

  if [ "$?" == "0" ]; then
    pushd .
    cd build/cmake/mingw
    mingw32-make
    popd
  fi

elif [ "$build_target" = "msvc" ]; then

  cmake -G "Visual Studio 17 2022" -S . -B build/cmake/msvc

else
  echo "Invalid build target: $build_target"
fi
