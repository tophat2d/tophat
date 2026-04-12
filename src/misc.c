#include "tophat.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define fileno _fileno
#else
#include <unistd.h>
#endif

#define MSG_LEN 1024

extern th_global *thg;

void
th_calculate_scaling(float camw, float camh)
{
	int w, h;
	th_window_get_dimensions(&w, &h);

	if (w * h * camw * camh == 0)
		return;

	if ((float)w / camw < (float)h / camh) {
		thg->scaling = ((float)w / camw);
	} else {
		thg->scaling = ((float)h / camh);
	}

	thg->offset.x = (w - camw * thg->scaling) / 2;
	thg->offset.y = (h - camh * thg->scaling) / 2;
}

float
th_get_scaling(int w, int h, int camw, int camh)
{
	if ((float)w / camw < (float)h / camh)
		return (float)w / camw;

	return (float)h / camh;
}

void
th_error(char *text, ...)
{
#ifdef __EMSCRIPTEN__
	fprintf(stderr, "error: ");
#else
	if (isatty(fileno(stderr))) {
		fprintf(stderr, "\x1b[1m\x1b[31merror: \x1b[0m");
	} else {
		fprintf(stderr, "error: ");
	}
#endif

	va_list args;
	va_start(args, text);
#if defined(_WIN32) && !defined(_CONSOLE)
#include <winuser.h>
	if (!thg->check) {
		char buf[4096];
		vsnprintf(buf, 4096, text, args);
		MessageBox(NULL, buf, "tophat error", 0x10);
	}
#endif
	vfprintf(stderr, text, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void
th_info(char *text, ...)
{
	va_list args;
	va_start(args, text);
#if defined(_WIN32) && !defined(_CONSOLE)
#include <winuser.h>
	if (!thg->check) {
		char buf[4096];
		vsnprintf(buf, 4096, text, args);
		MessageBox(NULL, buf, "information", 0x40);
	}
#endif
	vfprintf(stderr, text, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void
th_rotate_point(th_vf2 *p, th_vf2 o, fu rot)
{
	const float angle = (rot * PI) / 180;

	const fu cosa = cos(angle);
	const fu sina = sin(angle);

	p->x -= o.x;
	p->y -= o.y;

	const fu x = p->x * cosa - p->y * sina;
	const fu y = p->x * sina + p->y * cosa;

	p->x = o.x + x;
	p->y = o.y + y;
}

void
th_vector_normalize(float *x, float *y)
{
	float fx = fabs(*x);
	float fy = fabs(*y);

	if (fx > fy) {
		*x /= fx;
		*y /= fx;
	} else {
		*x /= fy;
		*y /= fy;
	}
}

void
th_regularize_path(const char *path, const char *cur_folder, char *regularized_path, int size)
{
	size_t o = 0;
	bool is_slash = true;

	// for now simply convert all backslashes to forward slashes and ignore `./`
	for (size_t i = 0; cur_folder[i] && size; i++) {
		if ((i == 0 || cur_folder[i - 1] == '/' || cur_folder[i - 1] == '\\') &&
		    cur_folder[i] == '.' &&
		    (cur_folder[i + 1] == '/' || cur_folder[i + 1] == '\\')) {
			i++;
			is_slash = true;
			continue;
		}

		size--;
		if (cur_folder[i] == '\\') {
			regularized_path[o] = '/';
		} else {
			regularized_path[o] = cur_folder[i];
		}

		is_slash = regularized_path[o] == '/';

		o++;
	}

	if (!is_slash && size) {
		regularized_path[o] = '/';
		o++;
		size--;
	}

	for (size_t i = 0; path[i] && size; i++) {
		if ((i == 0 || path[i - 1] == '/' || path[i - 1] == '\\') && path[i] == '.' &&
		    (path[i + 1] == '/' || path[i + 1] == '\\')) {
			i++;
			continue;
		}

		size--;
		if (path[i] == '\\') {
			regularized_path[o] = '/';
		} else {
			regularized_path[o] = path[i];
		}
		o++;
	}

	if (size) {
		regularized_path[o] = '\0';
		return;
	}

	th_error("Path too long: %s%s", cur_folder, path);
	exit(-1);
}

void
th_print_umka_error_and_quit(int code)
{
	UmkaError *error = umkaGetError(thg->umka);

	if (error->code != 0 && error->msg[0]) {
		th_error("%s (%d): %s\n", error->fileName, error->line, error->msg);

		fprintf(stderr, "\tStack trace:\n");

		for (int depth = 0; depth < 10; depth++) {
			char fnName[MSG_LEN + 1];
			char file[MSG_LEN + 1];
			int line, offset;

			if (!umkaGetCallStack(
				thg->umka, depth, MSG_LEN + 1, &offset, file, fnName, &line)) {
				fprintf(stderr, "\t\t...\n");
				break;
			}
			if (isatty(fileno(stderr))) {
				fprintf(stderr, "\033[34m");
			}
			fprintf(stderr, "\t\t%s:%06d: ", file, line);
			if (isatty(fileno(stderr))) {
				fprintf(stderr, "\033[0m");
			}
			fprintf(stderr, "%s\n", fnName);
		}
	}

#ifndef __EMSCRIPTEN__
	th_deinit();
#endif
	exit(code);
}
