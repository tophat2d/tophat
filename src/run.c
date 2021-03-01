#include <stdio.h>

#define CNFG_IMPLEMENTATION
#define CNFGRASTERIZER
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"
#include "bindings.h"

void HandleKey( int keycode, int bDown ) { }
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { }
void HandleDestroy() { }

int scaling;

int main() {
	void *umka = umkaAlloc();
	int umkaOK = umkaInit(umka, "game.um", NULL, 1024 * 1024, 1024 * 1024, 0, NULL);
	int gamefunc = 0;

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

	printf("Initialized umka\n");

	gamefunc = umkaGetFunc(umka, NULL, "game");

	printf("Running umka");
	umkaCall(umka, gamefunc, 0, NULL, NULL);

	return 0;
}
