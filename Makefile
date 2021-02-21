build:
	cc src/*.c src/img/*.c -lm -lX11 -o tophat

run: build
	./tophat

clear:
	rm tophat
