#define SOKOL_NO_ENTRY
#define SOKOL_IMPL

#include <TargetConditionals.h>

#if (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE)
#	define SOKOL_GLES3
#else
#	define SOKOL_GLCORE
# define TOPHAT_MACOS
#endif

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_log.h>
#include <sokol_time.h>

int *th_sapp_swap_interval = &_sapp.swap_interval;

GLuint
th_sg_get_gl_image(sg_image img)
{
	return *_sg_image_at(img.id)->gl.tex;
}

void
th_sapp_set_window_dims(const void *_window, int w, int h)
{
	NSWindow *window = (__bridge NSWindow *)_window;
	[window setContentSize:NSMakeSize(w, h)];
}

void
th_sapp_set_window_hidden(const void *_window, bool hidden)
{
	NSWindow *window = (__bridge NSWindow *)_window;
	// TODO: should hide the window but sokol applicationShouldTerminateAfterLastWindowClosed returns yes so that exits the process
	[window setIsMiniaturized:(hidden)];
}
