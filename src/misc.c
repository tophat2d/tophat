#include <stdio.h>
#include <stdarg.h>
#include <math.h>

#include "tophat.h"

float th_get_scaling(int w, int h, int camw, int camh) {
	if ((float)w/camw < (float)h/camh)
		return (float)w/camw;

	return (float)h/camh;
}

void th_error(char *text, ...) {
	fprintf(stderr, "\x1b[1m\x1b[31merror: \x1b[0m");

	va_list args;
	va_start(args, text);
	vfprintf(stderr, text, args);
	va_end(args);
	fprintf(stderr, "\n");
}

void th_rotate_point(th_vf2 *p, th_vf2 o, fu rot) {
	float angle = (rot * M_PI)/180;

	p->x -= o.x;
	p->y -= o.y;

	fu x = p->x * cos(angle) - p->y * sin(angle);
	fu y = p->x * sin(angle) + p->y * cos(angle);

	p->x = o.x + x;
	p->y = o.y + y;
}

void th_vector_normalize(float *x, float *y) {
	if (fabs(*x) > fabs(*y)) {
		*y /= fabs(*x);
		*x /= fabs(*x);
	} else {
		*x /= fabs(*y);
		*y /= fabs(*y);
	}
}
