#!/bin/sh

emcc lib/umka/src/*.c src/*.c -Ilib/umka/src -Ilib/miniaudio -Ilib/stb -Ilib/rawdraw -sSINGLE_FILE -sEXPORTED_RUNTIME_METHODS=UTF8ToString -sASYNCIFY -O3
cp a.out.js wasm/
