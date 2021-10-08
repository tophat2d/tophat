#include <string.h>
#include "tophat.h"

extern th_global thg;

void HandleKey(int keycode, int bDown) {
	if (keycode > 255) {
		switch (keycode) {
		case 65288:
			keycode = 0;
			break;
		case 65307:
			keycode = 1;
			break;
		case 65293:
			keycode = 2;
			break;
		case 65362:
			keycode = 3;
			break;
		case 65364:
			keycode = 4;
			break;
		case 65361:
			keycode = 5;
			break;
		case 65363:
			keycode = 6;
			break;
		default:
			return;
		}
	}

	if (!bDown) {
		thg.pressed[keycode] = 0;
		thg.just_pressed[keycode] = 0;
		return;
	}

	if (!thg.pressed[keycode]) {
		thg.pressed[keycode] = 1;
		thg.just_pressed[keycode] = 1;
		return;
	}

	thg.just_pressed[keycode] = 0;
}
void HandleButton( int x, int y, int button, int bDown ) {
	HandleKey(button+6, bDown);
}

void HandleMotion( int x, int y, int mask ) {
	thg.mouse = (th_vf2){{x, y}};
}
