//Collin Munson
//7/23/2007
//Program Description: snow falling and accumalating on house, trees and ground

#define WIN32_LEAN_AND_MEAN
#define INITGUID

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include "ddraw.h"

#define CLASS_NAME "LAB07"

#define KEYDOWN(vk) (GetAsyncKeyState(vk) & 0x8000) ? 1 : 0
#define KEYUP(vk) (GetAsyncKeyState(vk) & 0x8000) ? 0 : 1

#define _RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

#define BITMAP_ID 0x4D42

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 32
#define MAX_SNOW 750

typedef struct BITMAP_FILE_TAG
{
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;
	PALETTEENTRY palette[256];
	UCHAR *buffer;
}BITMAP_FILE, *BITMAP_FILE_PTR;

struct Snow
{
	float x, y;
	float xv, yv;
	int state;
	int counter, threshold;
};

int LoadBitmapFile(BITMAP_FILE_PTR bitmap, char *filename);
int FlipBitmap(UCHAR *image, int bytes_per_line, int height);
int UnloadBitmapFile(BITMAP_FILE_PTR bitmap);

void DrawBitmap32(BITMAP_FILE_PTR bitmap, int x, int y);

inline void PlotPixel32(int x, int y, DWORD color, DWORD *vb, int mem_pitch);

int DDrawFillSurface(LPDIRECTDRAWSURFACE7 lpdds, int color); 

int GameMain();
int GameInit();
int GameKill();

void InitSnow();
void MoveSnow();
void ResetSnowFlake(int index);
void DrawSnow();

HWND g_hwnd = 0;
HINSTANCE g_hinstance = 0;

LPDIRECTDRAW7 lpdd7 = 0;
LPDIRECTDRAWSURFACE7 lpddsprimary = 0;
LPDIRECTDRAWSURFACE7 lpddsback = 0;
DDSURFACEDESC2 ddsd = {0};

BITMAP_FILE house ={0}, tree = {0}, mask = {0}, stuck_snow = {0};

Snow snow[MAX_SNOW] = {0};

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
	if(!(g_hwnd = CreateWindowEx(NULL, CLASS_NAME, "Snow", WS_VISIBLE | WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hinstance, NULL)))
		return(0);
	ShowCursor(false);
	srand((int)GetTickCount());
	if (!GameInit())
		return(0);
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

	if (!LoadBitmapFile(&house, "bitmaps\\house.bmp"))
		return(0);
	if (!LoadBitmapFile(&tree, "bitmaps\\tree.bmp"))
		return(0);
	if (!LoadBitmapFile(&mask, "bitmaps\\mask.bmp"))
		return(0);
	if (!LoadBitmapFile(&stuck_snow, "bitmaps\\blank.bmp"))
		return(0);

	InitSnow();

	return(1);
}

int GameKill()
{
	UnloadBitmapFile(&stuck_snow);
	UnloadBitmapFile(&mask);
	UnloadBitmapFile(&tree);
	UnloadBitmapFile(&house);
	
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
	if (KEYDOWN(VK_ESCAPE))
		SendMessage(g_hwnd, WM_DESTROY, 0, 0);
	MoveSnow();
	DDRAW_INIT_STRUCT(ddsd);
	DDrawFillSurface(lpddsback, _RGB32BIT(0, 0, 0, 0));
	if (FAILED(lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)))
	{
		return(0);
	}
	
	DrawBitmap32(&house, 0, 224);
	DrawBitmap32(&tree, 2, 275);
	DrawBitmap32(&tree, 514, 275);
	DrawBitmap32(&stuck_snow, 0, 0);
	DrawSnow();
	
	if (FAILED(lpddsback->Unlock(NULL)))
	{
		return(0);
	}
	lpddsprimary->Flip(NULL, DDFLIP_WAIT);
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

int DDrawFillSurface(LPDIRECTDRAWSURFACE7 lpdds, int color)
{
	DDBLTFX ddbltfx;
	memset(&ddbltfx, 0, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = color;
	lpdds->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
	return(1);
}

void DrawBitmap32(BITMAP_FILE_PTR bitmap, int x, int y)
{
	if (x + bitmap->bitmapinfoheader.biWidth < 0 || x >= SCREEN_WIDTH || 
		y + bitmap->bitmapinfoheader.biHeight < 0 || y >= SCREEN_HEIGHT)
		return;

	DWORD *dest_ptr = (DWORD *)ddsd.lpSurface + y * (ddsd.lPitch >> 2) + x;
	DWORD *src_ptr  = (DWORD *)bitmap->buffer;

	DWORD pixel;
	for (int index_y = 0; index_y < bitmap->bitmapinfoheader.biHeight; index_y++)
    {
		for (int index_x = 0; index_x < bitmap->bitmapinfoheader.biWidth; index_x++)
		{
			int X = x + index_x;
			int Y = y + index_y;
			if ((pixel = src_ptr[index_x]))
			   if (Y >= 0 && Y < SCREEN_HEIGHT && X >= 0 && X < SCREEN_WIDTH)
					dest_ptr[index_x] = pixel;
		 
		}
		dest_ptr   += ddsd.lPitch >> 2;
		src_ptr += bitmap->bitmapinfoheader.biWidth;
   }
}

inline void PlotPixel32(int x, int y, DWORD color, DWORD *vb, int lpitch32)
{
	vb[x + y * lpitch32] = color;
}

void InitSnow()
{
	for(int i = 0; i < MAX_SNOW; i++)
	{
		snow[i].x = float (rand() % SCREEN_WIDTH);
		snow[i].y = float (rand() % SCREEN_HEIGHT);
		snow[i].xv = (rand()%5 - 2) * 0.2f;
		snow[i].yv = (rand() % 50 + 90) * 0.03f;
		snow[i].counter = 0;
		snow[i].threshold = rand() % 40 + 20;
	}
}

void MoveSnow()
{
	
	DWORD *src_ptr = (DWORD *)mask.buffer;
	DWORD *dest_ptr = (DWORD *)stuck_snow.buffer;
	for(int i = 0; i < MAX_SNOW; i++)
	{
		int x = int (snow[i].x += snow[i].xv);
		int y = int (snow[i].y += snow[i].yv);

		if (y >= SCREEN_HEIGHT)
			ResetSnowFlake(i);
		
		if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
		{
			if (src_ptr[x + y * (mask.bitmapinfoheader.biWidth)] == _RGB32BIT(0,0,0,255) && (rand() % 10) < 1)
			{
				dest_ptr[x + y * (stuck_snow.bitmapinfoheader.biWidth)] = _RGB32BIT(0,255,255,255);
				ResetSnowFlake(i);
			}
		}

		if (++snow[i].counter >= snow[i].threshold)
		{
			snow[i].xv = (rand()%5-2) * 0.2f;
			snow[i].counter = 0;
			snow[i].threshold = rand() % 40 + 20;
		}
	}
}

void ResetSnowFlake(int index)
{
	snow[index].x = float(rand() % SCREEN_WIDTH);
	snow[index].y = 0;
	snow[index].xv = (rand()%5 - 2) * 0.2f;
	snow[index].yv = (rand() % 50 + 90) * 0.03f;
	snow[index].counter = 0;
	snow[index].threshold = rand() % 40 + 20;
}

void DrawSnow()
{
	for(int i = 0; i < MAX_SNOW; i++)
	{
		int x = int (snow[i].x);
		int y = int (snow[i].y);
		if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
			PlotPixel32(x, y, _RGB32BIT(0,255,255,255),(DWORD *)ddsd.lpSurface,(ddsd.lPitch >> 2));
	}
}