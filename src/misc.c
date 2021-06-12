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
	fprintf(stderr, "\x1b[1m\x1b[31merror: \x1b[0m\n");

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

void th_vector_normalize(float x1, float y1, float x2, float y2, float *rx, float *ry) {
	float diff_x = x2 - x1;
	float diff_y = y2 - y1;


	if (diff_x < diff_y) {
		*rx = 1;
		*ry = diff_y / diff_x;
		return;
	}

	*rx = diff_x / diff_y;
	*ry = 1;
}
