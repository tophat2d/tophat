#!/bin/sh

# run make to do all the necessary embedding stuff
make

cc () {
	echo EMCC $f
	emcc -O3 -Wall -fno-strict-aliasing -Wno-format-security -DUMKA_STATIC \
		-Ilib/miniaudio \
		-Ilib/sokol \
		-Ilib/stb \
		-Ilib/umka/src \
		-Ilib/umprof \
		-DTH_GITVER=\"$(git rev-parse --short HEAD)\" \
		-DTH_VERSION=\"$(cat version)\" \
		-c $1
}

for f in lib/umka/src/umka_*.c; do
	cc $f
done

for f in src/*.c; do
	cc $f
done

emar rcs th_emscripten.a *.o
rm *.o
