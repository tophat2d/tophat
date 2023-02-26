
#define SOKOL_IMPL
#if defined(_MSC_VER)
#define SOKOL_GLCORE33
#define SOKOL_LOG(str) OutputDebugStringA(str)
#elif defined(__EMSCRIPTEN__)
#define SOKOL_GLES2
#elif defined(__APPLE__)
// NOTE: on macOS, sokol.c is compiled explicitly as ObjC
#define SOKOL_METAL
#else
#define SOKOL_GLCORE33
#endif

#include <sokol_app.h>
#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_time.h>
#include <sokol_log.h>

#ifdef __linux__
#include <X11/Xlib.h>
Window *th_sapp_win = &_sapp.x11.window;
Display **th_sapp_dpy = &_sapp.x11.display;
#endif