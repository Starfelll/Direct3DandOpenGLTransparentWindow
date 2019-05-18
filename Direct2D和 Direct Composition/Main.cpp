
#define UNICODE 1
#define _UNICODE 1


#include "MainWin.cpp"
#include "d2d_code.cpp"
#include <string>
int APIENTRY wWinMain(HINSTANCE hInstance , 
					HINSTANCE hPrevInstance,
					LPWSTR lpCmdLine,
					int nCmdShow)
{

    
	MSG msg;
	
	g_mainwin.Create(WS_EX_NOREDIRECTIONBITMAP,TEXT("Direct2D_AlphaWindow"),
						WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,1024,768);
	g_mainwin.Show(SW_SHOW);
	
	InitDirect2D(g_mainwin);
	msg.message = 0;

	int fps = 0;
	float s = 0;
	int sumFps = 0;
	while (msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg,0,0,0,PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
		}else{
			g_timer.Tick();

			s += g_timer.DeltaTime();
			if(s > 1){
				s = 0;
				fps = sumFps;
				using namespace std;
				SetWindowText(g_mainwin.GetHwnd(),
				reinterpret_cast <LPCWSTR>(   (wstring(L"fps: ") + std::to_wstring(fps)).c_str() ));
				sumFps = 1;
			}else{
				sumFps ++;
			}
			
			RenderDirect2D();
		}
	}
	DestroyResource();
	return 0;
}

/*
#ifndef UNICODE 
#define UNICODE 
#endif 
#include <windows.h> 
#pragma comment(lib, "user32.lib") 
int __stdcall wWinMain(HINSTANCE module, HINSTANCE, PWSTR, int) { 
	WNDCLASS wc = {}; 
	wc.hCursor       = LoadCursor(nullptr, IDC_ARROW); 
	wc.hInstance     = module; wc.lpszClassName = L"window"; 
	wc.style         = CS_HREDRAW | CS_VREDRAW; 
	wc.lpfnWndProc = [] (HWND window, UINT message, WPARAM wparam, LPARAM lparam) -> LRESULT 
	{ 
		if (WM_DESTROY == message) { PostQuitMessage(0); return 0; } 
		return DefWindowProc(window, message, wparam, lparam); 
	}; 
	RegisterClass(&wc); 
	HWND const window = 
	CreateWindow(wc.lpszClassName, L"Sample", 
	WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	nullptr, nullptr, module, nullptr); 
	MSG message; 
	while (BOOL result = GetMessage(&message, 0, 0, 0)) 
	{ 
		if (-1 != result) DispatchMessage(&message); 
	} 
}
*/

