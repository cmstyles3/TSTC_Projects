//Lab02_CMM - main.cpp

//Collin Munson
//GAME 1343.1031
//May 14, 2007

#define WIN32_LEAN_AND_MEAN //mfc sucks 

#include <windows.h>   
#include <windowsx.h>  

#define WINDOW_CLASS_NAME "WINCLASS1"
#define NUM_WINDOWS 2

int wincount = NUM_WINDOWS;

LRESULT CALLBACK WindowProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	PAINTSTRUCT ps;		
	HDC	hdc;	
	
	switch(msg)
	{	
		case WM_CREATE: 
		{	

		return(0);
		} break;

		case WM_PAINT: 
		{
			
			hdc = BeginPaint(hwnd,&ps);	 
			
			EndPaint(hwnd,&ps);
			return(0);
		} break;

		case WM_DESTROY: 
		{
			if(!(--wincount))
				PostQuitMessage(0);
			return(0);
		} break;

		default:break;
	} 
	return (DefWindowProc(hwnd, msg, wparam, lparam));
}

int WINAPI WinMain(	HINSTANCE hinstance,HINSTANCE hprevinstance,LPSTR lpcmdline,int ncmdshow)
{

	WNDCLASSEX winclass;
	HWND hwnd[NUM_WINDOWS];
	MSG msg;

	winclass.cbSize         = sizeof(WNDCLASSEX);
	winclass.style			= CS_DBLCLKS | CS_OWNDC | 
							  CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL;
	winclass.lpszClassName	= WINDOW_CLASS_NAME;
	winclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

	
	if (!RegisterClassEx(&winclass))
		return 0;

	if (!(hwnd[0] = CreateWindowEx(NULL,WINDOW_CLASS_NAME,"1st Window",
	WS_OVERLAPPEDWINDOW | WS_VISIBLE,0,0,400,400,NULL,NULL,hinstance,NULL)))
		return 0;

	if (!(hwnd[1] = CreateWindowEx(NULL,WINDOW_CLASS_NAME,"2nd Window",
	WS_OVERLAPPEDWINDOW | WS_VISIBLE,400,400,400,400,NULL,NULL,hinstance,NULL)))
		return 0;
	
	while(1)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return(msg.wParam);
} 



