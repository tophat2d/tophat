build:
	cc src/*.c src/img/*.c src/*.a -Wall -lm -lX11 -o tophat -Lsrc -lumka -L /lib64 -ldl -lGL

run: build
	./tophat debug

install: build
	sudo cp tophat /usr/share/tophat/bin/tophat-linuxx11

clear:
	rm tophat
