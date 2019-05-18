// areoGL.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "areoGL.h"


#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <dwmapi.h>
#pragma comment (lib, "dwmapi.lib")


#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")



#include <assert.h>
#include <tchar.h>

#ifdef  assert
#define verify(expr) if(!expr) assert(0)
#else verify(expr) expr
#endif

const TCHAR szAppName[] = _T("TransparentGL");
const TCHAR wcWndName[] = _T("TransparentGL");

HDC hDC;
HGLRC m_hrc;
int w = 240;
int h = 240;

BOOL initSC() {
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0, 0, 0, 0);

	return 0;
}

void resizeSC(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

BOOL renderSC() {

	
	
	glClearColor(0,0,0,0.0f);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glColor3f(0, 1, 1);
	glBegin(GL_TRIANGLES);                              // Drawing Using Triangles
	glColor4f(1.0f, 0.0f, 0.0f,0.5f);                      // Set The Color To Red
	glVertex3f(0.0f, 1.0f, 0.0f);                  // Top
	glColor4f(0.0f, 1.0f, 0.0f,0.5f);                      // Set The Color To Green
	glVertex3f(-1.0f, -1.0f, 0.0f);                  // Bottom Left
	glColor4f(0.0f, 0.0f, 1.0f,0.5f);                      // Set The Color To Blue
	glVertex3f(1.0f, -1.0f, 0.0f);                  // Bottom Right
	glEnd();

	glPopMatrix();
	glFlush();

	return 0;
}

BOOL CreateHGLRC(HWND hWnd) {
	PIXELFORMATDESCRIPTOR pfd = {
	  sizeof(PIXELFORMATDESCRIPTOR),
	  1,                                // Version Number
	  PFD_DRAW_TO_WINDOW |         // Format Must Support Window
	  PFD_SUPPORT_OPENGL |         // Format Must Support OpenGL
	  PFD_SUPPORT_COMPOSITION |         // Format Must Support Composition
	  PFD_DOUBLEBUFFER,                 // Must Support Double Buffering
	  PFD_TYPE_RGBA,                    // Request An RGBA Format
	  32,                               // Select Our Color Depth
	  0, 0, 0, 0, 0, 0,                 // Color Bits Ignored
	  8,                                // An Alpha Buffer
	  0,                                // Shift Bit Ignored
	  0,                                // No Accumulation Buffer
	  0, 0, 0, 0,                       // Accumulation Bits Ignored
	  24,                               // 16Bit Z-Buffer (Depth Buffer)
	  8,                                // Some Stencil Buffer
	  0,                                // No Auxiliary Buffer
	  PFD_MAIN_PLANE,                   // Main Drawing Layer
	  0,                                // Reserved
	  0, 0, 0                           // Layer Masks Ignored
	};

	HDC hdc = GetDC(hWnd);
	int PixelFormat = ChoosePixelFormat(hdc, &pfd);
	if (PixelFormat == 0) {
		assert(0);
		return FALSE;
	}

	BOOL bResult = SetPixelFormat(hdc, PixelFormat, &pfd);
	if (bResult == FALSE) {
		assert(0);
		return FALSE;
	}

	m_hrc = wglCreateContext(hdc);
	if (!m_hrc) {
		assert(0);
		return FALSE;
	}

	ReleaseDC(hWnd, hdc);

	return TRUE;
}

LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;

	switch (msg) {
	case WM_CREATE:
		break;

	case WM_DESTROY:
		if (m_hrc) {
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(m_hrc);
		}
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode) {
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowFunc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hThisInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)CreateSolidBrush(0x00000000);
	wc.lpszClassName = szAppName;

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, _T("RegisterClassEx - failed"), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	HWND hWnd = CreateWindowEx(NULL, szAppName, wcWndName,
		WS_VISIBLE | WS_OVERLAPPEDWINDOW, 200, 150, w, h,
		NULL, NULL, hThisInst, NULL);

	if (!hWnd) {
		MessageBox(NULL, _T("CreateWindowEx - failed"), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	DWM_BLURBEHIND bb = { 0 };
	HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
	bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
	bb.hRgnBlur = hRgn;
	bb.fEnable = TRUE;
	DwmEnableBlurBehindWindow(hWnd, &bb);



	CreateHGLRC(hWnd);

	HDC hdc = GetDC(hWnd);
	wglMakeCurrent(hdc, m_hrc);
	initSC();
	resizeSC(w, h);
	ReleaseDC(hWnd, hdc);

	MSG msg;
	while (1) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			HDC hdc = GetWindowDC(hWnd);
			wglMakeCurrent(hdc, m_hrc);

			renderSC();

			SwapBuffers(hdc);
			ReleaseDC(hWnd, hdc);
		}
	}

	return (FALSE);
}