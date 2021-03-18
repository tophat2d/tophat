cc=cc
cflags=src/*.c src/img/*.c src/*.a -Wall -lm -lX11 -o tophat -Lsrc -lumka -L /lib64 -ldl -lGL -lpthread

build:
	$(cc) $(cflags)

windows:
	x86_64-w64-mingw32-gcc src/*.c lib/windows/*.a -o tophat.exe -Wall -lm -Ldl -Ilib/rawdraw -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread -Llib/windows -lumka
run:
	$(cc) $(cflags) -g
	./tophat debug

install: build
	sudo cp tophat /usr/share/tophat/bin/tophat-linuxx11

package: build
	mkdir tophat-release
	mkdir tophat-release/bin
	cp tophat tophat-release/bin
	cp -r umka tophat-release
	cp example/preset.um tophat-release
	tar cfJ tophat.tar.xz tophat-release
	rm -r tophat-release

clean:
	rm -r tophat-release tophat.tar.xz
	rm tophat
