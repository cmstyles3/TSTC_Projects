#define WIN32_LEAN_AND_MEAN

#define INITGUID

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <stdio.h>
#include "ddraw.h"
#include <math.h>

#define CLASS_NAME "LAB06"

#define KEYDOWN(vk) (GetAsyncKeyState(vk) & 0x8000) ? 1 : 0
#define KEYUP(vk) (GetAsyncKeyState(vk) & 0x8000) ? 0 : 1

#define MAX_STARS 50
#define MAX_PARTICLES 150
#define MAX_FIREWORKS 5

#define PI 3.141592f

struct Star
{
	int x, y;
	int oldx, oldy;
	UCHAR color;
	bool change;
};

struct Particle
{
	float x, y;
	float oldx, oldy;
	float xv, yv;
	float df;
	int life;
	bool active;
};

struct Firework
{
	int x, y;
	int oldy;
	int dest;
	int state;
	bool active;
	UCHAR color;
	int counter, threshold;
	Particle particles[MAX_PARTICLES];
};

int GameMain();
int GameInit();
int GameKill();

void InitStars();
void ProcessStars();
void DrawStars();

void ProcessFireworks();
void DrawFireworks();

inline void SetPaletteColor(int, UCHAR, UCHAR, UCHAR);

inline void PlotPixel(int, int, UCHAR, UCHAR *);

Star stars[MAX_STARS] = {0};
Firework fireworks[MAX_FIREWORKS] = {0};

HWND g_hwnd = NULL;
HINSTANCE g_hinstance = NULL;

LPDIRECTDRAW7 lpdd7 = NULL;
LPDIRECTDRAWSURFACE7 lpddsprimary = NULL;
DDSURFACEDESC2 ddsd = {NULL};
LPDIRECTDRAWPALETTE lpddpal = NULL;
PALETTEENTRY palette[256] = {NULL};


LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	switch(msg)
	{
		case WM_CREATE:
		{

			return(0);
		}break;
		case WM_PAINT:
		{
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			return(0);
		}break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return(0);
		}break;
	}
	return(DefWindowProc(hwnd, msg, wparam, lparam));
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int nshowcmd)
{
	MSG msg;
	WNDCLASSEX wc;
	wc.cbClsExtra = 0;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hinstance;
	wc.lpfnWndProc = WinProc;
	wc.lpszClassName = CLASS_NAME;
	wc.lpszMenuName = NULL;
	wc.style = CS_DBLCLKS | CS_OWNDC | CS_VREDRAW | CS_HREDRAW;

	
	if(!RegisterClassEx(&wc))
		return(0);
	if(!(g_hwnd = CreateWindowEx(NULL, CLASS_NAME, "Fireworks Display", WS_VISIBLE | WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hinstance, NULL)))
		return(0);
	ShowCursor(false);
	srand((int)GetTickCount());
	GameInit();
	while(1)
	{
		PlaySound("night.wav", g_hinstance, SND_ASYNC | SND_LOOP | SND_NOSTOP);
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		GameMain();
	}
	GameKill();
	return((int)msg.wParam);
}

int GameInit()
{
	if (FAILED(DirectDrawCreateEx(NULL, (void **)&lpdd7, IID_IDirectDraw7, NULL)))
	{
		return(0);
	}

	if (FAILED(lpdd7->SetCooperativeLevel(g_hwnd, DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT)))
	{
		return(0);
	}

	if (FAILED(lpdd7->SetDisplayMode(640, 480, 8, 0, 0)))
	{
		return(0);
	}

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsprimary, NULL)))
	{
		return(0);
	}
	
	SetPaletteColor( 0,   0,   0,   0); //black

	SetPaletteColor( 1, 255, 255, 255); //white
	SetPaletteColor( 2, 200, 200, 200); //light grey
	SetPaletteColor( 3, 130, 130, 130); //grey
	SetPaletteColor( 4,  60,  60,  60); //very dark blue

	SetPaletteColor( 5, 255,   0,   0); //red
	SetPaletteColor( 6,   0, 255,   0); //green
	SetPaletteColor( 7,   0,   0, 255); //blue
	SetPaletteColor( 8, 255, 255,   0); //yellow
	SetPaletteColor( 9, 255,   0, 255); //purple
	SetPaletteColor(10,   0, 255, 255); //cyan

	SetPaletteColor(11, 130,   0,   0); //dark red
	SetPaletteColor(12,   0, 130,   0); //dark green
	SetPaletteColor(13,   0,   0, 130); //dark blue
	SetPaletteColor(14, 130, 130,   0); //dark yellow
	SetPaletteColor(15, 130,   0, 130); //dark purple
	SetPaletteColor(16,   0, 130, 130); //dark cyan
	
	if (FAILED(lpdd7->CreatePalette(DDPCAPS_8BIT | DDPCAPS_ALLOW256 | DDPCAPS_INITIALIZE, palette, &lpddpal, NULL)))
	{
		return(0);
	}
	
	if (FAILED(lpddsprimary->SetPalette(lpddpal)))
	{
		return(0);
	}

	InitStars();
	return(1);
}

int GameKill()
{
	if (lpddpal)
	{
		lpddpal->Release();
		lpddpal = NULL;
	}
	if (lpddsprimary)
	{
		lpddsprimary->Release();
		lpddsprimary = NULL;
	}
	if (lpdd7)
	{
		lpdd7->Release();
		lpdd7 = NULL;
	}
	return(1);
}

int GameMain()
{
	srand((int)GetTickCount());
	if (KEYDOWN(VK_ESCAPE))
		SendMessage(g_hwnd, WM_DESTROY, 0, 0);

	ProcessStars();
	ProcessFireworks();

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	
	if (FAILED(lpddsprimary->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)))
	{
		return(0);
	}
	
	DrawStars();
	DrawFireworks();
	if (FAILED(lpddsprimary->Unlock(NULL)))
	{
		return(0);
	}
	Sleep(30);
	return(1);
}

void InitStars()
{
	for (int i = 0; i < MAX_STARS; i++)
	{
		stars[i].x = rand()%640;
		stars[i].y = rand()%480;
		stars[i].color = rand()%4+1;
		stars[i].change = false;
		stars[i].oldx = stars[i].x;
		stars[i].oldy = stars[i].y;
	}
}

void ProcessStars()
{
	int i = 900;
	static int counter = 0, threshold = 0;

	if (++counter >= threshold)
	{
		threshold = rand()%15+5;
		counter = 0;
		i = rand()%50;
	}

	if (i >= 0 && i < 50)
	{
		stars[i].oldx = stars[i].x;
		stars[i].oldy = stars[i].y;
		stars[i].x = rand()%640;
		stars[i].y = rand()%480;
		stars[i].color = rand()%3+1;
		stars[i].change = true;
	}	
}

void DrawStars()
{
	for(int i = 0; i < MAX_STARS; i++)
	{
		if (stars[i].change)
		{
			PlotPixel(stars[i].oldx, stars[i].oldy, 0, (UCHAR *)ddsd.lpSurface);
			stars[i].change = false;
		}
		PlotPixel(stars[i].x, stars[i].y, stars[i].color, (UCHAR *)ddsd.lpSurface);
	}
}

void InitFireworks()
{
	for (int i = 0; i < MAX_FIREWORKS; i++)
	{
		fireworks[i].color = 8;
		fireworks[i].x = 0;
		fireworks[i].y = 0;
		fireworks[i].dest = 0;
		fireworks[i].state = 0;
		
		for (int j = 0; j < MAX_PARTICLES; j++)
		{
			fireworks[i].particles[j].x = 0;
			fireworks[i].particles[j].y = 0;
			fireworks[i].particles[j].xv = 0;
			fireworks[i].particles[j].yv = 0;
			fireworks[i].particles[j].active = false;
		}
	}
}

void ProcessFireworks()
{
	for (int i = 0; i < MAX_FIREWORKS; i++)
	{
		int x = fireworks[i].x;
		int y = fireworks[i].y;
		
		switch(fireworks[i].state)
		{
			
			case 1:
				if (++fireworks[i].counter >= fireworks[i].threshold)
				{
					fireworks[i].dest = rand()%200 + 80;
					fireworks[i].x = rand()%500 + 70;
					fireworks[i].y = 478;
					fireworks[i].state = 2;
					fireworks[i].active = true;
					PlaySound("firework.wav", g_hinstance, SND_ASYNC);
				}
				break;
			case 2:	
				fireworks[i].oldy = y;
				y -= 8;
				
				if (y <= fireworks[i].dest)
				{
					float size = (rand()%500 + 500) * 0.0004f;
					fireworks[i].state = 3;
					for (int j = 0; j < MAX_PARTICLES; j++)
					{
						fireworks[i].particles[j].x = (float)x;
						fireworks[i].particles[j].y = (float)y;
						fireworks[i].color = rand()%6 + 5;
						fireworks[i].particles[j].active = true;
						fireworks[i].particles[j].life = rand() % 50;
										
						int a = rand()%100;
						float speed = 0;
						if (a > 50)
							speed = (rand()%5 + 15) * size;
						else if(a > 20)
							speed = (rand()%5 + 10) * size;
						else
							speed = (rand()%8 + 2) * size;

						float dir = PI * 2 * j / MAX_PARTICLES;
						
						fireworks[i].particles[j].xv = cos(dir) * speed;
						fireworks[i].particles[j].yv = sin(dir) * speed;
						fireworks[i].active = false;
						fireworks[i].particles[j].df = float(rand()%3 + 7) * .03f;
						
					}
					PlaySound("explode.wav", g_hinstance, SND_ASYNC);
				}
				fireworks[i].x = x;
				fireworks[i].y = y;
				break;
			case 4:
				int count;
				count = MAX_PARTICLES;
				
				for (int j = 0; j < MAX_PARTICLES; j++)
				{
					
					if(fireworks[i].particles[j].active)
					{
						fireworks[i].particles[j].oldx = fireworks[i].particles[j].x;
						fireworks[i].particles[j].oldy = fireworks[i].particles[j].y;
						
						fireworks[i].particles[j].x += fireworks[i].particles[j].xv;
						fireworks[i].particles[j].y += fireworks[i].particles[j].yv;

						fireworks[i].particles[j].xv *= 0.92f;
						fireworks[i].particles[j].yv *= 0.92f;
						
						if (fireworks[i].particles[j].life > 90)
						{
							fireworks[i].particles[j].y += fireworks[i].particles[j].df;
							fireworks[i].particles[j].df *= 1.01f;
						}
						
						if (fireworks[i].particles[j].x < 0    || fireworks[i].particles[j].y < 0    || 
							fireworks[i].particles[j].x > 639 || fireworks[i].particles[j].y > 479  ||
							fireworks[i].particles[j].life > 170) 
						{
							fireworks[i].particles[j].active = false;
						}
						
						fireworks[i].particles[j].life++;	
					}
					else count--;
					
				}
				if (count <= 0) fireworks[i].state = 5;
				break;
			case 5:
				for (int j = 0; j < MAX_PARTICLES; j++)
				{
					fireworks[i].state = 0;
					fireworks[i].particles[j].x = 0;
					fireworks[i].particles[j].y = 0;
					fireworks[i].particles[j].xv = 0;
					fireworks[i].particles[j].yv = 0;
					fireworks[i].particles[j].active = false;
				}
				break;
			default: 
				fireworks[i].counter = 0;
				fireworks[i].threshold = rand() % 380 + 10;
				fireworks[i].state = 1;
				fireworks[i].color = 4;
				break;
		}
	}
}

void DrawFireworks()
{
	for (int i = 0; i < MAX_FIREWORKS; i++)
	{
		int x = fireworks[i].x;
		int y = fireworks[i].y;
		int oldy = fireworks[i].oldy;
		UCHAR color = rand()%5 + 6;
		switch(fireworks[i].state)
		{
			case 2:
			case 3:
				PlotPixel(x - 1, oldy, 0, (UCHAR *)ddsd.lpSurface);
				PlotPixel(x, oldy,    0, (UCHAR *)ddsd.lpSurface);
				PlotPixel(x + 1, oldy, 0, (UCHAR *)ddsd.lpSurface);
				PlotPixel(x - 1, oldy + 1, 0, (UCHAR *)ddsd.lpSurface);
				PlotPixel(x, oldy + 1, 0, (UCHAR *)ddsd.lpSurface);
				PlotPixel(x + 1, oldy + 1, 0, (UCHAR *)ddsd.lpSurface);
				if (fireworks[i].state == 2)
				{
					PlotPixel(x - 1, y, color, (UCHAR *)ddsd.lpSurface);
					PlotPixel(x, y, color, (UCHAR *)ddsd.lpSurface);
					PlotPixel(x + 1, y, color, (UCHAR *)ddsd.lpSurface);
					PlotPixel(x - 1, y + 1, color, (UCHAR *)ddsd.lpSurface);
					PlotPixel(x, y + 1, color, (UCHAR *)ddsd.lpSurface);
					PlotPixel(x + 1, y + 1, color, (UCHAR *)ddsd.lpSurface);
				}
				else
				{
					fireworks[i].state = 4;
				}
				break;
			case 4:
				for (int j = 0; j < MAX_PARTICLES; j++)
				{
					PlotPixel((int)fireworks[i].particles[j].oldx, (int)fireworks[i].particles[j].oldy, 0, (UCHAR *)ddsd.lpSurface);
					if(fireworks[i].particles[j].active)
					{
						if (fireworks[i].particles[j].life > 110 || fireworks[i].particles[j].life < 50)
						{
							PlotPixel((int)fireworks[i].particles[j].x, (int)fireworks[i].particles[j].y, fireworks[i].color + 6, (UCHAR *)ddsd.lpSurface);
						}
						else PlotPixel((int)fireworks[i].particles[j].x, (int)fireworks[i].particles[j].y, fireworks[i].color, (UCHAR *)ddsd.lpSurface);
					}
				}	
				break;
		}
	}
}

inline void SetPaletteColor(int index, UCHAR red, UCHAR green, UCHAR blue)
{
	palette[index].peRed = red;
	palette[index].peGreen = green;
	palette[index].peBlue = blue;
	palette[index].peFlags = PC_NOCOLLAPSE;
}

inline void PlotPixel(int x, int y, UCHAR color, UCHAR *vb)
{
	vb[x + y * (int)ddsd.lPitch] = color;
}