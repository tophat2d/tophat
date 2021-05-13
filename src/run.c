#include <stdio.h>
#include <string.h>

#define CNFG_IMPLEMENTATION
#include "../lib/rawdraw/CNFG.h"
#include "../lib/umka/src/umka_api.h"
#include "bindings.h"
#include "audio.h"
#include "misc.h"
#include "input.h"

float scaling;
char *respath;

int destroyfunc;
void *umka;

void HandleDestroy() {
	audeinit();

	if (destroyfunc) {
		umkaCall(umka, destroyfunc, 0, NULL, NULL);
	}
	umkaFree(umka);
}

int main(int argc, char *argv[]) {
	umka = umkaAlloc();
	int umkaOK;	

	char rp[255];
	respath = &rp[0];
	
	FILE *f;
	char scriptpath[20];
	if ((f = fopen("game.um", "r"))) {
		strcpy(respath, "./");
		strcpy(scriptpath, "game.um");
		fclose(f);
	} else if ((f = fopen("main.um", "r"))) {
		strcpy(respath, "./");
		strcpy(scriptpath, "main.um");
		fclose(f);
	} else if ((f = fopen("tophat.dat/game.um", "r"))) {
		strcpy(respath, "tophat.dat/");
		strcpy(scriptpath, "tophat.dat/game.um");
		fclose(f);
	} else {
		errprint("Could not find game.um. Make sure you are in a proper directory.");

		return 1;
	}
	umkaOK = umkaInit(umka, scriptpath, NULL, 1024 * 1024, 1024 * 1024, 0, NULL);

	auinit();

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
	scaling = 1;
	inputinit();

	umkaOK = umkaRun(umka);
	if (!umkaOK) {
		UmkaError error;
		umkaGetError(umka, &error);
		printf("Umka runtime error %s (%d): %s\n", error.fileName, error.line, error.msg);
	}

	return 0;
}
