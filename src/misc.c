#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "tophat.h"

extern th_global *thg;

void
th_calculate_scaling(float camw, float camh)
{
	int w, h;
	th_window_get_dimensions(&w, &h);

	if (w * h * camw * camh == 0)
		return;

	if ((float)w / camw < (float)h / camh) {
		thg->scaling = ((float)w / camw);
	} else {
		thg->scaling = ((float)h / camh);
	}

	thg->offset.x = (w - camw * thg->scaling) / 2;
	thg->offset.y = (h - camh * thg->scaling) / 2;
}

float
th_get_scaling(int w, int h, int camw, int camh)
{
	if ((float)w / camw < (float)h / camh)
		return (float)w / camw;

	return (float)h / camh;
}

void
th_error(char *text, ...)
{
#ifdef __EMSCRIPTEN__
	fprintf(stderr, "error: ");
#else
	fprintf(stderr, "\x1b[1m\x1b[31merror: \x1b[0m");
#endif

	va_list args;
	va_start(args, text);
#if defined(_WIN32) && !defined(_CONSOLE)
#include <winuser.h>
	char buf[4096];
	vsnprintf(buf, 4096, text, args);
	MessageBox(NULL, buf, "tophat error", 0x10);
#endif
	vfprintf(stderr, text, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void
th_info(char *text, ...)
{
	va_list args;
	va_start(args, text);
#if defined(_WIN32) && !defined(_CONSOLE)
#include <winuser.h>
	char buf[4096];
	vsnprintf(buf, 4096, text, args);
	MessageBox(NULL, buf, "information", 0x40);
#endif
	vfprintf(stderr, text, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void
th_rotate_point(th_vf2 *p, th_vf2 o, fu rot)
{
	const float angle = (rot * PI) / 180;

	const fu cosa = cos(angle);
	const fu sina = sin(angle);

	p->x -= o.x;
	p->y -= o.y;

	const fu x = p->x * cosa - p->y * sina;
	const fu y = p->x * sina + p->y * cosa;

	p->x = o.x + x;
	p->y = o.y + y;
}

void
th_vector_normalize(float *x, float *y)
{
	float fx = fabs(*x);
	float fy = fabs(*y);

	if (fx > fy) {
		*x /= fx;
		*y /= fx;
	} else {
		*x /= fy;
		*y /= fy;
	}
}

// implemented in umka_common.c
bool
moduleRegularizePath(const char *path, const char *curFolder, char *regularizedPath, int size);

th_err
th_regularize_path(const char *path, const char *cur_folder, char *regularized_path, int size)
{
	if (!moduleRegularizePath(path, cur_folder, regularized_path, size)) {
		return 1;
	}

	return 0;
}
