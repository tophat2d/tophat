#include "tophat.h"
#include <stdlib.h>
#include <stdio.h>

extern th_global *thg;

static bool window_active = false;

#ifdef linux
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <GL/glx.h>

static Atom wm_delete_message;
static Display *th_dpy = NULL;
static Window th_win;
static Window th_root_win;
static XWindowAttributes th_win_attribs;
static GLXContext th_ctx;

static XkbDescPtr desc;
static XIC xic;

static void th_window_deinit() {
	XDestroyWindow(th_dpy, th_win);
	XCloseDisplay(th_dpy);
}

void th_window_setup(char *name, int w, int h) {
	if (window_active) {
		th_error("Window already created.");
		return;
	}

	th_dpy = XOpenDisplay(NULL);
	if (!th_dpy) {
		th_error("Could not open X display.");
		return;
	}
	wm_delete_message = XInternAtom(th_dpy, "WM_DELETE_WINDOW", False);

	int screen = DefaultScreen(th_dpy);
	th_root_win = DefaultRootWindow(th_dpy);

	int gl_attribs[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		None
	};

	XVisualInfo *vi = glXChooseVisual(th_dpy, screen, gl_attribs);
	if (!vi) {
		th_error("No visual found.");
		return;
	}

	XSetWindowAttributes attribs;
	attribs.background_pixel = 0;
	attribs.colormap = XCreateColormap(th_dpy, th_root_win, vi->visual, AllocNone);
	th_win = XCreateWindow(th_dpy, th_root_win, 0, 0, w, h, 0, vi->depth, InputOutput,
		vi->visual, CWBorderPixel | CWColormap, &attribs);
	
	XSetWMProtocols(th_dpy, th_win, &wm_delete_message, 1);

	XMapWindow(th_dpy, th_win);
	XStoreName(th_dpy, th_win, name);
	th_ctx = glXCreateContext(th_dpy, vi, NULL, GL_TRUE);
	if (!th_ctx) {
		th_error("Could not create a context.");
		return;
	}
	glXMakeCurrent(th_dpy, th_win, th_ctx);

	XGetWindowAttributes(th_dpy, th_win, &th_win_attribs);
	XSelectInput(th_dpy, th_win, KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | ExposureMask | PointerMotionMask);
	atexit(th_window_deinit);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	XkbComponentNamesRec names = {};
	names.symbols = "us";
	desc = XkbGetKeyboardByName(th_dpy, XkbUseCoreKbd, &names, XkbAllComponentsMask, XkbAllComponentsMask, false);

	XIM xim = XOpenIM(th_dpy, 0, 0, 0);
	if (!xim) {
		th_error("Could not open XIM.");
		return;
	}

	xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, NULL);
	if (!xic) {
		th_error("Could not create XIC");
		return;
	}

	window_active = true;
}

void th_window_get_dimensions(int *w, int *h) {
	*w = th_win_attribs.width;
	*h = th_win_attribs.height;
}

int th_window_handle() {
	if (!window_active) {
		th_error("No window was created.");
		return 0;
	}

	th_input_key(4, 0);
	th_input_key(5, 0);

	XEvent ev;
	while (XPending(th_dpy)) {
		XNextEvent(th_dpy, &ev);

		int keyDir = 1;
		switch (ev.type) {
		case Expose:
			XGetWindowAttributes(th_dpy, th_win, &th_win_attribs);
			glViewport(0, 0, th_win_attribs.width, th_win_attribs.height);
			break;
		case KeyPress:; // FALLTHROUGH
			KeySym _;
			Status status;
			thg->input_string_len += Xutf8LookupString(xic, &ev.xkey,
				thg->input_string + thg->input_string_len,
				INPUT_STRING_SIZE - thg->input_string_len,
				&_, &status);

		case KeyRelease:;
			// modifiers are not used for this kind of input
			unsigned mods = 0;
			
			// translate to a qwerty key
			KeySym sym;
			XkbTranslateKeyCode(desc, ev.xkey.keycode, mods, &mods, &sym);
			if (!sym)
				sym = XLookupKeysym(&ev.xkey, 0);

			th_input_key(sym, ev.type == KeyPress);
			break;
		case ButtonRelease:
			keyDir = 0;
			if (ev.xbutton.button > 3) break;
		case ButtonPress:
			th_input_key(ev.xbutton.button, keyDir);
			break;
		case MotionNotify:
			thg->mouse = (th_vf2){ .x = ev.xmotion.x, .y = ev.xmotion.y };
			break;
		case ClientMessage:
			if (ev.xclient.data.l[0] == wm_delete_message) {
				return 0;
			}
			break;
		}
	}

	return 1;
}

void th_window_clear_frame() {
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void th_window_swap_buffers() {
	th_canvas_flush();
	th_image_flush();
	th_input_cycle();

	glXSwapBuffers(th_dpy, th_win);

	thg->input_string_len = 0;
}
#elif _WIN32
#include <windows.h>
#include <wingdi.h>
#include <windowsx.h>

static HWND th_win;
static HDC th_hdc;
static RECT th_win_rect;
static int should_close = 0;
static th_vf2 win_size;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_DESTROY)
		should_close = 1;
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

static HKL hkl;

void th_window_setup(char *name, int w, int h) {
	if (window_active) {
		th_error("Window already created.");
		return;
	}

	const char CLASS_NAME[] = "tophat";

	HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASS wc = {
		.lpfnWndProc = WndProc,
		.hInstance = hInstance,
		.hCursor = LoadCursor(NULL, IDC_ARROW),
		.lpszClassName = CLASS_NAME
	};

	RegisterClass(&wc);

	th_win = CreateWindow(
		CLASS_NAME,
		name,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		w, h,
		NULL, NULL,
		hInstance, NULL);
	GetWindowRect(th_win, &th_win_rect);
	win_size.x = th_win_rect.right;
	win_size.y = th_win_rect.bottom;

	if (th_win == NULL) {
		th_error("could not create a window");
		exit(1);
	}

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		8, 0, 8, 8, 8, 16, 
		8,
		24,
		32,
		8, 8, 8, 8,
		16,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	th_hdc = GetDC(th_win);
	GLuint pf = ChoosePixelFormat(th_hdc, &pfd);
	if (!SetPixelFormat(th_hdc, pf, &pfd)) {
		th_error("couldn't choose pixel format");
		exit(1);
	}
	HGLRC ctx = wglCreateContext(th_hdc);
	if (!ctx) {
		th_error("couldn't create an opengl context");
		exit(1);
	}
	wglMakeCurrent(th_hdc, ctx);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ShowWindow(th_win, 1);

	hkl = LoadKeyboardLayout("00000409", 0);

	window_active = true;
}

void th_window_get_dimensions(int *w, int *h) {
	*w = th_win_rect.right - th_win_rect.left;
	*h = th_win_rect.bottom - th_win_rect.top;
}

int th_window_handle() {
	if (!window_active) {
		th_error("No window was created.");
		return 0;
	}

	MSG msg;
	th_input_key(4, 0);
	th_input_key(5, 0);

	while (PeekMessage(&msg, NULL, 0, 0, 1)) {
		TranslateMessage(&msg);

		// i found a list of these on the wine website, but not on MSDN -_-
		switch (msg.message) {
		case WM_KEYDOWN:; // FALLTHROUGH
			char key_state[256];
			GetKeyboardState(key_state);

			wchar_t buf[256];
			int v = ToUnicode(msg.wParam, MapVirtualKey(msg.wParam, MAPVK_VK_TO_VSC), key_state, buf, 256, 0);
			if (v == 1) // only supports one character for now
				thg->input_string_len += th_utf8_encode(thg->input_string + thg->input_string_len, buf[0]);

		case WM_KEYUP:;
			uint32_t key = tolower(MapVirtualKeyEx(msg.wParam, MAPVK_VK_TO_CHAR, hkl));
			if (key == 0)
				key = MapVirtualKeyEx(MapVirtualKey(msg.wParam, MAPVK_VK_TO_VSC), MAPVK_VSC_TO_VK, hkl) + 0x7f;

			th_input_key(key, msg.message == WM_KEYDOWN);
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			th_input_key(1, msg.message == WM_LBUTTONDOWN);
			break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			th_input_key(2, msg.message == WM_MBUTTONDOWN);
			break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			th_input_key(3, msg.message == WM_RBUTTONDOWN);
			break;
		case WM_MOUSEMOVE:
			thg->mouse = (th_vf2){ .x = GET_X_LPARAM(msg.lParam), .y = GET_Y_LPARAM(msg.lParam) };
			break;
		case WM_MOUSEWHEEL:
			th_input_key(GET_WHEEL_DELTA_WPARAM(msg.wParam) < 0 ? 5 : 4, 1);
			break;
		default:
			DispatchMessage(&msg);
			break;
		}
	}
	GetClientRect(th_win, &th_win_rect);
	glViewport(0, 0, th_win_rect.right, th_win_rect.bottom);

	return !should_close;
}

void th_window_swap_buffers() {
	th_canvas_flush();
	th_image_flush();
	th_input_cycle();
	SwapBuffers(th_hdc);

	thg->input_string_len = 0;
}

void th_window_clear_frame() {
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

#else
#error tophat cant create a window on this platform yet
#endif

void th_window_begin_scissor(int x, int y, size_t w, size_t h) {
	// NOTE(skejeton): The flush is necessary because all the previous render calls
	//				   shouldn't be cut out using the rectangle.
	th_canvas_flush();
	th_image_flush();

	x *= thg->scaling;
	y *= thg->scaling;
	w *= thg->scaling;
	h *= thg->scaling;
	glEnable(GL_SCISSOR_TEST);
	int dimX, dimY;
	th_window_get_dimensions(&dimX, &dimY);
	glScissor(x+thg->offset.x, (dimY-(int)h)-y-thg->offset.y, w, h);
}

void th_window_end_scissor() {
	th_canvas_flush();
	th_image_flush();
	glDisable(GL_SCISSOR_TEST);
}
