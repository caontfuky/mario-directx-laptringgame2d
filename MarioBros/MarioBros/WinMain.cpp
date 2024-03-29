//Input : 
//d3d9.lib d3dx9.lib dinput8.lib dxguid.lib winmm.lib
#include "windows.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "Game.h"
#include "Graphic.h"
#include "Input.h"
#include "string"
using namespace std;


HINSTANCE hInstance;
HWND hWnd;

bool InitWin(HINSTANCE);
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPWSTR lpCmdLine,int nShowCmd)
{
	MSG msg;
	ZeroMemory(&msg,sizeof(msg));
	if(!InitWin(hInstance)) return 0;
	Game* game=new Game(hWnd,hInstance);
	int done=0;
	while(!done)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT) done=1;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		game->GameRun();
	}
	if(game) delete game;
	game=NULL;
	return 1;
}
bool InitWin(HINSTANCE hInstance)
{
	WNDCLASS w;
	w.cbClsExtra=0;
	w.cbWndExtra=0;
	w.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	w.hCursor=LoadCursor(hInstance,IDC_ARROW);
	w.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	w.hInstance=hInstance;
	w.lpfnWndProc=(WNDPROC)WndProc;
	w.lpszClassName="NameOfClass";
	w.lpszMenuName=NULL;
	w.style=CS_HREDRAW|CS_VREDRAW;
	if(!RegisterClass(&w)) return false;
	hWnd=CreateWindow("NameOfClass","MarioBros",WS_OVERLAPPEDWINDOW,0,30,SCREEN_WIDTH,SCREEN_HEIGHT,NULL,NULL,hInstance,NULL);
	if(hWnd==false) return false;
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);
	return true;
}
LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch(msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_F8:
			PostQuitMessage(0);
			break;
		}
	}
	return DefWindowProc(hWnd,msg,wParam,lParam);
}

