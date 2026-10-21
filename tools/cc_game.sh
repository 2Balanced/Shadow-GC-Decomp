#!/bin/sh
# Compile one Game unit with the game C++ flags (cflags_game_cpp in configure.py) into OUT_DIR.
# usage: tools/cc_game.sh Unit [extra mwcc flags...]
cd "$(dirname "$0")/.." || exit 1
unit=$1; shift
out=${OUT_DIR:-build/GUPE8P/cc_game}
mkdir -p "$out"
exec build/tools/sjiswrap.exe build/compilers/GC/2.7/mwcceppc.exe \
  -nodefaults -proc gekko -align powerpc -enum int -fp hardware -Cpp_exceptions off \
  -O4,p -inline auto -pragma "cats off" -pragma "warn_notinlined off" -maxerrors 1 -nosyspath \
  -RTTI off -fp_contract on -str reuse -multibyte -i include -i build/GUPE8P/include \
  -DBUILD_VERSION=0 -DVERSION_GUPE8P -DNDEBUG=1 \
  -O4,s -use_lmw_stmw on -str reuse,readonly -RTTI on -inline noauto,level=1,deferred \
  -i src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Include \
  -i src/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include \
  "$@" -c "src/Game/$unit.cpp" -o "$out/$unit.o"
