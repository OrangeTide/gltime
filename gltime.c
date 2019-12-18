#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>

static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static void init(HINSTANCE hInstance, HINSTANCE hPrevInstance);
static HWND newwin(HINSTANCE hInstance, int nShowCmd);
static void makegl(HWND hWnd, HDC *phdc);
static void loop(HWND hWnd);
static void failure(void);
static void reshape(int width, int height);
static void paint(HWND hWnd, HDC hDC);

int PASCAL
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HWND hWnd;

	init(hInstance, hPrevInstance);
	hWnd = newwin(hInstance, nShowCmd);
	loop(hWnd);

	return 0; // TODO: wParam from WM_QUIT
}

static void
init(HINSTANCE hInstance, HINSTANCE hPrevInstance)
{
	if (!hPrevInstance) {
		WNDCLASS wc = {
			.style = 0,
			.lpfnWndProc = (WNDPROC)WndProc,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = hInstance,
			.hIcon = LoadIcon((HINSTANCE) NULL, IDI_APPLICATION),
			.hCursor = LoadCursor((HINSTANCE) NULL, IDC_ARROW),
			.hbrBackground = NULL,
			.lpszMenuName = NULL, // TODO: configure a menu
			.lpszClassName = "MainWndClass",
		};

		if (!RegisterClass(&wc))
			failure();
	}
}

static HWND
newwin(HINSTANCE hInstance, int nShowCmd)
{
	const int width = 640, height = 480;
	const char title[] = "Title";
	HWND hWnd;
	HDC hDC;

	hWnd = CreateWindowEx(WS_EX_APPWINDOW, "MainWndClass", title, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, width, height, NULL, NULL, hInstance, NULL);
	if (hWnd == 0)
		failure();

	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	makegl(hWnd, &hDC);
	reshape(width, height);
	paint(hWnd, hDC);

	return hWnd;
}

static void
makegl(HWND hWnd, HDC *phdc)
{
	const PIXELFORMATDESCRIPTOR pfd = {
		.nSize		= sizeof(PIXELFORMATDESCRIPTOR),
		.nVersion	= 1,
		.dwFlags	= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		.iPixelType	= PFD_TYPE_RGBA,
		.cColorBits	= 16,
		.cDepthBits	= 16,
	};
	HDC hDC;
	GLuint iPixelFormat;
	HGLRC hRC;

	hDC = GetDC(hWnd);

	iPixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (!iPixelFormat)
		failure();

	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

	*phdc = hDC;
}

static LRESULT CALLBACK
WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;

	switch (uMsg) {
	case WM_DESTROY:
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		printf("KEY %04x\n", LOWORD(wParam));
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_SIZE:
		reshape(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_PAINT:
		hDC = BeginPaint(hwnd, &ps);
		paint(hwnd, hDC);
		EndPaint(hwnd, &ps);

		printf("WM_PAINT\n");

		break;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

static void
loop(HWND hWnd)
{
	MSG msg;

	// SetTimer(NULL, 0, frame_rate_delay, NULL);
	while (1) {
		if (0) {
			/* foreground loop */
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE | PM_NOYIELD)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					break;
			} else {
	//			paint(hWnd, hDC);
			}
		} else {
			/* background loop */
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

static void
failure(void)
{
	// DWORD dwErrorCode = GetLastError();
	MessageBoxA(NULL, "ERROR", "Something went wrong!", MB_OK);
	ExitProcess(1);
}

static void
reshape(int width, int height)
{
	// glViewport(0, 0, width, height);

	// TODO: set up projection
}

static void
paint(HWND hWnd, HDC hDC)
{
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// TODO: draw something

	SwapBuffers(hDC);
}
