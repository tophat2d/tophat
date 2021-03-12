build:
	cc src/*.c src/img/*.c src/*.a -Wall -lm -lX11 -o tophat -Lsrc -lumka -L /lib64 -ldl -lGL

run: build
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

clear:
	rm -r tophat-release tophat.tar.xz
	rm tophat
