CC=gcc
MINGW=x86_64-w64-mingw32-gcc

WARNINGS=-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-old-style-declaration -Wno-implicit-fallthrough
DEFINES=-DCNFGOGL -DUMKA_STATIC
RELEASE_FLAGS=-DRELEASE_BUILD
SRC=src/*.c
OBJ=src/*.o

LINUX_LFLAGS=-lm -lX11 -L /lib64 -ldl -lGL -lpthread
LINUX_UMKA=-Llib/umka/build -lumka lib/umka/build/libumka.a

WIN_LFLAGS=-lm -Ldl -Ilib/rawdraw -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread
WIN_UMKA=-Llib/windows -lumka_static

LINUX_FULL=lib/umka/build/libumka.a $(LINUX_LFLAGS) $(WARNINGS) $(DEFINES)
WIN_FULL=$(SOURCES) lib/rawdraw/chew.c lib/windows/libumka_static.a $(WARNINGS) $(WIN_LFLAGS) $(WIN_UMKA) $(DEFINES) -DNO_OPENGL_HEADERS

#build: $(OBJ)
#	$(CC) $(OBJ) $(LINUX_FULL) -o tophat

VERSION=v0.3-$(git rev-parse --short HEAD)

build: libs
	$(CC) -o tophat $(SRC) $(LINUX_FULL) $(RELEASE_FLAGS) -s -Os

install: libs
	tcc -o tophat $(SRC) $(LINUX_FULL) $(RELEASE_FLAGS) -g && sudo cp tophat /usr/share/tophat/bin/tophat-linux

wbuild: libs
	$(MINGW) -o tophat.exe $(SRC) $(WIN_FULL) $(RELEASE_FLAGS) -s -Os

run:
	tcc -o tophat $(SRC) $(LINUX_FULL) -g && ./tophat

$(OBJ): $(SRC)
	echo $@
	$(CC) $(LINUX_FULL) -c $< -o $@

clean:
	rm -rf tophat-release
	rm -f tophat tophat.exe src/umkalibs.h

libs:
	echo "#ifndef UMKALIBS_H" > src/umkalibs.h
	echo "#define UMKALIBS_H" >> src/umkalibs.h
	echo "const char *libs[] = {" >> src/umkalibs.h
	./lib/umka/build/umka cmd/embedder.um umka/animation.um umka/audio.um umka/csv.um umka/entity.um umka/image.um umka/input.um umka/map.um umka/misc.um umka/polygon.um umka/rawdraw.um umka/raycast.um umka/rectangle.um umka/tilemap.um umka/tophat.um umka/ui.um umka/vec.um umka/std/std.um umka/std/std.um >> src/umkalibs.h
	echo "};" >> src/umkalibs.h
	echo "#endif" >> src/umkalibs.h

package:
	mkdir -p tophat-release/bin
	cp tophat     tophat-release/bin/tophat-linux
	cp tophat.exe tophat-release/bin/tophat-win.exe
	echo $(VERSION) > tophat-release/version
	echo $(VERSION) > version
	cp examples/preset/*.um tophat-release
	rm -rf bin/tophat.zip
	zip -r ../tophat-bin/tophat.zip tophat-release
	rm -r tophat-release

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
	zip -Z store -y -q -r ../tophat-bin/tophat-win.zip tophat-win 
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

