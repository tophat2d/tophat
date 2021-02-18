#include "../lib/rawdraw/CNFG.h"
#include "entity.h"

#include <stdio.h>

void HandleKey( int keycode, int bDown ) { }
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { }
void HandleDestroy() { }

int main() {
	entity o = entityfromrect(newrect(10, 20, 40, 40), 0xffffebff);

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
