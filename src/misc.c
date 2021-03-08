#include <stdio.h>

#include "misc.h"

#ifdef _WIN32
	#include <Windows.h>
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

