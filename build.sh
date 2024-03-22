#!/bin/bash

build_target="wasm"

while [ "$1" != "" ]; do
  case "$1" in
    -t | --target)
      build_target="$2"
      shift 1
      ;;
    -h | --help)
      echo ": - -- Options"
      echo ": h help                      : prints this message"
      echo ": t target  [wasm|mingw|msvc] : sets build target"
      echo ": r release                   : release build (default is debug)"
      exit
      ;;
    *)
      echo ": Unknown parameter: $1"
      exit
      ;;
  esac
  shift 1
done

# Get make executable
make_exe="no_make"

which make &> /dev/null
if [ "$?" == "0" ]; then
  make_exe="make"
else
  which mingw32-make &> /dev/null
  if [ "$?" == "0" ]; then
    make_exe="mingw32-make"
  fi
fi

# executable checks
case "$build_target" in
  "wasm" )
    which clang &> /dev/null
    if [ "$?" != "0" ]; then
      echo ": build $build_target: clang compiler not found, ending build."
      exit
    fi
    ;;&
  "mingw" | "msvc" )
    which cmake &> /dev/null
    if [ "$?" != "0" ]; then
      echo ": build $build_target: cmake not found, ending build."
      exit
    fi
    ;;&
  "mingw" )
    if [ "$make_exe" == "no_make" ]; then
      echo ": build $build_target: make not found, ending build."
      exit
    fi
    ;;
esac

# Run build based on target type
if [ "$build_target" = "wasm" ]; then

  pushd . &> /dev/null
  cd ./lib/wasi-libc
  if [ ! -d sysroot/ ]; then

    if [ "$make_exe" == "no_make" ]; then
      echo ": build wasm: No make installed, required to build wasi-libc."
      popd &> /dev/null
      exit
    fi

    # Build wasi-libc
    eval $make_exe
  fi
  popd &> /dev/null

  mkdir -p build/wasm

  #clang -O2 -DNDEBUG --target=wasm32-wasi -fno-trapping-math -Wall -Wextra -Werror -Wno-null-pointer-arithmetic -Wno-unused-parameter -Wno-sign-compare -Wno-unused-variable -Wno-unused-function -Wno-ignored-attributes -Wno-missing-braces -Wno-ignored-pragmas -Wno-unused-but-set-variable -Wno-unknown-warning-option -mthread-model single -isystem lib/wasi-libc/sysroot/include/wasm32-wasi -Ilib/wasi-libc/sysroot/include/wasm32-wasi -MD -MP -o build/test.o -c src/test.c

  # -nostdinc doesn't work because wasi doesn't ship with stddef for some reason
  # --target=wasm32 for non-wasi build. It works, but no standard lib is painful
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

  cp build/wasm/test.wasm web/test.wasm

elif [ "$build_target" = "mingw" ]; then

  cmake -G "MinGW Makefiles" -S . -B build/mingw
  if [ "$?" != "0" ]; then
    exit
  fi

  pushd . &> /dev/null
  cd build/mingw
  eval $make_exe
  popd &> /dev/null

elif [ "$build_target" = "msvc" ]; then

  cmake -G "Visual Studio 17 2022" -S . -B build/msvc
  if [ "$?" != "0" ]; then
    exit
  fi

else

  echo ": Invalid build target: $build_target"

fi
