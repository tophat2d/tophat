#include "rect.h"

rect newrect(int px, int py, int w, int h) {
	rect tr;
	tr.x = px;
	tr.y = py;
	tr.w = w;
	tr.h = h;
	return tr;
}

