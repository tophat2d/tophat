#ifndef OBJECT_H
#define OBJECT_H

typedef struct {
	int px;
	int py;
	int w;
	int h;
	char *image;
} obj;

obj load(char path[]);
void draw(obj o);

#endif
