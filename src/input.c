#include <string.h>

int mousex;
int mousey;

int pressed[255];
int just_pressed[255];

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
		pressed[keycode] = 0;
		just_pressed[keycode] = 0;
		return;
	}

	if (!pressed[keycode]) {
		pressed[keycode] = 1;
		just_pressed[keycode] = 1;
		return;
	}

	just_pressed[keycode] = 0;
}
void HandleButton( int x, int y, int button, int bDown ) {
	HandleKey(button+6, bDown);
}

void HandleMotion( int x, int y, int mask ) {
	mousex = x;
	mousey = y;
}

void _th_input_init() {
	memset(&pressed[0], 0, 255 * sizeof(int));
	memset(&just_pressed[0], 0, 255 * sizeof(int));
}
