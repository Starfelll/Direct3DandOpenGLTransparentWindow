
#ifndef __WINDOWCLASS_H__
#define __WINDOWCLASS_H__


#include <Windows.h>
#include <assert.h>
#include <tchar.h>
#include <wchar.h>
#include <stdio.h>
#include "WndProcThunk.hpp"


class Window
{

public:

	Window(HINSTANCE hInst);
	~Window();

	inline HWND GetHwnd(){return this->m_HWnd;};


	SIZE GetClientSize();

	bool SetClientSize(int cWidth = -1,int cHeight = -1);

	

	bool Create(
		DWORD exStyle = 0,
		LPCTSTR titleText = TEXT(""),
		DWORD style = WS_OVERLAPPEDWINDOW,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int width = CW_USEDEFAULT,
		int height = CW_USEDEFAULT,
		HWND wndParent = nullptr,
		HMENU menu = nullptr,
		int cmdShow = SW_HIDE,
		WNDCLASSEX* partWndClass = nullptr
	);
	
	bool Create(int width = CW_USEDEFAULT, int height = CW_USEDEFAULT,int x = CW_USEDEFAULT, int y = CW_USEDEFAULT);
	bool Create();
	void Show(int nCmdShow = SW_SHOW);

	inline unsigned int GetClientWidth(){return this->m_ClientWidth;}
	inline unsigned int GetClientHeight(){return this->m_ClientHeight;}

protected:
	HWND m_HWnd;
	unsigned int m_Width;
	unsigned int m_Height;
	unsigned int m_ClientWidth;
	unsigned int m_ClientHeight;
	int m_X;
	int m_Y;
	HINSTANCE m_HInstance;
	WndProcThunk  m_WndProcThunk;


protected:
	static LRESULT CALLBACK  StaticWndProc( HWND hWnd ,UINT message ,WPARAM wParam , LPARAM lParam);
	static LRESULT CALLBACK InitWndProc(HWND hWnd,UINT message ,WPARAM wParam,LPARAM lParam);

	virtual LRESULT WndProc(HWND hWnd,UINT message ,WPARAM wParam,LPARAM lParam);
	
};





#endif