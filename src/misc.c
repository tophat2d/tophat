#include <stdio.h>
#include <math.h>
#include "misc.h"
#ifdef _WIN32
	#include <windows.h>
#else
	#include <unistd.h>
#endif

void slp(int t) {
	printf("%d, %f\n", t, (float)t/1000);
	usleep(t*1000);
}

int getscaling(int w, int h, int camw, int camh) {
	if (w/camw < h/camh) {
		return (int)(w/camw);
	}

	return (int)(h/camh);
}

void errprint(char *text) {
	printf("\033[31merror\033[0m: %s\n", text);
}

void rotatepoint(float *x, float *y, float cx, float cy, float rot) {
	float angle = (rot * M_PI)/180;

	float x1 = *x - cx;
	float y1 = *y - cy;

	float x2 = x1 * cos(angle) - y1 * sin(angle);
	float y2 = x1 * sin(angle) + y1 * cos(angle);

	*x = (int)round(x2) + cx;
	*y = (int)round(y2) + cy;
}
