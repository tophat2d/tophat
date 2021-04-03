cc=cc
mingw=x86_64-w64-mingw32-gcc
webcc=/home/marek/desk/emsdk/upstream/emscripten/emcc # TODO: better path

sources=src/*.c src/img/*.c src/*.a
wflags=-Wall
libs=-lm -lX11 -Lsrc -lumka -L /lib64 -ldl -lGL -lpthread

cflags=$(sources) $(wflags) -o tophat $(libs) -Os

# TODO: sort out this mess
wincflags=$(sources) lib/windows/libumka_static.a -o tophat.exe $(wflags) -lm -Ldl -Ilib/rawdraw -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread -Llib/windows -DUMKA_STATIC -static -lumka_static
webcflags=$(sources) lib/umka/src/*.c $(wflags) -s WASM=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -o main.wasm -ldl -lpthread

version=v0.0

build:
	$(cc) $(cflags)

windows:
	$(mingw) $(wincflags)

web:
	$(webcc) $(webcflags)

run:
	$(cc) $(cflags) -g
	if [ -f "./tophat" ]; then ./tophat debug; fi

install: 
	$(cc) $(cflags) -g
	sudo cp tophat /usr/share/tophat/bin/tophat-linux

package: clean build windows
	if [ ! -f "tophat-release" ]; then mkdir tophat-release; fi
	if [ ! -f "tophat-release/bin" ]; then mkdir tophat-release/bin; fi
	cp tophat tophat-release/bin/tophat-linux
	cp tophat.exe tophat-release/bin/tophat-win.exe
	cp -r umka tophat-release
	echo $(version) > tophat-release/version
	echo $(version) > bin/version
	cp examples/preset.um tophat-release
	zip -r tophat.zip tophat-release
	rm -r tophat-release
	mv tophat.zip bin

clean:
	if [ -f "./tophat-release" ]; then rm -r tophat-release tophat.tar.xz; fi
	if [ -f "./tophat.zip" ];     then rm -r tophat.zip;                   fi
	if [ -f "./bin/tophat.zip" ]; then rm -r bin/tophat.zip;               fi
	if [ -f "./tophat" ];         then rm tophat;                          fi
	if [ -f "./tophat.exe" ];     then rm tophat.exe;                      fi

cmdtool:
	chmod +x cmd/tophat
	sudo cp cmd/tophat /bin

ru:
	sl
