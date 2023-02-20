#include "tophat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


extern th_global *thg;

static bool window_active = false;
static bool window_fullscreen = false;

static void th_window_deinit() {
	// TODO
}

static void init() {
	// TODO
}

static void frame() {
	// TODO
}

static void cleanup() {
	// TODO
}

sapp_desc th_window_sapp_desc() {
  return (sapp_desc){
    .init_cb = init,
    .frame_cb = frame,
    .cleanup_cb = cleanup,
    .event_cb = __dbgui_event,
    .width = 640,
    .height = 480,
    .gl_force_gles2 = true,
    .window_title = "Tophat",
    .icon.sokol_default = true,
    .logger.func = slog_func,
  };
}

void th_window_setup(char *name, int w, int h) {
	sapp_set_window_title(name)
	// TODO: Set window size here
}

void th_window_get_dimensions(int *w, int *h) {
	*w = sapp_width();
	*h = sapp_height();
}

bool th_window_is_fullscreen() {
	return sapp_is_fullscreen();
}

void th_window_set_fullscreen(bool fullscreen) {
	if (window_fullscreen != fullscreen) {
		sapp_toggle_fullscreen()
	}
}

int th_window_handle() {
	// TODO
}

void th_window_clear_frame() {
	// TODO
}

void th_window_swap_buffers() {
	// TODO
}

void th_window_set_dims(th_vf2 dm) {
	// TODO
}

void th_window_set_icon(th_image *img) {
	// TODO
}

void th_window_show_cursor(bool show) {
	sapp_show_mouse(show);
}

void th_window_freeze_cursor(bool freeze) {
	sapp_lock_mouse(freeze);
}

void th_window_begin_scissor(int x, int y, size_t w, size_t h) {
	// TODO
}

void th_window_end_scissor() {
	// TODO
}


