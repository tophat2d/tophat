#rm -rf wasm-out
mkdir -p wasm-out

# run make to do all the necessary embedding stuff
make

emcc -g -O2 -DUMKA_STATIC -sASYNCIFY -Wall -Wno-format-security\
	-Ilib/miniaudio \
	-Ilib/sokol \
	-Ilib/stb \
	-Ilib/umka/src \
	-Ilib/umprof \
	lib/umka/src/umka_api.c \
	lib/umka/src/umka_common.c \
	lib/umka/src/umka_compiler.c \
	lib/umka/src/umka_const.c \
	lib/umka/src/umka_decl.c \
	lib/umka/src/umka_expr.c \
	lib/umka/src/umka_gen.c \
	lib/umka/src/umka_ident.c \
	lib/umka/src/umka_lexer.c \
	lib/umka/src/umka_runtime.c \
	lib/umka/src/umka_stmt.c \
	lib/umka/src/umka_types.c \
	lib/umka/src/umka_vm.c \
	src/atlas.c \
	src/audio.c \
	src/bindings.c \
	src/canvas.c \
	src/collisions.c \
	src/color.c \
	src/entity.c \
	src/font.c \
	src/image.c \
	src/input.c \
	src/main.c \
	src/miniaudio.c \
	src/misc.c \
	src/nav.c \
	src/particles.c \
	src/quad.c \
	src/raycast.c \
	src/shader.c \
	src/sokol.c \
	src/staembed.c \
	src/stbi.c \
	src/tilemap.c \
	src/tophat.c \
	src/transform.c \
	src/utf8.c \
	src/window.c \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s TOTAL_MEMORY=64mb \
	-lc \
	-lm \
	--shell-file=./shell.html \
	--preload-file main.um \
	--preload-file etc/roboto.ttf \
	--preload-file etc/logo/logo-normal-white.png \
	-o wasm-out/output.html

#emcc --shell-file=./shell.html -s ASYNCIFY=1 -s ALLOW_MEMORY_GROWTH=1 -lc -lm src/*.c -o output.html
#--shell-file=./shell.html
