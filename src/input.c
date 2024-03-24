#include "tophat.h"
#include <string.h>
#ifdef _WIN32
#include <Xinput.h>
#endif

extern th_global *thg;

#define ANY_CONTROL 16
#define ANY_SHIFT 17
#define ANY_ALT 18

void
th_input_sync_fake_keys()
{
	// emit fake scancodes for any ctrl/shift/alt
	thg->pressed[ANY_CONTROL] =
	    thg->pressed[SAPP_KEYCODE_LEFT_CONTROL] || thg->pressed[SAPP_KEYCODE_RIGHT_CONTROL];
	thg->pressed[ANY_SHIFT] =
	    thg->pressed[SAPP_KEYCODE_LEFT_SHIFT] || thg->pressed[SAPP_KEYCODE_RIGHT_SHIFT];
	thg->pressed[ANY_ALT] =
	    thg->pressed[SAPP_KEYCODE_LEFT_ALT] || thg->pressed[SAPP_KEYCODE_RIGHT_ALT];

	thg->just_pressed[ANY_CONTROL] = thg->just_pressed[SAPP_KEYCODE_LEFT_CONTROL] ||
	    thg->just_pressed[SAPP_KEYCODE_RIGHT_CONTROL];
	thg->just_pressed[ANY_SHIFT] = thg->just_pressed[SAPP_KEYCODE_LEFT_SHIFT] ||
	    thg->just_pressed[SAPP_KEYCODE_RIGHT_SHIFT];
	thg->just_pressed[ANY_ALT] =
	    thg->just_pressed[SAPP_KEYCODE_LEFT_ALT] || thg->just_pressed[SAPP_KEYCODE_RIGHT_ALT];
}

void
th_input_key(int keycode, int bDown)
{
	if (!bDown) {
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

void
th_input_repeated(int keycode, int bDown)
{
	if (bDown) {
		thg->press_repeat[keycode] = true;
	}
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

void
th_input_update_gamepads()
{
#ifdef _WIN32
	XINPUT_STATE state;
	for (int i = 0; i < 4; i++) {
		state = (XINPUT_STATE){0};

		th_generic_gamepad *gp = &thg->gamepad[i];

		if (XInputGetState(i, &state) == ERROR_SUCCESS) {
			XINPUT_VIBRATION vib = {0};
			vib.wLeftMotorSpeed = (gp->rumble_left * 65535);
			vib.wRightMotorSpeed = (gp->rumble_right * 65535);
			XInputSetState(i, &vib);

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
