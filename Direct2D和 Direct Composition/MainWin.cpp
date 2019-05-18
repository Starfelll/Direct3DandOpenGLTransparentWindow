#include "WindowClass.cpp"

class MainWin :public Window {
    MainWin(HINSTANCE instance)
        :Window(instance)
    {

    }
	public:MainWin():Window(nullptr){}


protected:
	LRESULT WndProc(HWND hWnd,UINT message ,WPARAM wParam,LPARAM lParam){
		switch(message){

		}
		return 0;
	}


    
}g_mainwin;