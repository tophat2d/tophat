#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb/stb_image.h"

#include <SDL2/SDL.h>
#include <stdio.h>


#include "object.h"

extern SDL_Window* win;
extern SDL_Surface* surface;

int load(char path[], obj *o) {
	SDL_Surface *tr = SDL_LoadBMP(path);
	if (tr == NULL) {
		return 1;
	}
	o->image = *tr;
	return 0;
}

void draw(obj o) {
	SDL_Rect dstrect, srcrect;

	dstrect.x = o.px;
	dstrect.y = o.py;
	dstrect.w = o.image.w;
	dstrect.h = o.image.h;
	srcrect.x = 0;
	srcrect.y = 0;
	srcrect.w = o.image.w;
	srcrect.h = o.image.h;

	SDL_BlitSurface(o.image, &srcrect, surface, &dstrect);
}
