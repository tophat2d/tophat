build:
	cc src/*.c -lm -lX11 -o tophat

run: build
	./tophat
