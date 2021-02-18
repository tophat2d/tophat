#include "../lib/rawdraw/CNFG.h"
#include "entity.h"

#include <stdio.h>

void HandleKey( int keycode, int bDown ) { }
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { }
void HandleDestroy() { }

int main() {
	entity o;
	o.w = 50;
	o.h = 80;
	o.color = 0xffffebff;
	o.image = NULL;
	o.px = 0;
	o.py = 0;

	CNFGSetup( "Example App", 1024, 768 );
	

	while (1) {
		CNFGBGColor = 0x080808ff;

		CNFGClearFrame();
		CNFGHandleInput();

					
		draw(o);
		CNFGSwapBuffers();
	}

	return 0;
}
