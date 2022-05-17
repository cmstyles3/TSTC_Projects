//Collin Munson
//Test 2

#define WIN32_LEAN_AND_MEAN
#define INITGUID

// Includes

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <mmsystem.h>
#include "ddraw.h"

// Defines

#define WINDOW_CLASS_NAME "TEST"

#define KEYDOWN(vk) (GetAsyncKeyState(vk) & 0x8000) ? 1 : 0
#define KEYUP(vk) (GetAsyncKeyState(vk) & 0x8000) ? 0 : 1

#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 32

#define _RGB16BIT555(r,g,b) ((b & 31) + ((g & 31) << 5) + ((r & 31) << 10))
#define _RGB16BIT565(r,g,b) ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))
#define _RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

#define BITMAP_ID 0x4D42
#define MAX_STARS 150
#define MAX_METEORS 10

typedef struct BITMAP_FILE_TAG
{
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;
	PALETTEENTRY palette[256];
	UCHAR *buffer;
}BITMAP_FILE, *BITMAP_FILE_PTR;

struct Star
{
	int x, y;
	int yv;
	DWORD color;
};

struct Meteor
{
	int x, y;
	int yv;
	int counter, threshold;
	int state;
};

// Function Prototypes

int LoadBitmapFile(BITMAP_FILE_PTR bitmap, char *filename);
int FlipBitmap(UCHAR *image, int bytes_per_line, int height);
int UnloadBitmapFile(BITMAP_FILE_PTR bitmap);

int GameMain();
int GameInit();
int GameKill();

int DDrawClearSurface(LPDIRECTDRAWSURFACE7 dest, int color);

void InitStars();
void SetStarDepth(int index);
void MoveStars();
void DrawStars();

void InitMeteors();
void MoveMeteors();
void DrawMeteors();
// Global variables

HWND g_hwnd = 0;
HINSTANCE g_hinstance = 0;

LPDIRECTDRAW7 lpdd7 = 0;
LPDIRECTDRAWSURFACE7 lpddsprimary = 0;
LPDIRECTDRAWSURFACE7 lpddsback = 0;
DDSURFACEDESC2 ddsd = {0};

LPDIRECTDRAWSURFACE7 spaceship, meteor;

int shipx = SCREEN_WIDTH/2 - 48 , shipy = SCREEN_HEIGHT - 96;

Star stars[MAX_STARS];
Meteor meteors[MAX_METEORS];
bool gameover = false;
int score;
int start_time;

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
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.lpszMenuName = NULL;
	wc.style = CS_DBLCLKS | CS_OWNDC | CS_VREDRAW | CS_HREDRAW;

	
	if(!RegisterClassEx(&wc))
		return(0);
	if(!(g_hwnd = CreateWindowEx(NULL, WINDOW_CLASS_NAME, "Test 2", WS_VISIBLE | WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hinstance, NULL)))
		return(0);

	ShowCursor(false);
	if (!GameInit())
		return(0);
	srand(GetTickCount());
	start_time = GetTickCount();
	PlaySound("techno.wav",NULL,SND_LOOP | SND_ASYNC);
	while(1)
	{
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

	if (FAILED(lpdd7->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, 0, 0)))
	{
		return(0);
	}

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwBackBufferCount = 1;
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;

	if (FAILED(lpdd7->CreateSurface(&ddsd, &lpddsprimary, NULL)))
	{
		return(0);
	}
	ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
	
	if (FAILED(lpddsprimary->GetAttachedSurface(&ddsd.ddsCaps, &lpddsback)))
		return(0);

	DDRAW_INIT_STRUCT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = 96;
	ddsd.dwHeight = 96;
	ddsd.ddckCKSrcBlt.dwColorSpaceLowValue = 0;
	ddsd.ddckCKSrcBlt.dwColorSpaceHighValue = 0;
	if (FAILED(lpdd7->CreateSurface(&ddsd, &spaceship, NULL)))
		return(0);
	BITMAP_FILE bitmap = {0};
	if (!LoadBitmapFile(&bitmap, "spaceship.bmp"))
		return(0);
	DWORD *src_ptr, *dest_ptr;
	spaceship->Lock(NULL, &ddsd, NULL, NULL);
	src_ptr = (DWORD *)bitmap.buffer;
	dest_ptr = (DWORD *)ddsd.lpSurface;
	for (int y = 0; y < 96; y++)
	{
		memcpy(dest_ptr,src_ptr,96 * 4);
		src_ptr += bitmap.bitmapinfoheader.biWidth;
		dest_ptr += ddsd.lPitch >> 2;
	}
	spaceship->Unlock(NULL);
	if (!UnloadBitmapFile(&bitmap))
		return(0);

	DDRAW_INIT_STRUCT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_CKSRCBLT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = 32;
	ddsd.dwHeight = 32;
	ddsd.ddckCKSrcBlt.dwColorSpaceLowValue = 0;
	ddsd.ddckCKSrcBlt.dwColorSpaceHighValue = 0;
	if (FAILED(lpdd7->CreateSurface(&ddsd, &meteor, NULL)))
		return(0);
	
	if (!LoadBitmapFile(&bitmap, "meteor.bmp"))
		return(0);
	src_ptr = 0;
	dest_ptr = 0;
	//DWORD *src_ptr, *dest_ptr;
	meteor->Lock(NULL, &ddsd, NULL, NULL);
	src_ptr = (DWORD *)bitmap.buffer;
	dest_ptr = (DWORD *)ddsd.lpSurface;
	for (int y = 0; y < 32; y++)
	{
		memcpy(dest_ptr,src_ptr,32 * 4);
		src_ptr += bitmap.bitmapinfoheader.biWidth;
		dest_ptr += ddsd.lPitch >> 2;
	}
	meteor->Unlock(NULL);
	if (!UnloadBitmapFile(&bitmap))
		return(0);

	DDrawClearSurface(lpddsprimary, 0);
	DDrawClearSurface(lpddsback, 0);
	
	InitStars();
	InitMeteors();
	return(1);
}

int GameKill()
{
	if (lpddsback)
	{
		lpddsback->Release();
		lpddsback = NULL;
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
	HDC hdc;
	char buffer[80];
	if (KEYDOWN(VK_ESCAPE))
		SendMessage(g_hwnd, WM_DESTROY, 0, 0);
	if (!gameover)
	{
		if (KEYDOWN(VK_UP))
		{
			shipy -= 4;
		}
		if (KEYDOWN(VK_DOWN))
		{
			shipy += 4;
		}
		if (KEYDOWN(VK_LEFT))
		{
			shipx -= 4;
		}
		if (KEYDOWN(VK_RIGHT))
		{
			shipx += 4;
		}
		if (shipy < 0) 
		{
			gameover = true;
			score = (GetTickCount() - start_time) / 1000;
		}
		else if (shipy > SCREEN_HEIGHT - 96) shipy = SCREEN_HEIGHT - 96;
		if (shipx < 0) shipx = 0;
		else if (shipx > SCREEN_WIDTH - 96) shipx = SCREEN_WIDTH - 96;
	}
	MoveStars();
	MoveMeteors();
	DDRAW_INIT_STRUCT(ddsd);
	DDrawClearSurface(lpddsback, 0);
	
		
	if (FAILED(lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)))
	{
		return(0);
	}
	DrawStars();
	if (FAILED(lpddsback->Unlock(NULL)))
	{
		return(0);
	}
	DrawMeteors();
	if (!gameover)
	{
		
		RECT dest_rect, src_rect;
		src_rect.left = 0;
		src_rect.top = 0;
		src_rect.right = 95;
		src_rect.bottom = 95;

		dest_rect.left = shipx;
		dest_rect.top = shipy;
		dest_rect.right = shipx + 95;
		dest_rect.bottom = shipy + 95;

		lpddsback->Blt(&dest_rect, spaceship, &src_rect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
	}
	else
	{
		hdc = GetDC(g_hwnd);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255,0,0));
		sprintf_s(buffer, sizeof(buffer), "Your score was %d seconds - Game Over!!", score);
		TextOut(hdc,SCREEN_WIDTH/2 - 8 * (strlen(buffer)/2),SCREEN_HEIGHT/2 - 8,buffer,strlen(buffer));
		ReleaseDC(g_hwnd, hdc);
	}
	
	
	FAILED(lpddsprimary->Flip(NULL, DDFLIP_WAIT));
	
	return(1);
}

int LoadBitmapFile(BITMAP_FILE_PTR bitmap, char *filename)
{
	HANDLE hFile;
	int index;
	DWORD nBytesRead;
	UCHAR *temp_buffer;

	hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return(0);
	}

	ReadFile(hFile, &bitmap->bitmapfileheader, sizeof(BITMAPFILEHEADER), &nBytesRead, NULL);
	
	if(bitmap->bitmapfileheader.bfType != BITMAP_ID)
	{
		CloseHandle(hFile);
		return(0);
	}
	
	ReadFile(hFile, &bitmap->bitmapinfoheader, sizeof(BITMAPINFOHEADER), &nBytesRead, NULL);

	if (bitmap->bitmapinfoheader.biBitCount == 8)
	{
		ReadFile(hFile, &bitmap->palette, 256 * sizeof(PALETTEENTRY), &nBytesRead, NULL);
		for(int i = 0; i < 256; i++)
		{
			int temp_color = bitmap->palette[i].peRed;
			bitmap->palette[i].peRed = bitmap->palette[i].peBlue;
			bitmap->palette[i].peBlue = temp_color;
			bitmap->palette[i].peFlags = PC_NOCOLLAPSE;
		}
	}

	SetFilePointer(hFile, -(int)(bitmap->bitmapinfoheader.biSize), NULL, FILE_BEGIN);

	if (bitmap->bitmapinfoheader.biBitCount == 8 || 
		bitmap->bitmapinfoheader.biBitCount == 16) 
	{
		if (bitmap->buffer)
			free(bitmap->buffer);
		
		if (!(bitmap->buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			CloseHandle(hFile);
			return(0);
		}

		ReadFile(hFile, bitmap->buffer, bitmap->bitmapinfoheader.biSizeImage, &nBytesRead, NULL);
	}
	else if (bitmap->bitmapinfoheader.biBitCount == 24)
	{
		if (bitmap->buffer)
			free(bitmap->buffer);
		
		if (!(temp_buffer = (UCHAR *)malloc(bitmap->bitmapinfoheader.biSizeImage)))
		{
			CloseHandle(hFile);
			return(0);
		}
		
		
		if (!(bitmap->buffer=(UCHAR *)malloc(4 * bitmap->bitmapinfoheader.biWidth * bitmap->bitmapinfoheader.biHeight)))
		{
			CloseHandle(hFile);
			free(temp_buffer);
			return(0);
		} 
		ReadFile(hFile, temp_buffer, bitmap->bitmapinfoheader.biSizeImage, &nBytesRead, NULL);
		for (index=0; index < bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight; index++)
		{
		   DWORD color;
		   UCHAR blue  = (temp_buffer[index*3 + 0]),
				 green = (temp_buffer[index*3 + 1]),
				 red   = (temp_buffer[index*3 + 2]);

           
		   color = _RGB32BIT(0,red,green,blue);
		   ((DWORD *)bitmap->buffer)[index] = color;
		}
		bitmap->bitmapinfoheader.biBitCount = 32;
		free(temp_buffer);
	}
	else
	{
		return(0);
	}

	CloseHandle(hFile);

	FlipBitmap(bitmap->buffer, bitmap->bitmapinfoheader.biWidth * (bitmap->bitmapinfoheader.biBitCount/8), bitmap->bitmapinfoheader.biHeight);
	return(1);
}

int UnloadBitmapFile(BITMAP_FILE_PTR bitmap)
{
	if (bitmap->buffer)
	{
		free(bitmap->buffer);
		bitmap->buffer = NULL;
	}
	return(1);
}

int FlipBitmap(UCHAR *image, int bytes_per_line, int height)
{
	UCHAR *buffer;
	
	if (!(buffer = (UCHAR *)malloc(bytes_per_line * height)))
		return(0);

	memcpy(buffer, image, bytes_per_line * height);

	for (int i = 0; i < height; i++)
		memcpy(&image[((height - 1) - i) * bytes_per_line], 
		&buffer[bytes_per_line * i], bytes_per_line);

	free(buffer);
	return(1);
}

void InitStars()
{
	int depth;
	for (int i = 0; i < MAX_STARS; i++)
	{
		stars[i].x = rand()%SCREEN_WIDTH;
		stars[i].y = rand()%SCREEN_HEIGHT;
		
		SetStarDepth(i);
	}
}

void SetStarDepth(int index)
{
	switch(rand()%3)
		{
		case 0:
			stars[index].yv = 3;
			stars[index].color = _RGB32BIT(0,150,150,150);
			break;
		case 1:
			stars[index].yv = 4;
			stars[index].color = _RGB32BIT(0,200,200,200);
			break;
		case 2:
			stars[index].yv = 6;
			stars[index].color = _RGB32BIT(0,255,255,255);
			break;

		}
}

void MoveStars()
{
	for (int i = 0; i < MAX_STARS; i++)
	{
		stars[i].y += stars[i].yv;
		if (stars[i].y >= SCREEN_HEIGHT)
		{
			stars[i].x = rand()%SCREEN_WIDTH;
			stars[i].y = 0;
			SetStarDepth(i);
		}
	}
}

void DrawStars()
{
	DWORD *dest = (DWORD *)ddsd.lpSurface;
	for (int i = 0; i < MAX_STARS; i++)
	{
		dest[stars[i].x + stars[i].y * (ddsd.lPitch >> 2)] = stars[i].color;
	}
}

void InitMeteors()
{
	for (int i = 0; i < MAX_METEORS; i++)
	{
		meteors[i].x = 0;
		meteors[i].y = 0;
		meteors[i].yv = 0;
		meteors[i].counter = 0;
		meteors[i].threshold = 0;
		meteors[i].state = 0;
	}
}

void MoveMeteors()
{
	for (int i = 0; i < MAX_METEORS; i++)
	{
		switch(meteors[i].state)
		{
			case 0:
				meteors[i].counter = 0;
				meteors[i].threshold = rand()%60 + 50;
				meteors[i].state = 1;
				break;
			case 1:
				if (++meteors[i].counter >= meteors[i].threshold)
				{
					meteors[i].x = rand()%(SCREEN_WIDTH - 32);
					meteors[i].y = 0;
					meteors[i].yv = rand()%4+5;
					meteors[i].state = 2;
				}
				break;
			case 2:
				meteors[i].y += meteors[i].yv;

				//USING MID POINT ON METEOR FOR COLLISION DETECTION
				int x = meteors[i].x/2;
				int y = meteors[i].y/2;

				if ((x >= shipx && x < shipx + 96) && 
					(y >= shipy && y < shipy + 96))
				{
					shipx = SCREEN_WIDTH/2 - 48; // RESET SHIPS POSITION
					shipy = SCREEN_HEIGHT - 96;
				}
				if (meteors[i].y > SCREEN_HEIGHT - 32)
					meteors[i].state = 0;
				break;
		}
	}
}

void DrawMeteors()
{
	for (int i = 0; i < MAX_METEORS; i++)
	{
		switch(meteors[i].state)
		{
		case 2:
			RECT dest_rect, src_rect;
		src_rect.left = 0;
		src_rect.top = 0;
		src_rect.right = 31;
		src_rect.bottom = 31;

		dest_rect.left = meteors[i].x;
		dest_rect.top = meteors[i].y;
		dest_rect.right = meteors[i].x + 31;
		dest_rect.bottom = meteors[i].y + 31;

		lpddsback->Blt(&dest_rect, meteor, &src_rect, DDBLT_WAIT | DDBLT_KEYSRC, NULL);
			break;
		}
	}
}

int DDrawClearSurface(LPDIRECTDRAWSURFACE7 dest, int color)
{
	DDBLTFX ddbltfx; 

	DDRAW_INIT_STRUCT(ddbltfx);

	ddbltfx.dwFillColor = color;


	dest->Blt(NULL,     
           NULL,       
           NULL,       
           DDBLT_COLORFILL | DDBLT_WAIT,   
           &ddbltfx);
	return(1);
}