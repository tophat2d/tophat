#include "object.h"

#include <stdio.h>
#include <SDL2/SDL.h>

SDL_Window* win;
SDL_Surface* surface;

// this will be replaced with init function
int main() {
	SDL_Init(SDL_INIT_VIDEO);

	win = SDL_CreateWindow("tophat", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 60, 60, SDL_WINDOW_SHOWN);

	surface = SDL_GetWindowSurface(win);

	SDL_DestroyWindow(win);
	SDL_Quit();

	return 0;
}
