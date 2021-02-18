#include "misc.h"

int getscaling(int w, int h, int camw, int camh) {
	if (w/camw < h/camh) {
		return (int)(w/camw);
	}

	return (int)(h/camh);
}
