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
	strncpy(thg->respath, "", sizeof thg->respath);
	const char *scriptpath = "main.um";
	bool check = false;
	bool silent = false;

	int argOffset = 1;
	while ((argc-argOffset) > 0) {
		if (strcmp(argv[argOffset], "-check") == 0) {
			check = true;
			argOffset += 1;
		} else if (strcmp(argv[argOffset], "-silent") == 0) {
			silent = true;
			argOffset += 1;
		} else if (strcmp(argv[argOffset], "-modsrc") == 0) {
			if ((argc-argOffset) < 2) {
				printf("modsrc takes one argument\n");
				exit(1);
			}
			
			for (int i=0; i < th_em_modulenames_count; i++) {
				if (strcmp(argv[argOffset+1], th_em_modulenames[i]) == 0) {
					th_info("%s\n", th_em_modulesrc[i]);
					exit(0);
				}
			}

			th_error("No module named %s\n", argv[argOffset+1]);
			argOffset += 2;
		} else if (strcmp(argv[argOffset], "-license") == 0) {
			th_info("%s\n", th_em_misc[0]);
			exit(0);
		} else if (strcmp(argv[argOffset], "-main") == 0) {
			if ((argc-argOffset) < 2) {
				th_error("main takes one argument - path to the main module\n");
				exit(1);
			}

			scriptpath = argv[argOffset+1];
			argOffset += 2;
		} else if (strcmp(argv[argOffset], "-version") == 0) {
			th_info(TH_VERSION "-" TH_GITVER ", built on " __DATE__ " " __TIME__ "\n%s\n", umkaGetVersion());
			exit(0);
		} else if (strcmp(argv[argOffset], "-dir") == 0) {
			if ((argc-argOffset) < 2) {
				th_error("dir takes 1 argument.\n");
				exit(1);
			}

			strncpy(thg->respath, argv[argOffset+1], sizeof thg->respath);
			argOffset += 2;
		} else if (strcmp(argv[argOffset], "-help") == 0) {
			th_info(
				"tophat - a minimalist game engine for making games in umka.\n"
				"Just launching tophat without flags will run main.um\n"
				"Available modes:\n"
				"  -check - only check the program for errors\n"
				"  -dir - specify the resource directory (. by default)\n"
				"  -help - show this help\n"
				"  -license - print the license\n"
				"  -main - specify the main file (dir/main.um by default)\n"
				"  -modsrc <module name> - print source of a builtin module\n"
				"  -prof - use the profiler\n"
				"  -profjson - output profiler stuff as json\n"
				"  -silent - omit warnings\n"
				"  -version - print the version\n"
				"Visit th.mrms.cz for more info.\n");
			exit(0);
		} else if (strcmp(argv[argOffset], "-prof") == 0) {
			thg->prof = true;
			argOffset += 1;
		} else if (strcmp(argv[argOffset], "-profjson") == 0) {
			thg->profJson = true;
			argOffset += 1;
		} else {
			break; // NOTE(skejeton): This is for arguments in the game itself
		}
	}

	FILE *f;
	if ((f = fopen(scriptpath, "r"))) {
		fclose(f);
	} else {
		th_error("Could not find %s. Make sure you are in a proper directory, or specify it using the dir flag.", scriptpath);

		return 1;
	}
	
	char *mainmod_fmt =
"import (\"window.um\"; mainmod = \"%s\")\n"
"fn main() {}\n"
"fn __th_init*() {\n"
"  mainmod.init()\n"
"}\n";
	char mainmod[sizeof(mainmod_fmt) + BUFSIZ];
	snprintf(
		mainmod,
		sizeof(mainmod),
		mainmod_fmt,
		scriptpath);

	umkaOK = umkaInit(thg->umka, "tophat_main.um", mainmod, 1024 * 1024, NULL,
		argc - argOffset, argv + argOffset, true, true, silent ? NULL : warning);
	if (thg->prof) umprofInit(thg->umka);

	if (!umkaOK) {
		printf("Could not initialize umka.\n");
		return 1;
	}

	_th_umka_bind(thg->umka);
	umkaOK = umkaCompile(thg->umka);

	if (!umkaOK) {
		UmkaError error;
		umkaGetError(thg->umka, &error);
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
	if (th_main(argc, argv))
		exit(1);
	return th_window_sapp_desc();
}
