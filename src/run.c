#include <stdio.h>
#include <string.h>

#define CNFG_IMPLEMENTATION
#define CNFGOGL
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"
#include "bindings.h"
//#include "audio.h"

float scaling;
int *pressed;
int *justpressed;
char *respath;

int mx;
int my;

int destroyfunc;
void *umka;

void HandleKey(int keycode, int bDown) {
	if (keycode > 255) {
		switch (keycode) {
		case 65288:
			keycode = 0;
			break;
		case 65307:
			keycode = 1;
			break;
		case 65293:
			keycode = 2;
			break;
		case 65362:
			keycode = 3;
			break;
		case 65364:
			keycode = 4;
			break;
		case 65361:
			keycode = 5;
			break;
		case 65363:
			keycode = 6;
			break;
		default:
			return;
		}
	}

	printf("%d\n", keycode);

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
void HandleMotion( int x, int y, int mask ) {
	mx = x;
	my = y;
}
void HandleDestroy() {
	//audeinit();
	umkaCall(umka, destroyfunc, 0, NULL, NULL);
}

int main(int argc, char *argv[]) {
	umka = umkaAlloc();
	int umkaOK;	
	scaling = 1;

	int pa[255];
	int jpa[255];
	pressed = &pa[0];
	justpressed = &jpa[0];

	memset(pressed, 0, 255 * sizeof(int));
	memset(justpressed, 0, 255 * sizeof(int));

	
	if (argc > 1) {
		respath = malloc(sizeof(char) * 2);
		strcpy(respath, "./");
		umkaOK = umkaInit(umka, "game.um", NULL, 1024 * 1024, 1024 * 1024, 0, NULL);
	} else {
		char scriptpath[50];
		respath = malloc(sizeof(char) * strlen(strcat(argv[0], ".dat/")));
		respath = argv[0];
		strcpy(scriptpath, respath);
		umkaOK = umkaInit(umka, strcat(scriptpath, "game.um"), NULL, 1024 * 1024, 1024 * 1024, 0, NULL);
	}

	if (!umkaOK) {
		printf("Could not initialize umka.\n");
		return 1;
	}

	umkabind(umka);
	umkaOK = umkaCompile(umka);

	if (!umkaOK) {
		UmkaError error;
		umkaGetError(umka, &error);
		printf("Umka error %s (%d, %d): %s\n", error.fileName, error.line, error.pos, error.msg);
		return 1;
	}

	destroyfunc = umkaGetFunc(umka, NULL, "windowdestroy");

	umkaRun(umka);

	umkaFree(umka);

	return 0;
}
