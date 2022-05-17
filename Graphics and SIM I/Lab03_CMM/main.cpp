// Lab03_CMM
// Collin Munson
// GAME 1343.1031
// May 30, 2007

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdio.h>

#include "resources.h"

#define CLASS_NAME "LAB03"

#define VK_E 0x45
#define VK_X 0x58
#define VK_I 0x49
#define VK_T 0x54

HINSTANCE hinstance_app = NULL;

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	char buffer[80];
	switch(message)
	{
		case WM_CREATE:
		{
			PlaySound(MAKEINTRESOURCE(ID_SOUND1),hinstance_app, SND_SYNC | SND_RESOURCE);
			PlaySound(MAKEINTRESOURCE(ID_SOUND2),hinstance_app, SND_ASYNC | SND_LOOP | SND_RESOURCE);
			hdc = GetDC(hwnd);
			SetTextColor(hdc,RGB(255,255,255));
			SetBkColor(hdc,RGB(0,0,0));
			SetBkMode(hdc,OPAQUE);
			ReleaseDC(hwnd,hdc);
			return 0;
		}break;
		case WM_PAINT:
		{
			
			hdc = BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);
			return 0;
		}break;
		case WM_KEYDOWN:
		{
			static unsigned int vk = 0;
			switch(wparam)
			{
				case VK_E:
					vk = VK_E;
					break;
				case VK_X:
					if (vk == VK_E) vk = VK_X;
					else vk = 0;
					break;
				case VK_I:
					if (vk == VK_X) vk = VK_I;
					else vk = 0;
					break;
				case VK_T:
					if (vk == VK_I) 
						SendMessage(hwnd,WM_DESTROY,wparam,lparam);
					else vk = 0;
					break;
				default: vk = 0; break;
			}
			
		}break;
		case WM_LBUTTONDOWN:
		{
			int x = (int)LOWORD(lparam);
			int y = (int)HIWORD(lparam);
			hdc = GetDC(hwnd);
			sprintf_s(buffer,"<--DOWN (%d, %d)",x,y);
			TextOut(hdc,x,y,buffer,(int)strlen(buffer));
			ReleaseDC(hwnd,hdc);
		}break;
		case WM_LBUTTONUP:
		{
			int x = (int)LOWORD(lparam);
			int y = (int)HIWORD(lparam);
			hdc = GetDC(hwnd);
			sprintf_s(buffer,"<--UP (%d, %d)",x,y);
			TextOut(hdc,x,y,buffer,(int)strlen(buffer));
			ReleaseDC(hwnd,hdc);
		}break;
		case WM_MOVE:
		{
			int x = LOWORD(lparam);
			int y = HIWORD(lparam);
			hdc = GetDC(hwnd);
			sprintf_s(buffer,"Window has been moved (%d, %d)      ",x,y);
			TextOut(hdc,0,16,buffer,(int)strlen(buffer));
			ReleaseDC(hwnd,hdc);
			return(0);
		}break;
		case WM_SIZE:
		{
			int width = LOWORD(lparam);
			int height = HIWORD(lparam);
			hdc = GetDC(hwnd);
			sprintf_s(buffer,"Window size changed to (%dx%d)      ",width,height);
			TextOut(hdc,0,0,buffer,(int)strlen(buffer));
			ReleaseDC(hwnd,hdc);
			return(0);
		}break;
		case WM_COMMAND:
		{
			switch(LOWORD(wparam))
			{
				case MENU_FILE_ID_EXIT:
				{
					SendMessage(hwnd,WM_DESTROY,wparam,lparam);
				}break;
				case MENU_COLORMSG_ID_BLUE:
				{
					hdc = GetDC(hwnd);
					SetTextColor(hdc,RGB(0,0,255));
					sprintf_s(buffer,"Text color has changed to Blue   ");
					TextOut(hdc,0,32,buffer,(int)strlen(buffer));
					ReleaseDC(hwnd,hdc);
				}break;
				case MENU_COLORMSG_ID_RED:
				{	
					hdc = GetDC(hwnd);
					SetTextColor(hdc,RGB(255,0,0));
					sprintf_s(buffer,"Text color has changed to Red     ");
					TextOut(hdc,0,32,buffer,(int)strlen(buffer));
					ReleaseDC(hwnd,hdc);
				}break;
				case MENU_COLORMSG_ID_WHITE:
				{	
					hdc = GetDC(hwnd);
					SetTextColor(hdc,RGB(255,255,255));
					sprintf_s(buffer,"Text color has changed to White ");
					TextOut(hdc,0,32,buffer,(int)strlen(buffer));
					ReleaseDC(hwnd,hdc);
				}break;
				case MENU_COLORMSG_ID_YELLOW:
				{	
					hdc = GetDC(hwnd);
					SetTextColor(hdc,RGB(255,255,0));
					sprintf_s(buffer,"Text color has changed to Yellow");
					TextOut(hdc,0,32,buffer,(int)strlen(buffer));
					ReleaseDC(hwnd,hdc);
				}break;
				case MENU_COLORMSG_ID_GREEN:
				{	
					hdc = GetDC(hwnd);
					SetTextColor(hdc,RGB(0,255,0));
					sprintf_s(buffer,"Text color has changed to Green  ");
					TextOut(hdc,0,32,buffer,(int)strlen(buffer));
					ReleaseDC(hwnd,hdc);
				}break;
				case MENU_HELP_ID_ABOUT:
				{
					MessageBox(hwnd,"Programmed by Collin Munson, Date Completed - May 30, 2007, Copyright","About",MB_OK);
				}break;
				default:break;
			}
			return 0;
		}break;
	
		case WM_DESTROY:
		{
			PlaySound(MAKEINTRESOURCE(ID_SOUND3),hinstance_app, SND_SYNC | SND_RESOURCE);
			PostQuitMessage(0);
		}break;
	}
	return(DefWindowProc(hwnd,message,wparam,lparam));
}

int WINAPI WinMain(HINSTANCE hinstance,HINSTANCE hprevinstance, LPSTR lpcmdline, int cmdshown)
{
	MSG msg; 
	HWND hwnd;

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(ID_ICON1));
	wc.hCursor = LoadCursor(hinstance, MAKEINTRESOURCE(ID_CURSOR1));
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(MainMenu);
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = LoadIcon(hinstance,MAKEINTRESOURCE(ID_ICON1));

	if(!RegisterClassEx(&wc)) 
		return(0);

	if(!(hwnd = CreateWindowEx(0,CLASS_NAME,"LAB03",
		WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_POPUP,0,0,400,400,
		0,0,hinstance,0)))
		return(0);

	hinstance_app = hinstance;
		
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT) 
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return(msg.message);
}