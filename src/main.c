#define __USE_MINGW_ANSI_STDIO 1

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bindings.h"
#include "tophat.h"
#include <umka_api.h>

#include <stdlib.h>
#define UMPROF_IMPL
#include <umprof.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <sokol_app.h>

#ifndef TH_VERSION
#define TH_VERSION ""
#define TH_GITVER ""
#endif

th_global *thg;

extern char *th_em_modulenames[];
extern char *th_em_moduledocs[];
extern char *th_em_modulesrc[];
extern char *th_em_misc[];
extern int th_em_modulenames_count;

static void
warning(UmkaError *error)
{
	fprintf(stderr, "Warning %s (%d, %d): %s\n", error->fileName, error->line, error->pos,
	    error->msg);
}

int
th_init(const char *scriptpath, const char *script_src)
{
	{
		thg->res_dir = strdup(scriptpath);
		char *fname = strrchr(thg->res_dir, '/');
		ssize_t len = fname == NULL ? strlen(thg->res_dir) : strlen(fname) - 1;
		thg->res_dir[strlen(thg->res_dir) - len] = '\0';
	}

	{
		char *abs = realpath(thg->res_dir[0] == '\0' ? "./" : thg->res_dir, NULL);
		char *dirname = strrchr(abs, '/');
		ssize_t dirlen = dirname == NULL ? 0 : strlen(dirname);

		bool data_path_allocd = 0;
#ifdef _WIN32
		char *data_path = getenv("APPDATA");
#else
		char *data_path = getenv("XDG_DATA_HOME");
		if (data_path == NULL) {
			data_path = getenv("HOME");
			if (data_path == NULL) {
				th_error("Could not find home directory");
			}
			data_path_allocd = 1;
			char *tmp = calloc(1, strlen(data_path) + strlen("/.local/share") + 1);
			sprintf(tmp, "%s/.local/share", data_path);
			data_path = tmp;
		}
#endif

		thg->data_dir = calloc(1, strlen(data_path) + strlen("/tophat") + dirlen + 1);
		strcpy(thg->data_dir, data_path);

		strcat(thg->data_dir, "/tophat");
		mkdir(thg->data_dir, 0777);

		strcat(thg->data_dir, dirname == NULL ? "" : dirname);
		mkdir(thg->data_dir, 0777);

		if (data_path_allocd)
			free(data_path);
		free(abs);
	}

	char *mainmod_fmt = "import (mainmod = \"%s\"; \"window.um\")\n"
			    "fn main() {}\n"
			    "fn __th_init*() {\n"
			    "  mainmod.init()\n"
			    "}\n";
	char mainmod[sizeof(mainmod_fmt) + BUFSIZ];
	snprintf(mainmod, sizeof(mainmod), mainmod_fmt, scriptpath);

	int umkaOK;

	thg->umka = umkaAlloc();
	umkaOK = umkaInit(thg->umka, "tophat_main.um", mainmod, 1024 * 1024, NULL,
	    thg->argc - thg->argOffset, thg->argv + thg->argOffset, true, true,
	    thg->silent ? NULL : warning);
	if (thg->prof)
		umprofInit(thg->umka);

	if (!umkaOK) {
		printf("Could not initialize umka.\n");
		return 1;
	}

	_th_umka_bind(thg->umka);

	if (script_src != NULL)
		umkaAddModule(thg->umka, scriptpath, script_src);

	umkaOK = umkaCompile(thg->umka);

	if (!umkaOK) {
		UmkaError error;
		umkaGetError(thg->umka, &error);
		th_error("%s (%d, %d): %s", error.fileName, error.line, error.pos, error.msg);
		return 1;
	}

	if (thg->print_asm) {
		char *path = calloc(1, strlen(scriptpath) + 4 + 1);
		sprintf(path, "%s.asm", scriptpath);

		char *buf = umkaAsm(thg->umka);

		FILE *f = fopen(path, "w");
		fprintf(f, "%s\n", buf);
		fclose(f);

		free(path);
		free(buf);
		return 0;
	}

	// Just check umka file
	if (thg->check) {
		return 0;
	}

	thg->umth_frame_callback = umkaGetFunc(thg->umka, "window.um", "umth_frame_callback");
	thg->umth_destroy_callback = umkaGetFunc(thg->umka, "window.um", "umth_destroy_callback");

	if (thg->umth_frame_callback == -1) {
		th_error("Internal error: umth_frame_callback == -1");
	}
	if (thg->umth_destroy_callback == -1) {
		th_error("Internal error: umth_destroy_callback == -1");
	}

	thg->scaling = 1;

	return 0;
}

void
th_deinit()
{
	if (thg->prof) {
		if (thg->profJson) {
			FILE *f = fopen("prof.json", "w");
			umprofPrintEventsJSON(f);
			fclose(f);
		} else {
			UmprofInfo arr[BUFSIZ] = {0};
			size_t len = umprofGetInfo(arr, BUFSIZ);
			umprofPrintInfo(stdout, arr, len);
		}
	}

	UmkaStackSlot s;
	umkaCall(thg->umka, thg->umth_destroy_callback, 0, &s, &s);

	umkaRun(thg->umka);
	umkaFree(thg->umka);

	th_audio_deinit();
	th_font_deinit();
	th_image_deinit();

	free(thg->res_dir);

	free(thg);
	thg = NULL;
}

#ifdef __EMSCRIPTEN__

int
run_playground(const char *src)
{
	UmkaStackSlot s;
	umkaCall(thg->umka, thg->umth_destroy_callback, 0, &s, &s);
	umkaFree(thg->umka);

	if (th_init("playground_main.um", src)) {
		return 1;
	}

	if (!umkaCall(
		thg->umka, umkaGetFunc(thg->umka, "tophat_main.um", "__th_init"), 0, &s, &s)) {
		th_print_umka_error_and_quit();
	}

	fprintf(stderr, "inited\n");
}

#endif

static int
th_main(int argc, char *argv[])
{
	thg = malloc(sizeof(th_global));
	*thg = (th_global){0};

	thg->argc = argc;
	thg->argv = argv;

	const char *scriptpath = "main.um";

	thg->argOffset = 1;
	while ((argc - thg->argOffset) > 0) {
		if (strcmp(argv[thg->argOffset], "-dpiaware") == 0) {
			thg->dpi_aware = true;
			thg->argOffset += 1;
		} else if (strcmp(argv[thg->argOffset], "-check") == 0) {
			thg->check = true;
			thg->argOffset += 1;
		} else if (strcmp(argv[thg->argOffset], "-asm") == 0) {
			thg->print_asm = true;
			thg->argOffset += 1;
		} else if (strcmp(argv[thg->argOffset], "-silent") == 0) {
			thg->silent = true;
			thg->argOffset += 1;
		} else if (strcmp(argv[thg->argOffset], "-modsrc") == 0) {
			if ((argc - thg->argOffset) < 2) {
				printf("modsrc takes one argument\n");
				exit(1);
			}

			for (int i = 0; i < th_em_modulenames_count; i++) {
				if (strcmp(argv[thg->argOffset + 1], th_em_modulenames[i]) == 0) {
					th_info("%s\n", th_em_modulesrc[i]);
					exit(0);
				}
			}

			th_error("No module named %s\n", argv[thg->argOffset + 1]);
			thg->argOffset += 2;
		} else if (strcmp(argv[thg->argOffset], "-doc") == 0) {
			if ((argc - thg->argOffset) < 2) {
				printf("doc takes one argument\n");
				exit(1);
			}

			for (int i = 0; i < th_em_modulenames_count; i++) {
				if (strcmp(argv[thg->argOffset + 1], th_em_modulenames[i]) == 0) {
					th_info("%s\n", th_em_moduledocs[i]);
					exit(0);
				}
			}

			th_error("No module named %s\n", argv[thg->argOffset + 1]);
			thg->argOffset += 2;
		} else if (strcmp(argv[thg->argOffset], "-license") == 0) {
			th_info("%s\n", th_em_misc[0]);
			exit(0);
		} else if (strcmp(argv[thg->argOffset], "-main") == 0) {
			if ((argc - thg->argOffset) < 2) {
				th_error("main takes one argument - path to the main module\n");
				exit(1);
			}

			scriptpath = argv[thg->argOffset + 1];
			thg->argOffset += 2;
		} else if (strcmp(argv[thg->argOffset], "-version") == 0) {
			th_info(TH_VERSION "-" TH_GITVER ", built on " __DATE__ " " __TIME__
					   "\n%s\n",
			    umkaGetVersion());
			exit(0);
		} else if (strcmp(argv[thg->argOffset], "-help") == 0) {
			th_info("tophat - a minimalist game engine for making games in umka.\n"
				"Just launching tophat without flags will run main.um\n"
				"Available modes:\n"
				"  -check - only check the program for errors\n"
				"  -dir - specify the resource directory (. by default)\n"
				"  -help - show this help\n"
				"  -license - print the license\n"
				"  -main - specify the main file (./main.um by default)\n"
				"  -modsrc <module name> - print source of a builtin module\n"
				"  -prof - use the profiler\n"
				"  -profjson - output profiler stuff as json\n"
				"  -silent - omit warnings\n"
				"  -version - print the version\n"
				"  -doc - print API docs for an umka module\n"
				"Visit tophat2d.dev for more info.\n");
			exit(0);
		} else if (strcmp(argv[thg->argOffset], "-prof") == 0) {
			thg->prof = true;
			thg->argOffset += 1;
		} else if (strcmp(argv[thg->argOffset], "-profjson") == 0) {
			thg->profJson = true;
			thg->argOffset += 1;
		} else {
			break; // NOTE(skejeton): This is for arguments in the game itself
		}
	}

	FILE *f;
	if ((f = fopen(scriptpath, "r"))) {
		fclose(f);
	} else {
		th_error("Could not find %s. Make sure you are in a proper directory, or specify "
			 "it using the dir flag.",
		    scriptpath);

		return 1;
	}

	return th_init(scriptpath, NULL);
}

sapp_desc
sokol_main(int argc, char *argv[])
{
	if (th_main(argc, argv))
		exit(1);
	return th_window_sapp_desc();
}
