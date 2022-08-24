#include <string.h>
#include "tophat.h"

extern th_global *thg;

#ifdef __linux__
#include <X11/keysym.h>

#define LINUX_KEY_SHIFT XK_Shift_L
#define LINUX_KEY_BACKSPACE XK_BackSpace
#define LINUX_KEY_DELETE XK_Delete
#define LINUX_KEY_LEFT_ARROW XK_Left
#define LINUX_KEY_RIGHT_ARROW XK_Right
#define LINUX_KEY_TOP_ARROW XK_Up
#define LINUX_KEY_BOTTOM_ARROW XK_Down
#define LINUX_KEY_ESCAPE XK_Escape
#define LINUX_KEY_ENTER XK_Return
#define LINUX_KEY_ALT XK_Alt_L
#define LINUX_KEY_CTRL XK_Control_L
#endif

#define WIN_KEY_SHIFT (16 + 0x7f)
#define WIN_KEY_BACKSPACE (8 + 0x7f)
#define WIN_KEY_DELETE (46 + 0x7f)
#define WIN_KEY_LEFT_ARROW (37 + 0x7f)
#define WIN_KEY_RIGHT_ARROW (39 + 0x7f)
#define WIN_KEY_TOP_ARROW (38 + 0x7f)
#define WIN_KEY_BOTTOM_ARROW (40 + 0x7f)
#define WIN_KEY_ESCAPE (27 + 0x7f)
#define WIN_KEY_ENTER (13 + 0x7f)
#define WIN_KEY_ALT (0x12 + 0x7f)
#define WIN_KEY_CTRL (0x11 + 0x7f)

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
	case LINUX_KEY_ALT:
		keycode = WIN_KEY_ALT;
		break;
	case LINUX_KEY_CTRL:
		keycode = WIN_KEY_CTRL;
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
