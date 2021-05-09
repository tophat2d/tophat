CC=gcc
MINGW=x86_64-w64-mingw32-gcc

SOURCES=src/*.c
WARNINGS=-Wall -Wno-maybe-uninitialized
LFLAGS=-lm -lX11 -Lsrc -lumka -L /lib64 -ldl -lGL -lpthread
WINLFLAGS=-lm -Ldl -Ilib/rawdraw -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread
UMKA_LIB=-Llib/umka/build -lumka
WIN_UMKA_LIB=-Llib/windows -lumka_static
DEFINES=-DCNFGOGL -DUMKA_STATIC

CFLAGS=-o tophat $(SOURCES) lib/umka/build/libumka.a $(WARNINGS) $(LFLAGS) $(UMKA_LIB) $(DEFINES)
WINCFLAGS=-o tophat $(SOURCES) lib/rawdraw/chew.c lib/windows/libumka_static.a $(WARNINGS) $(WINLFLAGS) $(WIN_UMKA_LIB) $(DEFINES) -DNO_OPENGL_HEADERS

RELEASE_FLAGS=-Os -DRELEASE_BUILD

VERSION=v0.2-$(shell git rev-parse --short HEAD)

build: libembed
	$(CC) $(CFLAGS) $(RELEASE_FLAGS)

windows: libembed
	$(MINGW) $(WINCFLAGS) $(RELEASE_FLAGS)

run:
	$(CC) $(CFLAGS) -g && ./tophat

install: 
	$(CC) $(CFLAGS) -g && sudo cp tophat /usr/share/tophat/bin/tophat-linux

package: clean build windows
	mkdir -p tophat-release/bin
	cp tophat     tophat-release/bin/tophat-linux
	cp tophat.exe tophat-release/bin/tophat-win.exe
	echo $(VERSION) > tophat-release/version
	echo $(VERSION) > bin/version
	cp examples/preset/*.um tophat-release
	rm -rf bin/tophat.zip
	zip -r tophat.zip tophat-release
	rm -r tophat-release
	mv tophat.zip bin
	make clean

win-package: clean build windows
	mkdir -p tophat-win/bin
	mkdir -p tophat-win/preset
	cp tophat tophat-win/bin/tophat-linux
	cp tophat.exe tophat-win/bin/tophat-win.exe
	echo $(VERSION) > tophat-win/version
	cp examples/preset/*.um tophat-win/preset
	cp cmd/run.bat tophat-win/preset
	cp cmd/init.bat tophat-win/
	cp cmd/package.bat tophat-win/
	rm -rf bin/tophat-win.zip
	zip -Z store -y -q -r bin/tophat-win.zip tophat-win 
	rm -r tophat-win

clean:
	rm -rf tophat-release
	rm -f tophat tophat.exe src/umkalibs.h

cmdtool:
	chmod +x cmd/tophat-cli
	sudo cp cmd/tophat-cli /bin

libembed:
	echo "#ifndef UMKALIBS_H\n#define UMKALIBS_H\nconst char *libs[] = {" > src/umkalibs.h
	./lib/umka/build/umka cmd/embedder.um umka/animation.um umka/audio.um umka/csv.um umka/entity.um umka/image.um umka/input.um umka/map.um umka/misc.um umka/polygon.um umka/rawdraw.um umka/raycast.um umka/rectangle.um umka/tilemap.um umka/tophat.um umka/ui.um umka/vec.um umka/std/std.um >> src/umkalibs.h
	echo "};\n#endif" >> src/umkalibs.h

deps:
	@echo "updating submodules"
	git submodule update --remote --merge
	@echo "downloading miniaudio"
	curl https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h > lib/miniaudio.h
	@echo "downloading stb_image"
	curl https://raw.githubusercontent.com/nothings/stb/master/stb_image.h > lib/stb_image.h
	@echo "building umka"
	cd lib/umka && $(MAKE)
	@echo "done"
