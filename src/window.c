#include "tophat.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

static Cursor blank_cursor;
static bool cursor_frozen = false;

static XkbDescPtr desc;
static XIC xic;

static bool is_fullscreen = false;

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
	XGetWindowAttributes(th_dpy, th_win, &th_win_attribs);
	glViewport(0, 0, th_win_attribs.width, th_win_attribs.height);
	thg->viewport.w = th_win_attribs.width;
	thg->viewport.h = th_win_attribs.height;

	char bm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	Pixmap pix = XCreateBitmapFromData(th_dpy, th_win, bm, 1, 1);
	XColor black;
	memset(&black, 0, sizeof(XColor));
	black.flags = DoRed | DoGreen | DoBlue;
	blank_cursor = XCreatePixmapCursor(th_dpy, pix, pix, &black, &black, 0, 0);
	XFreePixmap(th_dpy, pix);
}

void th_window_get_dimensions(int *w, int *h) {
	*w = th_win_attribs.width;
	*h = th_win_attribs.height;
}

static void send_event(Atom type, int a, int b, int c, int d, int e) {
	XEvent event = {0};

	event.type = ClientMessage;
	event.xclient.window = th_win;
	event.xclient.format = 32;
	event.xclient.message_type = type;
	event.xclient.data.l[0] = a;
	event.xclient.data.l[1] = b;
	event.xclient.data.l[2] = c;
	event.xclient.data.l[3] = d;
	event.xclient.data.l[4] = e;

	XSendEvent(th_dpy, XDefaultRootWindow(th_dpy),
		False,
		SubstructureNotifyMask | SubstructureRedirectMask,
		&event);
}

static Atom intern_atom(char *n) {
	Atom a = XInternAtom(th_dpy, n, False);
	if (a)
		th_error("Could not find atom %s.", n);
	return a;
}

bool th_window_is_fullscreen() {
	return is_fullscreen;
}

void th_window_set_fullscreen(bool fullscreen) {
	if (is_fullscreen == fullscreen)
		return;

	send_event(intern_atom("_NET_WM_STATE"), fullscreen,
		intern_atom("_NET_WM_STATE_FULLSCREEN"), 0, 1, 0);
	XFlush(th_dpy);

	is_fullscreen = fullscreen;
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

			thg->viewport.w = th_win_attribs.width;
			thg->viewport.h = th_win_attribs.height;
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
			thg->mouse_delta = (th_vf2){
				.x = thg->mouse.x - ev.xmotion.x,
				.y = thg->mouse.y - ev.xmotion.y
			};

			if (cursor_frozen)
				XWarpPointer(th_dpy, None, th_win, None, None, None, None,
					thg->mouse.x, thg->mouse.y
				);
			else
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

void th_window_set_dims(th_vf2 dm) {
	XResizeWindow(th_dpy, th_win, dm.x, dm.y);
}

void th_window_set_icon(th_image *img) {
	Atom net_wm_icon = XInternAtom(th_dpy, "_NEW_WM_ICON", False);
	Atom cardinal = XInternAtom(th_dpy, "CARDINAL", False);

	uint32_t *data = th_image_get_data(img, true);
	XChangeProperty(th_dpy, th_win, net_wm_icon, cardinal, 32, PropModeReplace,
		(const unsigned char *)data, img->dm.x * img->dm.y);
	free(data);
}

void th_window_show_cursor(bool show) {
	if (show)
		XUndefineCursor(th_dpy, th_win);
	else
		XDefineCursor(th_dpy, th_win, blank_cursor);
}

void th_window_freeze_cursor(bool freeze) {
	cursor_frozen = freeze;
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
// NOTE(skejeton): this is the position of the window prior to going fullscreen
static struct { int x, y, w, h; } win_prior;

static HKL hkl;

static void KeyProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	uint32_t key = tolower(MapVirtualKeyEx(wParam, MAPVK_VK_TO_CHAR, hkl));
	if (key == 0 || iscntrl(key))
		key = MapVirtualKeyEx(MapVirtualKey(wParam, MAPVK_VK_TO_VSC), MAPVK_VSC_TO_VK, hkl) + 0x7f;

	th_input_key(key, (msg == WM_SYSKEYDOWN) || (msg == WM_KEYDOWN));
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	// i found a list of these on the wine website, but not on MSDN -_-

	switch (msg) {
		case WM_MENUCHAR:
			// NOTE(skejeton): disable the ding sound when pressing an alt shortcut
			return MNC_CLOSE << 16;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			KeyProc(hwnd, msg, wParam, lParam);

			// NOTE(skejeton): disable the ding sound when pressing an alt shortcut
			return MNC_CLOSE << 16;
			break;
		case WM_KEYDOWN: {
			char key_state[256];
			GetKeyboardState(key_state);

			wchar_t buf[256];
			int v = ToUnicode(wParam, MapVirtualKey(wParam, MAPVK_VK_TO_VSC), key_state, buf, 256, 0);
			if (v == 1) // only supports one character for now
				thg->input_string_len += th_utf8_encode(thg->input_string + thg->input_string_len, buf[0]);
			// FALLTHROUGH
		case WM_KEYUP:
			KeyProc(hwnd, msg, wParam, lParam);
			break;
		} case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
			th_input_key(1, msg == WM_LBUTTONDOWN);
			break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
			th_input_key(2, msg == WM_MBUTTONDOWN);
			break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			th_input_key(3, msg == WM_RBUTTONDOWN);
			break;
		case WM_MOUSEMOVE:
			thg->mouse = (th_vf2){ .x = GET_X_LPARAM(lParam), .y = GET_Y_LPARAM(lParam) };
			break;
		case WM_MOUSEWHEEL:
			th_input_key(GET_WHEEL_DELTA_WPARAM(wParam) < 0 ? 5 : 4, 1);
			break;
		case WM_DESTROY:
			should_close = 1;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void w32_get_client_window_size(int *w, int *h) {
	RECT rect = {0};
	rect.right = *w;
	rect.bottom = *h;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
	*w = rect.right-rect.left;
	*h = rect.bottom-rect.top;
}

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

	// NOTE(skejeton): Set window size to w/h, because in CreateWindow the window size also includes the titlebar/decoraion size,
	//				   that makes the actual content window size incorrect.
	int window_w = w, window_h = h;
	w32_get_client_window_size(&window_w, &window_h);

	th_win = CreateWindow(
		CLASS_NAME,
		name,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		window_w, window_h,
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
	th_gl_init();

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ShowWindow(th_win, 1);

	hkl = LoadKeyboardLayout("00000409", 0);

	window_active = true;
	thg->viewport.w = w;
	thg->viewport.h = h;
}

void th_window_get_dimensions(int *w, int *h) {
	*w = th_win_rect.right - th_win_rect.left;
	*h = th_win_rect.bottom - th_win_rect.top;
}

// NOTE(skejeton):
// whether the window is fullscreen or not can be detected if we check if window has a WS_POPUP style
bool th_window_is_fullscreen() {
	return GetWindowLongPtr(th_win, GWL_STYLE) & WS_POPUP;
}

static void set_window_prior_to_rect(RECT r) {
	win_prior.x = r.left;
	win_prior.y = r.top;
	win_prior.w = r.right-r.left;
	win_prior.h = r.bottom-r.top;
}

void th_window_set_fullscreen(bool fullscreen) {
	if (th_window_is_fullscreen() == fullscreen) {
		// no change required
		return;
	}

	int w = GetSystemMetrics(SM_CXSCREEN), h = GetSystemMetrics(SM_CYSCREEN);

	if (fullscreen) {
		SetWindowLongPtr(th_win, GWL_STYLE, WS_VISIBLE | WS_POPUP);
		RECT r;
		GetWindowRect(th_win, &r);
		set_window_prior_to_rect(r);
		SetWindowPos(th_win, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);
	} else {
		SetWindowLongPtr(th_win, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
		SetWindowPos(th_win, HWND_TOP, win_prior.x, win_prior.y, win_prior.w, win_prior.h, SWP_FRAMECHANGED);
	}
}

int th_window_handle() {
	if (!window_active) {
		th_error("No window was created.");
		return 0;
	}

	// NOTE(skejeton): prevent stale alt
	th_input_key(VK_MENU+0x7F, GetAsyncKeyState(VK_MENU));
	
	MSG msg;
	th_input_key(4, 0);
	th_input_key(5, 0);

	while (PeekMessage(&msg, NULL, 0, 0, 1)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GetClientRect(th_win, &th_win_rect);
	glViewport(0, 0, th_win_rect.right, th_win_rect.bottom);
	thg->viewport.w = th_win_rect.right - th_win_rect.left;
	thg->viewport.h = th_win_rect.bottom - th_win_rect.top;

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

// TO: ~ske
// After implementing these functions, please remove the "linux only" note in
// their umka docs.
void th_window_set_dims(th_vf2 dm) { }

void th_window_set_icon(th_image *img) { }

void th_window_show_cursor(bool show) { }

void th_window_freeze_cursor(bool freeze) { }

#else
#error tophat cant create a window on this platform yet
#endif

void th_window_begin_scissor(int x, int y, size_t w, size_t h) {
	// NOTE(skejeton): The flush is necessary because all the previous render
	//                 calls shouldn't be cut out using the rectangle.
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


