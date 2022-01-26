#include <stdio.h>
#include <string.h>

#include <umka_api.h>
#include "bindings.h"
#include "tophat.h"

th_global thg = {0};
int destroyfunc;
int died = 0;

extern char *th_em_modulenames[];
extern char *th_em_modulesrc[];
extern char *th_em_misc[];
extern int th_em_modulenames_count;

void die() {
	if (died) return;
	
	th_audio_deinit();
	th_font_deinit();
	th_image_deinit();
	if (destroyfunc) {
		umkaCall(thg.umka, destroyfunc, 0, NULL, NULL);
	}

	umkaFree(thg.umka);
	died = 1;
}

int main(int argc, char *argv[]) {
	thg.umka = umkaAlloc();
	int umkaOK;

	if (argc != 1) {
		if (strcmp(argv[1], "modsrc") == 0) {
			if (argc != 3) {
				printf("modsrc takes one argument\n");
				return 1;
			}
			
			for (int i=0; i < th_em_modulenames_count; i++) {
				if (strcmp(argv[2], th_em_modulenames[i]) == 0) {
					printf(th_em_modulesrc[i]);
					return 0;
				}
			}

			printf("No module named %s\n", argv[2]);
		} else if (strcmp(argv[1], "license") == 0) {
			printf("%s\n", th_em_misc[0]);
			return 0;
		} else if (strcmp(argv[1], "version") == 0) {
			printf("%s\n", th_em_misc[1]);
		} else {
			printf(
				"tophat - a minimalist game engine for making games in umka.\n"
				"Just launching tophat without flags will run main.um or tophat.dat/main.um\n"
				"Available modes:\n"
				"  modsrc <module name> - print source of a builtin module\n"
				"  license - print the license\n"
				"  version - print the version\n"
				"Visit mrms.cz/tophat.html for more info.");
			return 0;
		}

		return 0;
	}

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
