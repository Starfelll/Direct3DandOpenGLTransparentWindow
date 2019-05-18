// areoGL2.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "areoGL2.h"


#include <windows.h>
#include <windowsx.h>
#include <GL/gl.h>
#include <GL/glu.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#include <assert.h>
#include <tchar.h>

#ifdef  assert
#define verify(expr) if(!expr) assert(0)
#else verify(expr) expr
#endif

const TCHAR szAppName[] = _T("TransparentGL");
const TCHAR wcWndName[] = _T("WS_EX_LAYERED OpenGL");

HDC hDC;
HGLRC m_hrc;
int w(240);
int h(240);

HDC pdcDIB;
HBITMAP hbmpDIB;
void *bmp_cnt(NULL);
int cxDIB(0);
int cyDIB(0);
BITMAPINFOHEADER BIH;


BOOL initSC()
{
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

void resizeSC(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

BOOL renderSC()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	glColor3f(0, 1, 1);
	glBegin(GL_TRIANGLES);                              // Drawing Using Triangles
	glColor3f(1.0f, 0.0f, 0.0f);                      // Set The Color To Red
	glVertex3f(0.0f, 1.0f, 0.0f);                  // Top
	glColor3f(0.0f, 1.0f, 0.0f);                      // Set The Color To Green
	glVertex3f(-1.0f, -1.0f, 0.0f);                  // Bottom Left
	glColor3f(0.0f, 0.0f, 1.0f);                      // Set The Color To Blue
	glVertex3f(1.0f, -1.0f, 0.0f);                  // Bottom Right
	glEnd();

	glPopMatrix();
	glFlush();

	return 0;
}

// DIB -> hDC
void draw(HDC pdcDest)
{
	assert(pdcDIB);

	verify(BitBlt(pdcDest, 0, 0, w, h, pdcDIB, 0, 0, SRCCOPY));
}

void CreateDIB(int cx, int cy)
{
	assert(cx > 0);
	assert(cy > 0);

	cxDIB = cx;
	cyDIB = cy;

	int iSize = sizeof(BITMAPINFOHEADER);
	memset(&BIH, 0, iSize);

	BIH.biSize = iSize;
	BIH.biWidth = cx;
	BIH.biHeight = cy;
	BIH.biPlanes = 1;
	BIH.biBitCount = 24;
	BIH.biCompression = BI_RGB;

	if (pdcDIB)
		verify(DeleteDC(pdcDIB));

	pdcDIB = CreateCompatibleDC(NULL);
	assert(pdcDIB);

	if (hbmpDIB)
		verify(DeleteObject(hbmpDIB));

	hbmpDIB = CreateDIBSection(
		pdcDIB,
		(BITMAPINFO*)&BIH,
		DIB_RGB_COLORS,
		&bmp_cnt,
		NULL,
		0);

	assert(hbmpDIB);
	assert(bmp_cnt);

	if (hbmpDIB)
		SelectObject(pdcDIB, hbmpDIB);
}

BOOL CreateHGLRC()
{
	DWORD dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_BITMAP;

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = dwFlags;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int PixelFormat = ChoosePixelFormat(pdcDIB, &pfd);
	if (PixelFormat == 0) {
		assert(0);
		return FALSE;
	}

	BOOL bResult = SetPixelFormat(pdcDIB, PixelFormat, &pfd);
	if (bResult == FALSE) {
		assert(0);
		return FALSE;
	}

	m_hrc = wglCreateContext(pdcDIB);
	if (!m_hrc) {
		assert(0);
		return FALSE;
	}

	return TRUE;
}

LRESULT CALLBACK WindowFunc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;

	switch (msg)
	{
	case WM_ERASEBKGND:
		return 0;
		break;

	case WM_CREATE:
		break;

	case WM_DESTROY:
		if (m_hrc)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(m_hrc);
		}
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		//hDC = GetDC(hWnd);
		//OutputDebugString(TEXT("sss"));
		renderSC(); // OpenGL -> DIB
		draw(hDC);  // DIB -> hDC
		EndPaint(hWnd, &ps);
		break;

	case WM_SIZE:
		w = LOWORD(lParam); h = HIWORD(lParam);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hrc);

		CreateDIB(w, h);
		CreateHGLRC();
		verify(wglMakeCurrent(pdcDIB, m_hrc));

		initSC();
		resizeSC(w, h);
		renderSC();
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPSTR str, int nWinMode)
{
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
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = szAppName;

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, _T("RegisterClassEx - failed"), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	HWND hWnd = CreateWindowEx(WS_EX_LAYERED, szAppName, wcWndName,
		WS_VISIBLE | WS_POPUP, 200, 150, w, h,
		NULL, NULL, hThisInst, NULL);
	if (!hWnd) {
		MessageBox(NULL, _T("CreateWindowEx - failed"), _T("Error"), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	verify(SetLayeredWindowAttributes(hWnd, 0x0, 0, LWA_COLORKEY));

	MSG msg;
	while (1)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else return 0;
		}
	}

	return (FALSE);
}