#!/bin/bash

build_target="wasm"
unit_test=false
build_type="Debug"
skip_cmake=false
args=""

while [ "$1" != "" ]; do
  case "$1" in
    -t | --target)
      build_target="$2"
      shift 1
      ;;
    -h | --help)
      echo ": - -- Options"
      echo ": h help                            : prints this message"
      echo ": t target  [wasm|clang|mingw|msvc] : sets build target"
      echo ": r release                         : release build (default is debug)"
      echo ": u unit-test                       : builds unit tests"
      echo ": U                                 : runs tests after build (mingw)"
      echo ": a args    \" \"                     : passes args to built exe (if any)"
      echo ": s skip-cmake                      : skips cmake"
      exit
      ;;
    -u | --unit-test)
      unit_test=true
      ;;
    -U )
      build_target="mingw"
      unit_test=true
      skip_cmake=true
      ;;
    -a | --args)
      args="$2"
      shift 1
      ;;
    -r | --release)
      build_type="Release"
      ;;
    -s | --skip-cmake)
      skip_cmake=true
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
if [ "$build_target" = "wasm" ] || [ "$build_target" = "clang" ]; then

  flags_common="
    -Wall -Wextra -Wno-missing-braces \
    -I src -I src/eng \
  "

  sources="
    src/*.c \
    src/game/*.c \
    src/levels/*.c \
    src/eng/*.c \
  "

  sources_test="
    tst/*.c \
    tst/src/eng/*.c \
    tst/tst/*.c \
    src/eng/*.c \
  "

  flags_test="
    -Dmalloc=cspec_malloc -Drealloc=cspec_realloc \
    -Dcalloc=cspec_calloc -Dfree=cspec_free \
    -I src -I src/eng -I tst \
  "

  flags_debug_opt="-g -O0"
  if [ "$build_type" = "Release" ]; then
    flags_debug_opt="-Oz -flto"
  fi

  if [ "$build_target" = "wasm" ]; then

    # -nostdinc doesn't work because wasi doesn't ship with stddef for some reason
    # --target=wasm32 for non-wasi build. It works, but no standard lib is painful
    flags_wasm="--target=wasm32-wasi -D__WASM__ \
      -Wl,--allow-undefined -Wl,--no-entry -Wl,--lto-O3 \
      --no-standard-libraries \
      -isystem lib/wasi-libc/sysroot/include/wasm32-wasi \
      -isystem src/eng/wasm \
      lib/wasi-libc/sysroot/lib/wasm32-wasi/libc.a \
      src/eng/wasm/*.c \
    "

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

    #clang -O2 -DNDEBUG --target=wasm32-wasi -fno-trapping-math -Wall -Wextra -Werror -Wno-null-pointer-arithmetic -Wno-unused-parameter -Wno-sign-compare -Wno-unused-variable -Wno-unused-function -Wno-ignored-attributes -Wno-missing-braces -Wno-ignored-pragmas -Wno-unused-but-set-variable -Wno-unknown-warning-option -mthread-model single -isystem lib/wasi-libc/sysroot/include/wasm32-wasi -Ilib/wasi-libc/sysroot/include/wasm32-wasi -MD -MP -o build/test.o -c src/test.c

    if [ "$unit_test" == false ]; then

      mkdir -p build/wasm

      clang $flags_wasm -o build/wasm/test.wasm \
        $flags_common $flags_debug_opt $sources

      cp build/wasm/test.wasm web/test.wasm

    else

      mkdir -p build/wasm/test

      clang $flags_wasm -o build/wasm/test/test.wasm \
        $flags_common $flags_test $flags_debug_opt $sources_test

      cp build/wasm/test/test.wasm web/test.wasm

    fi

  elif [ "$build_target" = "clang" ]; then

    echo "Clang native not yet working"

#    if [ "$unit_test" == false ]; then
#
#      mkdir -p build/clang
#
#      clang --target=wasm32-wasi -Oz -flto -D__WASM__ \
#        --no-standard-libraries \
#        -Wall -Wextra -Wno-missing-braces \
#        -Wl,--allow-undefined -Wl,--no-entry -Wl,--lto-O3 \
#        -isystem lib/wasi-libc/sysroot/include/wasm32-wasi \
#        -isystem src/eng/wasm \
#        -I src -I src/eng \
#        -o build/clang/test.exe \
#        src/*.c \
#        src/game/*.c \
#        src/levels/*.c \
#        src/eng/*.c \
#        src/eng/wasm/*.c \
#        lib/wasi-libc/sysroot/lib/wasm32-wasi/libc.a
#
#      cp build/clang/test.wasm web/test.wasm
#
#    else
#
#      mkdir -p build/clang/test
#
#      pushd . &> /dev/null
#      cd build/clang/test
#
#      clang -Oz -flto \
#        -Dmalloc=malloc_test -Drealloc=realloc_test \
#        -Dcalloc=calloc_test -Dfree=free_test \
#        -Wall -Wextra -Wno-missing-braces \
#        -Wl,--lto-O3 \
#        -I src -I src/eng -I tst \
#        -o build/clang/test/test \
#        tst/*.c \
#        tst/eng/*.c \
#        src/eng/*.c \
#
#      popd &> /dev/null
#
#    fi

  fi

elif [ "$build_target" = "mingw" ]; then

  if [ "$skip_cmake" != true ]; then
    cmake -G "MinGW Makefiles" -S . -B build/mingw -DCMAKE_BUILD_TYPE=$build_type
    if [ "$?" != "0" ]; then
      exit
    fi
  fi

  pushd . &> /dev/null
  cd build/mingw
  eval $make_exe
  if [ "$?" != "0" ]; then
    unit_test=false
  fi
  popd &> /dev/null

  if [ "$unit_test" = true ]; then
    ./build/mingw/debug/Donk_tests.exe $args
  fi

elif [ "$build_target" = "msvc" ]; then

  cmake -G "Visual Studio 17 2022" -S . -B build/msvc
  if [ "$?" != "0" ]; then
    exit
  fi

else

  echo ": Invalid build target: $build_target"

fi
