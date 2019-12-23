/* Copyright 2019 Jon Mayo
 *
 * Modification and redistribution is permitted. The works are without warranty.
 */
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <tchar.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#define BUFFER_OFFSET(p) ((void*)(intptr_t)p)

#define MY_CLASS _T("MyWndClass")

static PFNGLUSEPROGRAMPROC glUseProgram;
static PFNGLGENBUFFERSPROC glGenBuffers;
static PFNGLBINDBUFFERPROC glBindBuffer;
static PFNGLDELETEBUFFERSPROC glDeleteBuffers;
static PFNGLBUFFERDATAPROC glBufferData;

static int doublebuffer = 1;
static HGLRC hRC;

static const GLfloat my_vertices[] = {
	/* Cube */
	0.5f, 0.5f, 0.5f,	-0.5f, 0.5f, 0.5f,	-0.5f, -0.5f, 0.5f,	0.5f, -0.5f, 0.5f, /* front */
	0.5f, 0.5f, 0.5f,	0.5f, -0.5f, 0.5f,	0.5f, -0.5f, 0.5f,	0.5f, 0.5f, -0.5f, /* right */
	0.5f, 0.5f, 0.5f,	0.5f, 0.5f, -0.5f,	-0.5f, 0.5f, -0.5f,	-0.5f, 0.5f, 0.5f, /* top */
	-0.5f, 0.5f, 0.5f,	-0.5f, 0.5f, -0.5f,	-0.5f, -0.5f, -0.5f,	-0.5f, -0.5f, 0.5f, /* left */
	-0.5f, -0.5f, -0.5f,	0.5f, -0.5f, -0.5f,	0.5f, -0.5f, 0.5f,	-0.5f, -0.5f, 0.5f, /* bottom */
	0.5f, -0.5f, -0.5f,	-0.5f, -0.5f, -0.5f,	-0.5f, 0.5f, -0.5f,	0.5f, 0.5f, -0.5f, /* back */
};

static const GLushort my_indices[] = {
	0, 1, 2,	2, 3, 0,	/* front */
	4, 5, 6,	6, 7, 4,	/* right */
	8, 9, 10,	10, 11, 8,	/* top */
	12, 13, 14,	14, 15, 12,	/* left */
	16, 17, 18,	18, 19, 16,	/* bottom */
	20, 21, 22,	22, 23, 20,	/* back */
};

static const GLenum vbo_type[] = {
	GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER,
};
static GLuint vbo[sizeof(vbo_type) / sizeof(*vbo_type)];
static const GLsizei vbo_size[sizeof(vbo_type) / sizeof(*vbo_type)] = {
	sizeof(my_vertices),
	sizeof(my_indices),
};
static const void *vbo_data[sizeof(vbo_type) / sizeof(*vbo_type)] = {
	my_vertices,
	my_indices,
};

enum colors {
	C_BACKGROUND,
	C_OBJECT,
	C_HIGHLIGHT,
	C_SECONDARY,
	C_TEXT,
	C_MAX
};

struct color {
	GLfloat r, g, b;
} palette[][C_MAX] = {
	{
		/* dark sand */
		{ 0.039, 0.035, 0.031 },
		{ 0.133, 0.200, 0.231 },
		{ 0.000, 0.055, 0.682 },
		{ 0.776, 0.675, 0.561 },
		{ 0.369, 0.314, 0.247 },
	},
	{
		/* island */
		{ 0.247, 0.486, 0.675 },
		{ 0.584, 0.686, 0.729 },
		{ 0.741, 0.769, 0.655 },
		{ 0.835, 0.882, 0.639 },
		{ 0.886, 0.973, 0.612 },
	},
};

static void
app_once(void)
{
	unsigned i, n = sizeof(vbo) / sizeof(*vbo);

	glGenBuffers(n, vbo);

	for (i = 0; i < n; i++) {
		glBindBuffer(vbo_type[i], vbo[i]);

		glBufferData(vbo_type[i], vbo_size[i], vbo_data[i], GL_STATIC_DRAW);
	}
}

static void
app_done(void)
{
	unsigned i, n = sizeof(vbo) / sizeof(*vbo);

	for (i = 0; i < n; i++) {
		glBindBuffer(vbo_type[i], 0);
	}

	glDeleteBuffers(n, vbo);
}

static void
app_resize(INT width, INT height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);
	gluPerspective(45.0, (double)width / (double)height, 0.1, 20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

static void
app_paint(void)
{
	unsigned i, n = sizeof(vbo) / sizeof(*vbo);
	struct color bg = palette[0][C_BACKGROUND];
	struct color fg = palette[0][C_OBJECT];
	struct color hi = palette[0][C_HIGHLIGHT];
	void *offset1 = BUFFER_OFFSET(0); // (void*)(intptr_t)0;
	GLsizei stride = 3 * sizeof(*my_vertices);
	GLsizei vertex_count = vbo_size[0] / stride;

	glClearColor(bg.r, bg.g, bg.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);

#if 1 // Draws two blue triangles
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// TODO: gluLookAt(1, 2, 3, 4, 5, 6, 7, 8);
	glTranslatef(-0.5, -0.5, -2.0);

	glColor3f(hi.r, hi.g, hi.b);
	glBegin(GL_TRIANGLES);
	/* front facing */
	glVertex3f(0., 0., 0.);
	glVertex3f(1., 0., 0.);
	glVertex3f(1., 1., 0.);

	/* back facing */
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 1., 0.);
	glVertex3f(1., 1., 0.);
	glEnd();
#endif

#if 1 // draws our VBO: a cube
	for (i = 0; i < n; i++) {
		glBindBuffer(vbo_type[i], vbo[i]);
	}

	/* enable client state */
	glEnableClientState(GL_VERTEX_ARRAY);
	// glEnableClientState(GL_NORMAL_ARRAY);
	// glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);

	glVertexPointer(3, GL_FLOAT, stride, offset1);
	// glNormalPointer(GL_FLOAT, stride, TBD);
	// glTexCoordPointer(2, GL_FLOAT, stride, TBD);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -10.0);

	static GLfloat m = 0.0f;
	glRotatef(m, 0.5, 0.5, 0.0);
	m += 1.0; // TODO: this is not the proper place to update state. follow MVC principles!

	glColor3f(fg.r, fg.g, fg.b);
	glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

	/* disable the things we enabled */
	glDisableClientState(GL_VERTEX_ARRAY);

	/* unbind the things we bound */
	for (i = 0; i < n; i++) {
		glBindBuffer(vbo_type[i], 0);
	}
#endif

}

static int
loadextensions(void)
{
	const GLubyte *extensions = glGetString(GL_EXTENSIONS);

	// TODO: parse string

	glUseProgram = (PFNGLUSEPROGRAMPROC)(void*)wglGetProcAddress("glUseProgram");
	glGenBuffers = (PFNGLGENBUFFERSPROC)(void*)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)(void*)wglGetProcAddress("glBindBuffer");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)(void*)wglGetProcAddress("glDeleteBuffers");
	glBufferData = (PFNGLBUFFERDATAPROC)(void*)wglGetProcAddress("glBufferData");

	if (!glUseProgram || !glGenBuffers || !glBindBuffer || !glDeleteBuffers || !glBufferData) {
		return -1;
	}

	return 0;
}

static int
post_key_event(unsigned down, unsigned keycode)
{
	// TODO: look up keys on a hash table
	switch (keycode) {
	// case VK_F1:
	// TODO: toggle a menu: SetMenu(hwnd, toggle ? hmenu : NULL);
	case VK_ESCAPE:
		if (down)
			PostQuitMessage(0);
		return 0;
	}

	return 1;
}

static void
do_paint(HDC hDC)
{
	wglMakeCurrent(hDC, hRC);
	app_paint();
	if (doublebuffer)
		SwapBuffers(hDC);
}

static LRESULT CALLBACK
WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;

	switch (message) {
	case WM_CREATE: {
		INT iPixelFormat;
		PIXELFORMATDESCRIPTOR pfd;
		LPCREATESTRUCT cs = (LPCREATESTRUCT)lParam;

		hDC = GetDC(hwnd);
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
		pfd.nVersion = 1;
		pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
		if (doublebuffer)
			pfd.dwFlags |= PFD_DOUBLEBUFFER;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 16;
		pfd.cDepthBits = 16;

		iPixelFormat = ChoosePixelFormat(hDC, &pfd);
		if (!iPixelFormat) {
			MessageBox(NULL, _T("Unable to initialize OpenGL compatible pixel format"), NULL, MB_OK | MB_ICONEXCLAMATION);
			PostQuitMessage(1);
			return -1;
		}

		DescribePixelFormat(hDC, iPixelFormat, sizeof(pfd), &pfd);
		SetPixelFormat(hDC, iPixelFormat, &pfd);

		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);

		if (loadextensions()) {
			MessageBox(NULL, _T("Unable to initialize modern OpenGL API"), NULL, MB_OK | MB_ICONEXCLAMATION);
			wglMakeCurrent(hDC, 0);
			wglDeleteContext(hRC);
			return -1;
		}

		app_once();
		app_resize(cs->cx, cs->cy);

		return 0;
	}
	case WM_DESTROY: {
		hDC = GetDC(hwnd);
		wglMakeCurrent(hDC, hRC);
		app_done();
		wglMakeCurrent(hDC, 0);
		wglDeleteContext(hRC);
		PostQuitMessage(0);
		return 0;
	}
	/* DefWindowProc already does this ...
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
	*/
	case WM_PAINT: {
		hDC = BeginPaint(hwnd, &ps);
		do_paint(hDC);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_SIZE:
		app_resize(LOWORD(lParam), HIWORD(lParam));
		return 0;
	case WM_KEYDOWN:
		if (post_key_event(1, wParam) == 0)
			return 0;
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

int APIENTRY
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	HWND hwnd;
	TCHAR title[] = _T("Title");
	INT width = 640, height = 480;
	MSG msg;
	DWORD frametime = 1000 / 60; /* 60 FPS */

	if (!hPrevInstance) {
		ZeroMemory(&wc, sizeof(wc));
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.lpszClassName = MY_CLASS;
		RegisterClass(&wc);
	}

	hwnd = CreateWindowEx(WS_EX_APPWINDOW, MY_CLASS, title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, width, height, NULL, NULL, hInstance, NULL);
	if (!hwnd)
		goto failure;
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (1) {
		DWORD nCount = 0;
		if (MsgWaitForMultipleObjects(0, NULL, FALSE, frametime, QS_ALLINPUT) == WAIT_OBJECT_0 + nCount) {
			while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT)
					goto done;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		do_paint(GetDC(hwnd));
	}

	/* TODO: A more basic loop to use if we're not animating stuff *//*
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	*/
done:
	UnregisterClass(MY_CLASS, hInstance);

	return 0;
failure:
	return 1;
}
