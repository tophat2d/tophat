build:
	cc src/*.c src/img/*.c src/*.a -Wall -lm -lX11 -o tophat -Lsrc -lumka -L /lib64 -ldl -lGL

run: build
	./tophat

umka:
	cc umka/*.c -o umka/build
	./umka/build

clear:
	rm tophat
