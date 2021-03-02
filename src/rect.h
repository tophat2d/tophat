#ifndef RECT_H
#define RECT_H

typedef struct {
	int x;
	int y;
	int w;
	int h;
} rect;

rect newrect(int px, int py, int w, int h);

#endif
