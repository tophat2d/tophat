platform := $(shell uname -s)
shortplatform := $(shell (X=`uname -s`; echo $${X:0:10}))

BUILDPATH = build

ifeq ($(platform), Linux)
  LFLAGS = -lm -lX11 -L /lib64 -ldl -lGL -lpthread
  LPATH = LD_LIBRARY_PATH
  CROSS_CC=x86_64-w64-mingw32-gcc
  FULL=$(LFLAGS) $(WARNINGS) $(DEFINES)
  BINARY=tophat
else
ifeq ($(shortplatform), MINGW64_NT)
  LFLAGS=-lm -Ldl -Ilib/rawdraw -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread
  LPATH = PATH
  FULL=lib/rawdraw/chew.c $(WARNINGS) $(LFLAGS) $(DEFINES) -DNO_OPENGL_HEADERS
  BINARY=tophat.exe
endif
endif

WARNINGS=-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-old-style-declaration -Wno-implicit-fallthrough
DEFINES=-DCNFGOGL -DUMKA_STATIC
RELEASE_FLAGS=-DRELEASE_BUILD
UMKA = lib/umka/build/libumka.a

SRC=src/*.c
OBJ=src/*.o

UMKA_SRC=lib/umka/src/umka_common.c lib/umka/src/umka_const.c lib/umka/src/umka_expr.c lib/umka/src/umka_ident.c lib/umka/src/umka_runtime.c lib/umka/src/umka_types.c lib/umka/src/umka_api.c lib/umka/src/umka_compiler.c lib/umka/src/umka_decl.c lib/umka/src/umka_gen.c lib/umka/src/umka_lexer.c lib/umka/src/umka_stmt.c lib/umka/src/umka_vm.c

VERSION=v0.3-$(shell git rev-parse --short HEAD)

OBJS=$(patsubst src/%.c, src/%.o, $(wildcard src/*.c))

src/%.o: src/%.c
	@echo CC $@
	@$(CC) -o $@ -c $< $(FULL) $(RELEASE_FLAGS) -Ofast

build: libs $(OBJS)
	@mkdir -p $(BUILDPATH)
	@echo LD $(BUILDPATH)/$(BINARY)
	@$(CC) $(OBJS) $(UMKA) $(FULL) -o $(BUILDPATH)/$(BINARY)

cross: libs
	$(CROSS_CC) -o tophat.exe $(SRC) $(WIN_FULL) $(RELEASE_FLAGS) -s -Os

run:
	@rm src/bindings.o
	@make build
	./$(BUILDPATH)/$(BINARY)

clean:
	rm -rf tophat-release tophat-win
	rm -rf $(BUILDPATH)
	rm -f src/umkalibs.h
	rm -f src/*.o

libs:
	@echo "embedding tophat std"
	@echo "#ifndef UMKALIBS_H" > src/umkalibs.h
	@echo "#define UMKALIBS_H" >> src/umkalibs.h
	@echo "const char *libs[] = {" >> src/umkalibs.h
	@./lib/umka/build/umka cmd/embedder.um umka/animation.um umka/audio.um umka/csv.um umka/entity.um umka/image.um umka/input.um umka/misc.um umka/polygon.um umka/rawdraw.um umka/raycast.um umka/rectangle.um umka/tilemap.um umka/tophat.um umka/ui.um umka/vec.um umka/std/std.um umka/std/std.um >> src/umkalibs.h
	@echo "};" >> src/umkalibs.h
	@echo "#endif" >> src/umkalibs.h

package:
	mkdir -p tophat-release/bin
	cp tophat     tophat-release/bin/tophat-linux
	cp tophat.exe tophat-release/bin/tophat-win.exe
	echo $(VERSION) > tophat-release/version
	echo $(VERSION) > version
	cp examples/preset/*.um tophat-release
	rm -rf bin/tophat.zip
	zip -r tophat.zip tophat-release
	rm -r tophat-release
	rm version

win-package:
	mkdir -p tophat-win/bin
	cp tophat tophat-win/bin/tophat-linux
	cp tophat.exe tophat-win/bin/tophat-win.exe
	echo $(VERSION) > tophat-win/version
	cp -r examples/* tophat-win/
	mv tophat-win/README.md tophat-win/examples.txt
	cp cmd/run.bat tophat-win/preset
	cp cmd/init.bat tophat-win/
	cp cmd/package.bat tophat-win/
	cp etc/win-readme tophat-win/readme.txt
	rm -rf bin/tophat-win.zip
	zip -Z store -y -q -r tophat-win.zip tophat-win 
	rm -r tophat-win

deps:
	if [ ! -f libs/umka/Makefile ]; then git submodule update --init --recursive --remote; fi
	@echo "updating submodules"
	git submodule update --remote --merge
	@echo "downloading miniaudio"
	curl https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h > lib/miniaudio.h
	@echo "downloading stb_image"
	curl https://raw.githubusercontent.com/nothings/stb/master/stb_image.h > lib/stb_image.h
	@echo "building umka"
	cd lib/umka && $(MAKE)
	@echo "done"

