#include <stdio.h>
#include <string.h>

#include <umka_api.h>
#include "bindings.h"
#include "tophat.h"

#include <stdlib.h>
#define UMPROF_IMPL
#include <umprof.h>

th_global *thg;
int destroyfunc;
int dead = 0;

extern char *th_em_modulenames[];
extern char *th_em_modulesrc[];
extern char *th_em_misc[];
extern int th_em_modulenames_count;

static void warning(UmkaError *error) {
	fprintf(stderr, "Warning %s (%d, %d): %s\n", error->fileName, error->line, error->pos, error->msg);
}

static void die() {
	if (dead) return;
	
	th_audio_deinit();
	th_font_deinit();
	th_image_deinit();
	th_shader_deinit();
	if (destroyfunc) {
		umkaCall(thg->umka, destroyfunc, 0, NULL, NULL);
	}

	umkaFree(thg->umka);
	dead = 1;
}

int main(int argc, char *argv[]) {
	th_global thg_ = {0};
	thg = &thg_;

	thg->umka = umkaAlloc();
	int umkaOK;
	strcpy(thg->respath, "");
	const char *scriptpath = "main.um";
	bool check = false;
	bool prof = false;
	bool silent = false;

	int argOffset = 1;
	while ((argc-argOffset) > 0) {
		if (strcmp(argv[argOffset], "check") == 0) {
			check = true;
			argOffset += 1;
		} else if (strcmp(argv[argOffset], "silent") == 0) {
			silent = true;
			argOffset += 1;
		} else if (strcmp(argv[argOffset], "modsrc") == 0) {
			if ((argc-argOffset) < 2) {
				printf("modsrc takes one argument\n");
				return 1;
			}
			
			for (int i=0; i < th_em_modulenames_count; i++) {
				if (strcmp(argv[argOffset+1], th_em_modulenames[i]) == 0) {
					printf("%s\n", th_em_modulesrc[i]);
					return 0;
				}
			}

			printf("No module named %s\n", argv[argOffset+1]);
			argOffset += 2;
		} else if (strcmp(argv[argOffset], "license") == 0) {
			printf("%s\n", th_em_misc[0]);
			return 0;
		} else if (strcmp(argv[argOffset], "main") == 0) {
			if ((argc-argOffset) < 2) {
				printf("main takes one argument - path to the main module\n");
				return 1;
			}

			scriptpath = argv[argOffset+1];
			argOffset += 2;
		} else if (strcmp(argv[argOffset], "version") == 0) {
			printf("%s\n", th_em_misc[1]);
			return 0;
		} else if (strcmp(argv[argOffset], "dir") == 0) {
			if ((argc-argOffset) < 2) {
				printf("dir takes 1 argument.\n");
				return 1;
			}

			strcpy(thg->respath, argv[argOffset+1]);
			argOffset += 2;
		} else if (strcmp(argv[argOffset], "help") == 0) {
			printf(
				"tophat - a minimalist game engine for making games in umka.\n"
				"Just launching tophat without flags will run main.um\n"
				"Available modes:\n"
				"  check - only check the program for errors\n"
				"  dir - specify the resource directory (. by default)\n"
				"  help - show this help\n"
				"  license - print the license\n"
				"  main - specify the main file (dir/main.um by default)\n"
				"  modsrc <module name> - print source of a builtin module\n"
				"  prof - use the profiler\n"
				"  silent - omit warnings\n"
				"  version - print the version\n"
				"Visit th.mrms.cz for more info.\n");
			return 0;
		} else if (strcmp(argv[argOffset], "prof") == 0) {
			prof = true;
			argOffset += 1;
		} else {
			break; // NOTE(skejeton): This is for arguments in the game itself
		}
	}

	FILE *f;
	if ((f = fopen(scriptpath, "r"))) {
		fclose(f);
	} else {
		th_error("Could not find main.um. Make sure you are in a proper directory, or specify it using the dir flag.");

		return 1;
	}
	
	umkaOK = umkaInit(thg->umka, scriptpath, NULL, 1024 * 1024, NULL,
		argc - argOffset, argv + argOffset, true, true, silent ? NULL : warning);
	if (prof) umprofInit(thg->umka);

	th_audio_init();

	if (!umkaOK) {
		printf("Could not initialize umka.\n");
		return 1;
	}

	_th_umka_bind(thg->umka);
	umkaOK = umkaCompile(thg->umka);

	if (!umkaOK) {
		UmkaError error;
		umkaGetError(thg->umka, &error);
		th_error("%s (%d, %d): %s\n", error.fileName, error.line, error.pos, error.msg);
		return 1;
	}

	// Just check umka file
	if (check) {
		return 0;
	}

	destroyfunc = umkaGetFunc(thg->umka, NULL, "windowdestroy");
	thg->scaling = 1;

	umkaOK = umkaRun(thg->umka);
	if (!umkaOK) {
		UmkaError error;
		umkaGetError(thg->umka, &error);
		th_error("%s (%d): %s\n", error.fileName, error.line, error.msg);
		fprintf(stderr, "\tStack trace:\n");

		for (int depth = 0; depth < 10; depth++) {
			char fnName[UMKA_MSG_LEN + 1];
			char file[UMKA_MSG_LEN + 1];
			int line, offset;

			if (!umkaGetCallStack(thg->umka, depth, UMKA_MSG_LEN + 1, &offset, file, fnName, &line)) {
				break;
				fprintf(stderr, "\t\t...\n");
			}

#ifndef _WIN32
			fprintf(stderr, "\033[34m");
#endif
			fprintf(stderr, "\t\t%s:%06d: ", file, line);
#ifndef _WIN32
			fprintf(stderr, "\033[0m");
#endif
			fprintf(stderr, "%s\n", fnName);
		}
	} else if (prof) {
		UmprofInfo info[2048] = {0};
		int len = umprofGetInfo(info, 2048);
		umprofPrintInfo(stderr, info, len);
	}

	die();

	return 0;
}
