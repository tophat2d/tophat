#include <stdio.h>
#include <string.h>

#define CNFG_IMPLEMENTATION
#define CNFRASTERIZER
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"
#include "bindings.h"

float scaling;
int *pressed;
int *justpressed;
char *respath;

void HandleKey(int keycode, int bDown) {
	if (keycode > 255) {
		//TODO: switch case with another options
		return;
	}

	if (!bDown) {
		pressed[keycode] = 0;
		justpressed[keycode] = 0;
		return;
	}

	if (!pressed[keycode]) {
		pressed[keycode] = 1;
		justpressed[keycode] = 1;
		return;
	}

	justpressed[keycode] = 0;
}
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { }
void HandleDestroy() { }

int main(int argc, char *argv[]) {
	void *umka = umkaAlloc();
	int umkaOK;	
	int gamefunc = 0;
	scaling = 1;

	int pa[255];
	int jpa[255];
	pressed = &pa[0];
	justpressed = &jpa[0];

	if (argc > 1) {
		respath = malloc(sizeof(char) * 2);
		strcpy(respath, "./");
		umkaOK = umkaInit(umka, "game.um", NULL, 1024 * 1024, 1024 * 1024, 0, NULL);
	} else {
		respath = malloc(sizeof(char) * strlen(strcat(argv[0], ".dat/")));
		strcpy(respath, strcat(argv[0], ".dat/"));
		umkaOK = umkaInit(umka, strcat(argv[0], ".dat/game.um"), NULL, 1024 * 1024, 1024 * 1024, 0, NULL);
	}

	if (!umkaOK) {
		printf("Could not initialize umka.\n");
		return 1;
	}

	bind(umka);
	umkaOK = umkaCompile(umka);

	if (!umkaOK) {
		UmkaError error;
		umkaGetError(umka, &error);
		printf("Umka error %s (%d, %d): %s\n", error.fileName, error.line, error.pos, error.msg);
		return 1;
	}

	gamefunc = umkaGetFunc(umka, NULL, "game");

	umkaCall(umka, gamefunc, 0, NULL, NULL);

	umkaFree(umka);

	return 0;
}
