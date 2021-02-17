#include "../lib/rawdraw/CNFG.h"
#include "object.h"

#include <stdio.h>

void HandleKey( int keycode, int bDown ) { }
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { }
void HandleDestroy() { }

int main() {
	obj o = load("test.png");

	unsigned char *img = o.image;
	size_t img_size = o.w * o.h * 3;


	for (unsigned char *p = img; p != img + img_size; p += 3) {
		printf("R: %d, G: %d, B: %d,", *p, *(p + 1), *(p + 2));
	}

	printf("loaded image");

	return 0;

	o.px = 0;
	o.py = 0;

	CNFGSetup( "Example App", 1024, 768 );
	
	while (1) {
		draw(o);
	
		CNFGSwapBuffers();
	}

	return 0;
}
