#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "misc.h"

float th_get_scaling(int w, int h, int camw, int camh) {
	if ((float)w/camw < (float)h/camh)
		return (float)w/camw;

	return (float)h/camh;
}

// TODO: allow for formatted text
void th_error(char *text, ...) {
	fprintf(stderr, "\x1b[1m\x1b[31merror: \x1b[0m");

	va_list args;
	va_start(args, text);
	vfprintf(stderr, text, args);
	va_end(args);
}

void th_rotate_point(float *x, float *y, float cx, float cy, float rot) {
	float angle = (rot * M_PI)/180;

	float x1 = *x - cx;
	float y1 = *y - cy;

	float x2 = x1 * cos(angle) - y1 * sin(angle);
	float y2 = x1 * sin(angle) + y1 * cos(angle);

	*x = (int)round(x2) + cx;
	*y = (int)round(y2) + cy;
}
