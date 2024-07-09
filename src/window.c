#include "tophat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_log.h>
#include <umka_api.h>

#include <umprof.h>
#define MSG_LEN 1024

extern th_global *thg;

void
th_print_umka_error_and_quit(int code)
{
	UmkaError *error = umkaGetError(thg->umka);

	if (error->code != 0 && error->msg[0]) {
		th_error("%s (%d): %s\n", error->fileName, error->line, error->msg);

		fprintf(stderr, "\tStack trace:\n");

		for (int depth = 0; depth < 10; depth++) {
			char fnName[MSG_LEN + 1];
			char file[MSG_LEN + 1];
			int line, offset;

			if (!umkaGetCallStack(
				thg->umka, depth, MSG_LEN + 1, &offset, file, fnName, &line)) {
				fprintf(stderr, "\t\t...\n");
				break;
			}
#ifndef _WIN32
			fprintf(stderr, "\033[34m");
#endif
			fprintf(stderr, "\t\t%s:%06d: ", file, line);
#ifndef _WIN32
			fprintf(stderr, "\033[0m");
#endif
			fprintf(stderr, "%s\n", fnName);
		}
	}

	th_deinit();
	exit(code);
}

static void
init()
{
	th_audio_init();
	sg_setup(&(sg_desc){
	    .environment = sglue_environment(),
	    .logger.func = slog_func,
	});

	thg->dpi_scale_factor = sapp_dpi_scale();

	th_nav_init();
	th_canvas_init();
	th_image_init();

	UmkaStackSlot s;

	if (umkaAlive(thg->umka)) {
		int code = umkaCall(
		    thg->umka, umkaGetFunc(thg->umka, "tophat_main.um", "__th_init"), 0, &s, &s);
		if (!umkaAlive(thg->umka)) {
			th_print_umka_error_and_quit(code);
		}
	}
}

static void
frame()
{
	thg->dpi_scale_factor = sapp_dpi_scale();
	th_input_update_gamepads();

	thg->pass_action = (sg_pass_action){
	    .colors[0] =
		{
		    .load_action = SG_LOADACTION_LOAD,
		    .store_action = SG_STOREACTION_STORE,
		},
	};
	sg_begin_pass(&(sg_pass){
	    .action = thg->pass_action,
	    .swapchain = sglue_swapchain(),
	});
	sg_apply_pipeline(thg->canvas_pip);
	int window_width, window_height;
	th_window_get_dimensions(&window_width, &window_height);
	thg->target_size = (th_vf2){.w = window_width, .h = window_height};

	UmkaStackSlot s;
	if (thg->umka && umkaAlive(thg->umka) && thg->umth_frame_callback != -1) {
		s.realVal = sapp_frame_duration();

		int code = umkaCall(thg->umka, thg->umth_frame_callback, 1, &s, &s);
		if (!umkaAlive(thg->umka)) {
			th_print_umka_error_and_quit(code);
		}
	}

	thg->mouse_delta = (th_vf2){{0}};

	th_input_cycle();
	th_canvas_flush();
	th_canvas_end_frame();
	sg_end_pass();
	sg_commit();
}

static void
event(const sapp_event *ev)
{
	switch (ev->type) {
	case SAPP_EVENTTYPE_UNFOCUSED: th_input_reset(); break;
	case SAPP_EVENTTYPE_MOUSE_MOVE:
		thg->mouse_delta = (th_vf2){.x = ev->mouse_dx, .y = ev->mouse_dy};
		thg->mouse = (th_vf2){.x = ev->mouse_x, .y = ev->mouse_y};
		break;
	case SAPP_EVENTTYPE_CHAR:
		if (ev->char_code < ' ' || ev->char_code == 127 /* DEL character */)
			break;

		thg->input_string_len = th_utf8_encode(thg->input_string, ev->char_code);
		break;
	case SAPP_EVENTTYPE_KEY_DOWN:
	case SAPP_EVENTTYPE_KEY_UP:
		th_input_repeated(ev->key_code, ev->type == SAPP_EVENTTYPE_KEY_DOWN);
		th_input_modifiers(ev->modifiers);
		if (ev->key_repeat)
			break;

		th_input_key(ev->key_code, ev->type == SAPP_EVENTTYPE_KEY_DOWN);
		break;
	case SAPP_EVENTTYPE_MOUSE_SCROLL:
		thg->mouse_wheel.x = ev->scroll_x;
		thg->mouse_wheel.y = ev->scroll_y;
		break;
	case SAPP_EVENTTYPE_MOUSE_DOWN:
	case SAPP_EVENTTYPE_MOUSE_UP:
		th_input_key(ev->mouse_button + 1, ev->type == SAPP_EVENTTYPE_MOUSE_DOWN);
		break;
	}
}

static void
cleanup()
{
	th_deinit();
	sg_shutdown();
}

sapp_desc
th_window_sapp_desc()
{
	return (sapp_desc){
	    .init_cb = init,
	    .frame_cb = frame,
	    .cleanup_cb = cleanup,
	    .event_cb = event,
	    .width = 640,
	    .height = 480,
	    .window_title = "Tophat",
	    .enable_clipboard = true,
	    .clipboard_size = 8192,
	    .icon.sokol_default = true,
	    .logger.func = slog_func,
	    .high_dpi = thg->dpi_aware,
#ifdef __EMSCRIPTEN__
	    .html5_bubble_mouse_events = true,
	    .html5_bubble_touch_events = true,
	    .html5_bubble_wheel_events = true,
	    .html5_bubble_key_events = true,
	    .html5_bubble_char_events = true,
#endif
	};
}

fu
th_window_dpi_scale()
{
	return thg->dpi_scale_factor;
}

void
th_window_setup(char *name, int w, int h)
{
	sapp_set_window_title(name);
	th_window_set_dims((th_vf2){{w, h}});
}

void
th_window_get_dimensions(int *w, int *h)
{
	*w = sapp_width();
	*h = sapp_height();
}

bool
th_window_is_fullscreen()
{
	return sapp_is_fullscreen();
}

void
th_window_set_fullscreen(bool fullscreen)
{
	if (sapp_is_fullscreen() != fullscreen) {
		sapp_toggle_fullscreen();
	}
}

void
th_window_set_icon(th_image *img)
{
	uint32_t *pixels = th_image_get_data(img);

	sapp_set_icon(
	    &(sapp_icon_desc){.images = {(sapp_image_desc){.width = img->dm.x,
				  .height = img->dm.y,
				  .pixels = (sapp_range){.ptr = pixels,
				      .size = img->dm.x * img->dm.y * sizeof(uint32_t)}}}});

	free(pixels);
}

void
th_window_show_cursor(bool show)
{
	sapp_show_mouse(show);
}

void
th_window_freeze_cursor(bool freeze)
{
	sapp_lock_mouse(freeze);
}

void
th_window_set_cursor(int cursor)
{
	if (cursor < 0 || cursor >= _SAPP_MOUSECURSOR_NUM) {
		th_error("missing cursor with number: %d\n", cursor);
		return;
	}

	// - Cursor IDs in tophat are identical to sokols
	sapp_set_mouse_cursor(cursor);
}

void
th_window_request_exit()
{
	sapp_request_quit();
}

// ---- PLATFORM DEPENDENT CODE

#ifdef _WIN32
th_window_handle
th_get_window_handle()
{
	return sapp_win32_get_hwnd();
}

void
w32_get_client_window_size(int *w, int *h)
{
	RECT rect = {0};
	rect.right = *w;
	rect.bottom = *h;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	*w = rect.right - rect.left;
	*h = rect.bottom - rect.top;
}

void
th_window_set_dims(th_vf2 dm)
{
	RECT r;
	int w = dm.x, h = dm.y;
	w32_get_client_window_size(&w, &h);
	HWND hwnd = th_get_window_handle();
	if (GetWindowRect(hwnd, &r)) {
		SetWindowPos(hwnd, HWND_TOP, r.left, r.top, w, h, 0);
	}
}
#elif __linux__
extern Window *th_sapp_win;
extern Display **th_sapp_dpy;
th_window_handle
th_get_window_handle()
{
	return th_sapp_win;
}

void
th_window_set_dims(th_vf2 dm)
{
	XResizeWindow(*th_sapp_dpy, *th_sapp_win, dm.x, dm.y);
}
#elif defined(__EMSCRIPTEN__)
th_window_handle
th_get_window_handle()
{
	return 0;
}

void
th_window_set_dims(th_vf2 dm)
{
	// FIXME: FAKE
}
#else
#error Unsupported platform
#endif
