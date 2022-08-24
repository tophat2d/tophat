#include <string.h>
#include "tophat.h"

extern th_global *thg;

#define LINUX_KEY_SHIFT 65505
#define LINUX_KEY_BACKSPACE 65288
#define LINUX_KEY_DELETE 65535
#define LINUX_KEY_LEFT_ARROW 65361
#define LINUX_KEY_RIGHT_ARROW 65363
#define LINUX_KEY_TOP_ARROW 65362
#define LINUX_KEY_BOTTOM_ARROW 65364
#define LINUX_KEY_ESCAPE 65307
#define LINUX_KEY_ENTER 65293

#define WIN_KEY_SHIFT 16
#define WIN_KEY_BACKSPACE 8
#define WIN_KEY_DELETE 46
#define WIN_KEY_LEFT_ARROW 37
#define WIN_KEY_RIGHT_ARROW 39
#define WIN_KEY_TOP_ARROW 38
#define WIN_KEY_BOTTOM_ARROW 40
#define WIN_KEY_ESCAPE 27
#define WIN_KEY_ENTER 13

void HandleKey( int keycode, int bDown ) { }
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { }
void HandleDestroy() { }

void th_input_key(int keycode, int bDown) {
#ifdef __linux__
	switch (keycode) {
	case LINUX_KEY_SHIFT:
		keycode = WIN_KEY_SHIFT;
		break;
	case LINUX_KEY_BACKSPACE:
		keycode = WIN_KEY_BACKSPACE;
		break;
	case LINUX_KEY_DELETE:
		keycode = WIN_KEY_DELETE;
		break;
	case LINUX_KEY_LEFT_ARROW:
		keycode = WIN_KEY_LEFT_ARROW;
		break;
	case LINUX_KEY_RIGHT_ARROW:
		keycode = WIN_KEY_RIGHT_ARROW;
		break;
	case LINUX_KEY_TOP_ARROW:
		keycode = WIN_KEY_TOP_ARROW;
		break;
	case LINUX_KEY_BOTTOM_ARROW:
		keycode = WIN_KEY_BOTTOM_ARROW;
		break;
	case LINUX_KEY_ESCAPE:
		keycode = WIN_KEY_ESCAPE;
		break;
	case LINUX_KEY_ENTER:
		keycode = WIN_KEY_ENTER;
		break;
	}
#endif

	if (keycode > 255)
		return;

	if (!bDown) {
		thg->pressed[keycode] = 0;
		thg->just_pressed[keycode] = 0;
		return;
	}

	if (!thg->pressed[keycode]) {
		thg->pressed[keycode] = 1;
		thg->just_pressed[keycode] = 1;
		return;
	}

	thg->just_pressed[keycode] = 0;
}

void th_input_cycle() {
	memset(thg->just_pressed, 0, 255 * sizeof(uu));
}
