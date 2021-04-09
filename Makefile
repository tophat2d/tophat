cc=gcc
mingw=x86_64-w64-mingw32-gcc
webcc=/home/marek/desk/emsdk/upstream/emscripten/emcc # TODO: better path

sources=src/*.c src/img/*.c src/*.a
wflags=-Wall
libs=-lm -lX11 -Lsrc -lumka -L /lib64 -ldl -lGL -lpthread

cflags=$(sources) $(wflags) -o tophat $(libs) -DCNFGOGL

# TODO: sort out this mess
wincflags=$(sources) -DCNFGOGL lib/windows/libumka_static.a -o tophat.exe $(wflags) -lm -Ldl -Ilib/rawdraw -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread -Llib/windows -DUMKA_STATIC -static -lumka_static 
webcflags=$(sources) lib/umka/src/*.c $(wflags) -s WASM=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -o main.wasm -ldl -lpthread

version=v0.0

build:
	$(cc) $(cflags) -g

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
	mkdir -p tophat-release/bin
	cp tophat     tophat-release/bin/tophat-linux
	cp tophat.exe tophat-release/bin/tophat-win.exe
	cp -r umka tophat-release
	echo $(version) > tophat-release/version
	echo $(version) > bin/version
	cp examples/preset.um tophat-release
	rm -rf bin/tophat.zip
	zip -r tophat.zip tophat-release
	rm -r tophat-release
	mv tophat.zip bin

win-package: clean build windows
	mkdir -p tophat-win/tophat/bin
	cp tophat     tophat-win/tophat/bin/tophat-linux
	cp tophat.exe tophat-win/tophat/bin/tophat-win.exe
	cp -r umka tophat-win/tophat/
	cp examples/preset.um tophat-win/tophat
	cp cmd/install.bat tophat
	rm -rf bin/tophat-win.zip
	zip -Z store -y -q -r bin/tophat-win.zip tophat-win 
	rm -r tophat

clean:
	rm -rf tophat-release
	rm -f tophat tophat.exe

cmdtool:
	chmod +x cmd/tophat
	sudo cp cmd/tophat /bin

ru:
	sl
