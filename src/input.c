#include <string.h>
#include "tophat.h"

extern th_global *thg;

#define ANY_CONTROL 16
#define ANY_SHIFT   17
#define ANY_ALT     18

void th_input_key(int keycode, int bDown) {
	if (!bDown) {
		if (thg->pressed[keycode])
			thg->just_released[keycode] = 1;
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

	// emit fake scancodes for any ctrl/shift/alt
	thg->pressed[ANY_CONTROL] = thg->pressed[SAPP_KEYCODE_LEFT_CONTROL] || thg->pressed[SAPP_KEYCODE_RIGHT_CONTROL];
	thg->pressed[ANY_SHIFT] = thg->pressed[SAPP_KEYCODE_LEFT_SHIFT] || thg->pressed[SAPP_KEYCODE_RIGHT_SHIFT];
	thg->pressed[ANY_ALT] = thg->pressed[SAPP_KEYCODE_LEFT_ALT] || thg->pressed[SAPP_KEYCODE_RIGHT_ALT];

	thg->just_pressed[ANY_CONTROL] = thg->just_pressed[SAPP_KEYCODE_LEFT_CONTROL] || thg->just_pressed[SAPP_KEYCODE_RIGHT_CONTROL];
	thg->just_pressed[ANY_SHIFT] = thg->just_pressed[SAPP_KEYCODE_LEFT_SHIFT] || thg->just_pressed[SAPP_KEYCODE_RIGHT_SHIFT];
	thg->just_pressed[ANY_ALT] = thg->just_pressed[SAPP_KEYCODE_LEFT_ALT] || thg->just_pressed[SAPP_KEYCODE_RIGHT_ALT];
}

void th_input_cycle() {
	thg->mouse_wheel = (th_vf2){0, 0};
	memset(thg->just_pressed, 0, 512 * sizeof(uu));
	memset(thg->just_released, 0, 512 * sizeof(uu));
}
