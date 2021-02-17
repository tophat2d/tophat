#ifndef OBJECT_H
#define OBJECT_H

typedef struct {
	int px;
	int py;
	SDL_Surface image;
} obj;

int load(char path[], obj *o);
void draw(obj o);

#endif
