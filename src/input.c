#include <string.h>
#include "tophat.h"

extern th_global *thg;

// TODO: redo keycode constants in input.um to match sokol
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
}

void th_input_cycle() {
	memset(thg->just_pressed, 0, 255 * sizeof(uu));
	memset(thg->just_released, 0, 255 * sizeof(uu));
}
