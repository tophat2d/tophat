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
	strcpy(thg.respath, "");

#ifndef EMSCRIPTEN
	int argOffset = 1;
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
		} else if (strcmp(argv[1], "dir") == 0) {
			if (argc != 3) {
				printf("dir takes 1 argument.\n");
				return 1;
			}

			strcpy(thg.respath, argv[2]);
			argOffset += 2;
		} else if (strcmp(argv[1], "help") == 0) {
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
	}

	FILE *f;
	char scriptpath[4096];
	strcpy(scriptpath, thg.respath);
	strcat(scriptpath, "main.um");
	if ((f = fopen(scriptpath, "r"))) {
		fclose(f);
	} else {
		th_error("Could not find main.um. Make sure you are in a proper directory, or specify it using the dir flag.");

		return 1;
	}

	umkaOK = umkaInit(thg.umka, scriptpath, NULL,
		1024 * 1024, 1024 * 1024, NULL,
		argc - argOffset, argv + argOffset);
#else
	umkaOK = umkaInit(thg.umka, "main.um",
"import (\n"
	"\"th.um\"\n"
	"\"std.um\"\n"
	"\"rect.um\"\n"
	"\"input.um\"\n"
	"\"image.um\"\n"
	"\"canvas.um\"\n"
	"\"window.um\"\n"
")\n"
"\n"
"fn main() {\n"
	"window.setup(\"my title\", 400, 400)\n"
	"cam := rect.mk(0, 0, 192, 168)\n"
"\n"
	"for window.cycle(cam) {\n"
		"canvas.drawRect(th.red, rect.mk(10, 10, 20, 5))\n"
		"canvas.drawRect(0x0022ff88, rect.mk(10, 10, 10, 10))\n"
		"canvas.drawLine(th.red, th.Vf2{10, 10}, th.Vf2{20, 20}, 1)\n"

		"if th.time > 10000 { return }\n"
	"}\n"
"}\n",
		1024 * 1024, 1024 * 1024, NULL,
		argc, argv);
#endif

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
