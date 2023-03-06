#include <stdio.h>
#include <string.h>

#include <umka_api.h>
#include "bindings.h"
#include "tophat.h"

#include <stdlib.h>
#define UMPROF_IMPL
#include <umprof.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <sokol_app.h>

th_global *thg;

extern char *th_em_modulenames[];
extern char *th_em_modulesrc[];
extern char *th_em_misc[];
extern int th_em_modulenames_count;

static void warning(UmkaError *error) {
	fprintf(stderr, "Warning %s (%d, %d): %s\n", error->fileName, error->line, error->pos, error->msg);
}

static int th_main(int argc, char *argv[]) {
	thg = malloc(sizeof(th_global));
	*thg = (th_global){0};

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

	if (!umkaOK) {
		printf("Could not initialize umka.\n");
		return 1;
	}

	_th_umka_bind(thg->umka);
	umkaOK = umkaCompile(thg->umka);

	if (!umkaOK) {
		UmkaError error;
		umkaGetError(thg->umka, &error);
		int val = 42;
		th_error("bazinga! %d", val);
		th_error("%s (%d, %d): %s", error.fileName, error.line, error.pos, error.msg);
		return 1;
	}

	// Just check umka file
	if (check) {
		return 0;
	}

	thg->scaling = 1;

	return 0;
}


sapp_desc sokol_main(int argc, char *argv[]) {
	printf("PROBE1\n");
	if (th_main(argc, argv))
		exit(1);
	printf("PROBE2\n");
	return th_window_sapp_desc();
}