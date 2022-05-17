#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "resource.h"

#define WINDOW_CLASS "TEST"

HWND g_hwnd1, g_hwnd2;
HINSTANCE g_hinstance;
bool main_visible = true;


LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch(msg)
	{
		case WM_CREATE:
		{
			PlaySound("StartSound",g_hinstance,SND_RESOURCE | SND_SYNC);
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
				case MENU_VIEW_ID_MAIN:
				{
					ShowWindow(g_hwnd1, SW_SHOW);
					ShowWindow(g_hwnd2, SW_HIDE);
					main_visible = true;
					
				}break;
				case MENU_VIEW_ID_SECOND:
				{
					ShowWindow(g_hwnd1, SW_HIDE);
					ShowWindow(g_hwnd2, SW_SHOW);
					main_visible = false;
				}break;
				case MENU_HELP_ID_ABOUT:
				{
					MessageBox(NULL, "Programmed By Collin Munson \nDate Completed June 20, 2007 \nCopyright 2007", "About", MB_OK);
				}break;
			}
		}break;
		case WM_PAINT:
		{
			
			hdc = BeginPaint(hwnd, &ps);
			if(main_visible)
			{
				HPEN pen;
				HBRUSH brush;
				
				pen = CreatePen(PS_SOLID, 2, RGB(0,0,255));
				brush = CreateSolidBrush(RGB(0,255,0));
				SelectObject(hdc,pen);
				SelectObject(hdc,brush);
				
				RECT rect = {10,10,270, 300};
				FrameRect(hdc,&rect,CreateSolidBrush(RGB(100,0,200)));

				rect.top = 30;
				rect.bottom = 100;
				rect.left = 65;
				rect.right = 180;
				FrameRect(hdc,&rect,CreateSolidBrush(RGB(200,100,0)));

				rect.top = 110;
				rect.bottom = 190;
				rect.left = 200;
				rect.right = 380;
				FrameRect(hdc,&rect,CreateSolidBrush(RGB(0,0,255)));
				
				Ellipse(hdc, 60, 50, 120, 110);
				Ellipse(hdc, 100, 100, 200, 200);
				Ellipse(hdc, 40, 300, 80, 340);
				if(pen) DeleteObject(pen);
				if(brush) DeleteObject(brush);
				
			}
			else
			{
				HPEN pen = CreatePen(PS_SOLID, 2, RGB(0,0,255));;
				HBRUSH brush = CreateSolidBrush(RGB(0,255,0));;

				SelectObject(hdc,pen);
				SelectObject(hdc,brush);
				RECT rect = {10,10,20, 20};
				FillRect(hdc,&rect,CreateSolidBrush(RGB(255,0,0)));

				rect.top = 30;
				rect.bottom = 80;
				rect.left = 60;
				rect.right = 110;
				FillRect(hdc,&rect,CreateSolidBrush(RGB(0,255,255)));

				rect.top = 110;
				rect.bottom = 190;
				rect.left = 200;
				rect.right = 280;
				FillRect(hdc,&rect,CreateSolidBrush(RGB(90,170,0)));

				POINT poly1[] = { {10, 300}, {30, 300 }, {40, 315}, {40, 380}, { 20, 385}} ;
				Polygon(hdc, poly1, 5);
				POINT poly2[] = { {400, 250}, {420, 350}, {420,450}, {370, 400}, { 340, 400}};
				Polygon(hdc, poly2, 5);
				if(pen) DeleteObject(pen);
				if(brush) DeleteObject(brush);

			}
			EndPaint(hwnd, &ps);
			return(0);
		}break;
		case WM_DESTROY:
		{
			PlaySound("EndSound",g_hinstance,SND_RESOURCE | SND_SYNC);
			PostQuitMessage(0);
			return(0);
		}break;
	}
	return(DefWindowProc(hwnd,msg,wparam,lparam));
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevhinstance, LPSTR lpcmdline, int nshowcmd)
{
	MSG msg;
	HWND hwnd1, hwnd2;
	WNDCLASSEX wc;
	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hCursor = LoadCursor(hinstance,"MyCursor");
	wc.hIcon = LoadIcon(hinstance, "MyIcon");
	wc.hIconSm = LoadIcon(hinstance, "MyIcon");
	wc.hInstance = hinstance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = WINDOW_CLASS;
	wc.lpszMenuName = MAKEINTRESOURCE(MainMenu);
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_OWNDC;

	if(!RegisterClassEx(&wc))
		return(0);
	if(!(hwnd1 = CreateWindowEx(NULL, WINDOW_CLASS, "CODE TEST", WS_VISIBLE | WS_OVERLAPPEDWINDOW, 
		0, 0, 500, 500, NULL, NULL, hinstance, NULL)))
		return(0);
	if(!(hwnd2 = CreateWindowEx(NULL, WINDOW_CLASS, "CODE TEST", WS_OVERLAPPEDWINDOW, 
		0, 0, 500, 500, NULL, NULL, hinstance, NULL)))
		return(0);

	g_hwnd1 = hwnd1;
	g_hwnd2 = hwnd2;
	g_hinstance = hinstance;
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return(msg.wParam);
}