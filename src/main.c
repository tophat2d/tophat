#include <stdio.h>
#include <string.h>

#define CNFG_IMPLEMENTATION
#include <CNFG.h>
#include <umka_api.h>
#include "bindings.h"
#include "tophat.h"

th_global thg = {0};
int destroyfunc;
int died = 0;

void die() {
	if (died) return;
	
	th_audio_deinit();
	if (destroyfunc) {
		umkaCall(thg.umka, destroyfunc, 0, NULL, NULL);
	}

	umkaFree(thg.umka);
	died = 1;
}

void HandleDestroy() {
	die();
}

int main(int argc, char *argv[]) {
	thg.umka = umkaAlloc();
	int umkaOK;

	FILE *f;
	char scriptpath[512];
	if ((f = fopen("main.um", "r"))) {
		strcpy(thg.respath, "./");
		strcpy(scriptpath, "main.um");
		fclose(f);
	} else if ((f = fopen("tophat.dat/main.um", "r"))) {
		strcpy(thg.respath, "tophat.dat/");
		strcpy(scriptpath, "tophat.dat/main.um");
		fclose(f);
	} else {
		th_error("Could not find game.um or main.um. Make sure you are in a proper directory.");

		return 1;
	}
	umkaOK = umkaInit(thg.umka, scriptpath, NULL, 1024 * 1024, 1024 * 1024, NULL, 0, NULL);

	th_audio_init();

	if (!umkaOK) {
		printf("Could not initialize umka.\n");
		return 1;
	}

	_th_umka_bind(thg.umka);
	umkaOK = umkaCompile(thg.umka);

	if (!umkaOK) {
		UmkaError error;
		umkaGetError(thg.umka, &error);
		th_error("%s (%d, %d): %s\n", error.fileName, error.line, error.pos, error.msg);
		return 1;
	}

	destroyfunc = umkaGetFunc(thg.umka, NULL, "windowdestroy");
	thg.scaling = 1;

	umkaOK = umkaRun(thg.umka);
	if (!umkaOK) {
		UmkaError error;
		umkaGetError(thg.umka, &error);
		th_error("%s (%d): %s\n", error.fileName, error.line, error.msg);
		fprintf(stderr, "\tStack trace:\n");

		for (int depth = 0; depth < 10; depth++) {
			char fnName[UMKA_MSG_LEN + 1];
			int fnOffset;

			if (!umkaGetCallStack(thg.umka, depth, &fnOffset, fnName, UMKA_MSG_LEN + 1)) {
				break;
				fprintf(stderr, "\t\t...\n");
			}

#ifndef _WIN32
			fprintf(stderr, "\033[34m");
#endif
			fprintf(stderr, "\t\t%08d: ", fnOffset);
#ifndef _WIN32
			fprintf(stderr, "\033[0m");
#endif
			fprintf(stderr, "%s\n", fnName);
		}
	}

	die();

	return 0;
}
