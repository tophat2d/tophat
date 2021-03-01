build:
	cc src/*.c src/img/*.c src/*.a -lm -lX11 -o tophat -Lsrc -lumka -L /lib64 -ldl

run: build
	./tophat

umka:
	cc umka/*.c -o umka/build
	./umka/build

clear:
	rm tophat
