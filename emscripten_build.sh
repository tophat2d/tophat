#!/bin/sh

# run make to do all the necessary embedding stuff
make

cc () {
	echo EMCC $f
	emcc -g -O2 -DUMKA_STATIC \
		-Ilib/miniaudio \
		-Ilib/sokol \
		-Ilib/stb \
		-Ilib/umka/src \
		-Ilib/umprof \
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

#emcc --shell-file=./shell.html -s ASYNCIFY=1 -s ALLOW_MEMORY_GROWTH=1 -lc -lm src/*.c -o output.html
#--shell-file=./shell.html
