cc=cc
cflags=src/*.c src/img/*.c src/*.a -Wall -lm -lX11 -o tophat -Lsrc -lumka -L /lib64 -ldl -lGL -lpthread
version=v0.0

build:
	$(cc) $(cflags)

windows:
	x86_64-w64-mingw32-gcc src/*.c lib/windows/*.a -o tophat.exe -Wall -lm -Ldl -Ilib/rawdraw -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread -Llib/windows -lumka
run:
	$(cc) $(cflags) -g
	if [ -f "./tophat" ]; then ./tophat debug; fi

install: build
	sudo cp tophat /usr/share/tophat/bin/tophat-linux

package: clean build windows
	if [ ! -f "tophat-release" ]; then mkdir tophat-release; fi
	if [ ! -f "tophat-release/bin" ]; then mkdir tophat-release/bin; fi
	cp tophat tophat-release/bin/tophat-linux
	cp tophat.exe tophat-release/bin/tophat-win.exe
	cp bin/windows/libumka.dll tophat-release/bin
	cp -r umka tophat-release
	echo $(version) > tophat-release/version
	echo $(version) > bin/version
	cp examples/preset.um tophat-release
	tar cfJ tophat.tar.xz tophat-release
	rm -r tophat-release
	mv tophat.tar.xz bin

clean:
	if [ -f "./tophat-release" ]; then rm -r tophat-release tophat.tar.xz; fi
	if [ -f "./tophat.tar.xz" ]; then rm -r tophat.tar.xz; fi
	if [ -f "./tophat" ]; then rm tophat; fi
	if [ -f "./tophat.exe" ]; then rm tophat.exe; fi

cmdtool:
	chmod +x cmd/tophat
	sudo cp cmd/tophat /bin

ru:
	sl
