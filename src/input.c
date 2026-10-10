#include "tophat.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#include <xinput.h>
#endif
#include <math.h>

extern th_global *thg;

#define ANY_CONTROL 16
#define ANY_SHIFT 17
#define ANY_ALT 18
#define ANY_SUPER 19
#define ANY_CTRL_COMMAND 20

void
th_input_key(int keycode, int bDown)
{
	if (!bDown) {
		thg->just_released[keycode] = thg->internal_pressed_state[keycode];
		thg->pressed[keycode] = 0;
		thg->internal_pressed_state[keycode] = 0;
		thg->just_pressed[keycode] = 0;
		return;
	}

	if (!thg->pressed[keycode]) {
		thg->pressed[keycode] = 1;
		thg->internal_pressed_state[keycode] = 1;
		thg->just_pressed[keycode] = 1;
		return;
	}

	thg->just_pressed[keycode] = 0;
}

static th_touch_finger*
acquire_touch_finger(size_t id) {
	// creae if doesn't exist

	for (int i = 0; i < MAX_TOUCH_FINGERS; i++) {
		if (!thg->touch_fingers[i].valid || (thg->touch_fingers[i].valid && thg->touch_fingers[i].id == id)) {
			memset(&thg->touch_fingers[i], 0, sizeof(th_touch_finger));
			thg->touch_fingers[i].id = id;
			thg->touch_fingers[i].valid = true;
			return &thg->touch_fingers[i];
		}
	}

	assert(false && "no free touch finger");
	return NULL;
}

static void
release_touch_finger(size_t id) {
	for (int i = 0; i < MAX_TOUCH_FINGERS; i++) {
		if (thg->touch_fingers[i].id == id) {
			thg->touch_fingers[i].pending_free = true;
			return;
		}
	}

	assert(false && "releaseing invalid finger");
}

void
th_input_touch(const sapp_event *ev)
{
	for (int i = 0; i < ev->num_touches; i++) {
		th_touch_finger *finger = acquire_touch_finger(ev->touches[i].identifier);
		finger->position.x = (ev->touches[i].pos_x - thg->offset.x) / thg->scaling;
		finger->position.y = (ev->touches[i].pos_y - thg->offset.y) / thg->scaling;

		switch (ev->type) {
		case SAPP_EVENTTYPE_TOUCHES_BEGAN:
			finger->just_pressed = true;
			finger->pressed = true;
			break;
		case SAPP_EVENTTYPE_TOUCHES_MOVED:
			finger->just_pressed = false;
			break;
		case SAPP_EVENTTYPE_TOUCHES_ENDED:
			finger->just_released = true;
			finger->pressed = false;
			release_touch_finger(ev->touches[i].identifier);
			break;
		case SAPP_EVENTTYPE_TOUCHES_CANCELLED:
			finger->pressed = false;
			release_touch_finger(ev->touches[i].identifier);
			break;
		default:
			assert(false && "non touch event sneaked in");
		}
	}
}

void
th_input_repeated(int keycode, int bDown)
{
	if (bDown) {
		thg->press_repeat[keycode] = true;

		switch (keycode) {
		case SAPP_KEYCODE_LEFT_CONTROL:
		case SAPP_KEYCODE_RIGHT_CONTROL: thg->press_repeat[ANY_CONTROL] = true; break;
		case SAPP_KEYCODE_LEFT_SHIFT:
		case SAPP_KEYCODE_RIGHT_SHIFT: thg->press_repeat[ANY_SHIFT] = true; break;
		case SAPP_KEYCODE_LEFT_ALT:
		case SAPP_KEYCODE_RIGHT_ALT: thg->press_repeat[ANY_ALT] = true; break;
		}
	}
}

void
th_input_modifiers(uint32_t modifiers)
{
	// Release the keys that might have stuck.
	if ((modifiers & SAPP_MODIFIER_CTRL) == 0) {
		th_input_key(SAPP_KEYCODE_LEFT_CONTROL, false);
		th_input_key(SAPP_KEYCODE_RIGHT_CONTROL, false);
	}
	if ((modifiers & SAPP_MODIFIER_SHIFT) == 0) {
		th_input_key(SAPP_KEYCODE_LEFT_SHIFT, false);
		th_input_key(SAPP_KEYCODE_RIGHT_SHIFT, false);
	}
	if ((modifiers & SAPP_MODIFIER_ALT) == 0) { // Mac: option
		th_input_key(SAPP_KEYCODE_LEFT_ALT, false);
		th_input_key(SAPP_KEYCODE_RIGHT_ALT, false);
	}
	if ((modifiers & SAPP_MODIFIER_SUPER) == 0) { // Mac: command
		th_input_key(SAPP_KEYCODE_LEFT_SUPER, false);
		th_input_key(SAPP_KEYCODE_RIGHT_SUPER, false);
	}

	// Update fake keys.
	th_input_key(ANY_CONTROL, modifiers & SAPP_MODIFIER_CTRL);
	th_input_key(ANY_SHIFT, modifiers & SAPP_MODIFIER_SHIFT);
	th_input_key(ANY_ALT, modifiers & SAPP_MODIFIER_ALT);
	th_input_key(ANY_SUPER, modifiers & SAPP_MODIFIER_SUPER);

	#if __APPLE__
	th_input_key(ANY_CTRL_COMMAND, modifiers & SAPP_MODIFIER_SUPER);
	th_input_key(ANY_CTRL_COMMAND, modifiers & SAPP_MODIFIER_SUPER);
	#else
	th_input_key(ANY_CTRL_COMMAND, modifiers & SAPP_MODIFIER_CTRL);
	th_input_key(ANY_CTRL_COMMAND, modifiers & SAPP_MODIFIER_CTRL);
	#endif
}

void
th_input_cycle()
{
	thg->mouse_wheel = (th_vf2){{0, 0}};
	*thg->input_string = 0;
	thg->input_string_len = 0;
	memset(thg->just_pressed, 0, 512 * sizeof(uu));
	memset(thg->just_released, 0, 512 * sizeof(uu));
	memset(thg->press_repeat, 0, 512 * sizeof(uu));

	for (int i = 0; i < MAX_TOUCH_FINGERS; i++) {
		th_touch_finger *finger = &thg->touch_fingers[i];

		if (finger->valid) {
			if (finger->pending_free) {
				finger->valid = false;
			}

			thg->touch_fingers[i].just_pressed = false;
			thg->touch_fingers[i].just_released = false;
		}
	}
}

void
th_input_reset()
{
	th_input_cycle();
	memset(thg->pressed, 0, 512 * sizeof(uu));
}

// Deadzone and drift fix
static float
i_fix(float value)
{
	if (fabs(value) < 0.08)
		return 0;
	return value;
}

static void
i_update_button(th_gamepad_button *btn, float value)
{
	btn->just_pressed = !btn->pressed && value > 0.5f;
	btn->just_released = btn->pressed && value <= 0.5f;
	btn->pressed = value > 0.5f;
	btn->pressure = value;
}

#ifdef _WIN32
typedef DWORD(WINAPI *XInputGetState_t)(DWORD, XINPUT_STATE *);
XInputGetState_t _th_XInputGetState = NULL;
typedef DWORD(WINAPI *XInputSetState_t)(DWORD, XINPUT_VIBRATION *);
XInputSetState_t _th_XInputSetState = NULL;
#endif

void
th_input_update_gamepads()
{
#ifdef _WIN32
	if (!_th_XInputGetState)
		return;

	XINPUT_STATE state;
	for (int i = 0; i < MAX_GAMEPADS; i++) {
		state = (XINPUT_STATE){0};

		th_generic_gamepad *gp = &thg->gamepad[i];

		if (_th_XInputGetState(i, &state) == ERROR_SUCCESS) {
			XINPUT_VIBRATION vib = {0};
			vib.wLeftMotorSpeed = (gp->rumble_left * 65535);
			vib.wRightMotorSpeed = (gp->rumble_right * 65535);
			_th_XInputSetState(i, &vib);

			gp->rumble_left = 0;
			gp->rumble_right = 0;

			gp->connected = true;

			XINPUT_GAMEPAD xgp = state.Gamepad;

			i_update_button(&gp->dpad_up, (xgp.wButtons & XINPUT_GAMEPAD_DPAD_UP) > 0);
			i_update_button(
			    &gp->dpad_down, (xgp.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) > 0);
			i_update_button(
			    &gp->dpad_left, (xgp.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) > 0);
			i_update_button(
			    &gp->dpad_right, (xgp.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) > 0);

			i_update_button(&gp->start, (xgp.wButtons & XINPUT_GAMEPAD_START) > 0);
			i_update_button(&gp->select, (xgp.wButtons & XINPUT_GAMEPAD_BACK) > 0);

			i_update_button(
			    &gp->left_stick_press, (xgp.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) > 0);
			i_update_button(&gp->right_stick_press,
			    (xgp.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) > 0);

			i_update_button(&gp->LB, (xgp.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) > 0);
			i_update_button(
			    &gp->RB, (xgp.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) > 0);

			i_update_button(&gp->A, (xgp.wButtons & XINPUT_GAMEPAD_A) > 0);
			i_update_button(&gp->B, (xgp.wButtons & XINPUT_GAMEPAD_B) > 0);
			i_update_button(&gp->X, (xgp.wButtons & XINPUT_GAMEPAD_X) > 0);
			i_update_button(&gp->Y, (xgp.wButtons & XINPUT_GAMEPAD_Y) > 0);

			i_update_button(&gp->LT, (float)xgp.bLeftTrigger / 255.0f);
			i_update_button(&gp->RT, (float)xgp.bRightTrigger / 255.0f);

			gp->left_stick.x = i_fix((float)xgp.sThumbLX / 32768.0f);
			gp->left_stick.y = i_fix((float)xgp.sThumbLY / 32768.0f);

			gp->right_stick.x = i_fix((float)xgp.sThumbRX / 32768.0f);
			gp->right_stick.y = i_fix((float)xgp.sThumbRY / 32768.0f);
		} else {
			gp->connected = false;
		}
	}
#endif
}

void
th_input_init(void)
{
#ifdef _WIN32
	HMODULE xinput = LoadLibraryA("xinput1_4.dll");
	if (!xinput) {
		printf("Failed to load xinput1_4.dll, falling back to xinput1_3.dll\n");
		xinput = LoadLibraryA("xinput1_3.dll");
		if (!xinput) {
			printf("Failed to load xinput1_3.dll, gamepad support will be disabled\n");
			return;
		}
	}

	_th_XInputGetState = (XInputGetState_t)GetProcAddress(xinput, "XInputGetState");
	_th_XInputSetState = (XInputSetState_t)GetProcAddress(xinput, "XInputSetState");
#endif
}
