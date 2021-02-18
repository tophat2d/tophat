#ifndef OBJECT_H
#define OBJECT_H

typedef struct {
	int px;
	int py;
	int w;
	int h;
	int c;
	char *image;
	uint32_t color;
} entity;

entity load(char path[]);
void draw(entity o);

#endif
