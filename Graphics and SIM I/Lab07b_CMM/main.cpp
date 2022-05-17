//Collin Munson
//Graphics and Simulation I
//Maze with user controlled dude being animated

#define WIN32_LEAN_AND_MEAN
#define INITGUID

// Includes

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "ddraw.h"

// Defines

#define WINDOW_CLASS_NAME "LAB07B"

#define KEYDOWN(vk) (GetAsyncKeyState(vk) & 0x8000) ? 1 : 0
#define KEYUP(vk) (GetAsyncKeyState(vk) & 0x8000) ? 0 : 1

#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 32
#define MAX_FRAMES 64

#define WALL 0
#define START 1
#define END 2
#define PATH 3

#define _RGB16BIT555(r,g,b) ((b & 31) + ((g & 31) << 5) + ((r & 31) << 10))
#define _RGB16BIT565(r,g,b) ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))
#define _RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

#define BITMAP_ID 0x4D42


typedef struct BITMAP_FILE_TAG
{
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;
	PALETTEENTRY palette[256];
	UCHAR *buffer;
}BITMAP_FILE, *BITMAP_FILE_PTR;

typedef struct BOB_TYP
{
	int x, y;
	int xv, yv;
	int width, height;
	int curr_frame;
	int num_frames;
	int counter, threshold;
	LPDIRECTDRAWSURFACE7 images[MAX_FRAMES];
}BOB, *BOB_PTR;

// Function Prototypes

int LoadBitmapFile(BITMAP_FILE_PTR bitmap, char *filename);
int FlipBitmap(UCHAR *image, int bytes_per_line, int height);
int UnloadBitmapFile(BITMAP_FILE_PTR bitmap);

int DDrawClearSurface(LPDIRECTDRAWSURFACE7, int color);

int CreateBOB(BOB_PTR bob, int x, int y, int width, int height, int num_frames, int mem_flags = 0);
int DestroyBOB(BOB_PTR bob);
int LoadFrameBOB(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy);
int LoadFrameBOB16(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy);
int LoadFrameBOB32(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy);
int DrawBOB32(BOB_PTR bob, LPDIRECTDRAWSURFACE7 dest);

LPDIRECTDRAWSURFACE7 DDrawCreateSurface(int width, int height, int mem_flags);
int DrawBitmap(LPDIRECTDRAWSURFACE7 image, int x, int y);

int GameMain();
int GameInit();
int GameKill();


// Global variables

HWND g_hwnd = 0;
HINSTANCE g_hinstance = 0;

LPDIRECTDRAW7 lpdd7 = 0;
LPDIRECTDRAWSURFACE7 lpddsprimary = 0;
LPDIRECTDRAWSURFACE7 lpddsback = 0;
DDSURFACEDESC2 ddsd = {0};
LPDIRECTDRAWPALETTE lpddpal = 0;


BOB maze, dude;
int maze_id[300];
					

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
	if(!(g_hwnd = CreateWindowEx(NULL, WINDOW_CLASS_NAME, "Maze", WS_VISIBLE | WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hinstance, NULL)))
		return(0);

	ShowCursor(false);
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
	
	BITMAP_FILE bitmap = {0};

	if (!LoadBitmapFile(&bitmap, "maze_id.bmp"))
		return(0);
	DWORD *src_ptr = (DWORD *)bitmap.buffer;
	int start_x = 0, start_y = 0;
	for (int y = 0; y < 15; y++)
	{
		for(int x = 0; x < 20; x++)
		{
			int index = x + y * 20;
			if (src_ptr[index] == _RGB32BIT(0,0,0,255))
				maze_id[index] = WALL;
			else if (src_ptr[index] == _RGB32BIT(0,0,255,0))
			{
				maze_id[index] = START;
				start_x = x << 5, start_y = y << 5;
			}
			else if (src_ptr[index] == _RGB32BIT(0,255,0,0))
				maze_id[index] = END;
			else maze_id[index] = PATH;
		}
	}
	if(!UnloadBitmapFile(&bitmap))
		return(0);

	if (!LoadBitmapFile(&bitmap, "maze.bmp"))
		return(0);
	
	CreateBOB(&maze, 0, 0, 640, 480, 1);
	LoadFrameBOB32(&maze, &bitmap, 0, 0, 0);
	
	UnloadBitmapFile(&bitmap);
	LoadBitmapFile(&bitmap, "dude.bmp");
	CreateBOB(&dude, start_x, start_y, 32, 32, 12);
	for (int i = 0; i < 12; i++)
		LoadFrameBOB32(&dude, &bitmap, i, i * 32, 0);
	
	UnloadBitmapFile(&bitmap);
	dude.xv=1;
	return(1);
}

int GameKill()
{
	DestroyBOB(&dude);
	DestroyBOB(&maze);
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
	static int direction = 1;
	bool moving = false;
	//dude.xv = dude.yv = 0;
	if (KEYDOWN(VK_LEFT))
	{
		dude.xv = -1;
		dude.yv = 0;
		direction = 0;
		
	}
	if (KEYDOWN(VK_RIGHT))
	{
		dude.xv = 1;
		dude.yv = 0;
		direction = 1;
		
	}
	if (KEYDOWN(VK_UP))
	{
		dude.xv = 0;
		dude.yv = -1;
		direction = 2;
		
	}
	if (KEYDOWN(VK_DOWN))
	{
		dude.xv = 0;
		dude.yv = 1;
		direction = 3;
		
	}

	if (dude.xv || dude.yv) 
		moving = true;

	dude.x += dude.xv;
	dude.y += dude.yv;
	int x = (dude.x + 10) >> 5;
	int y = (dude.y + 22) >> 5;
	int x2 = (dude.x + 22) >> 5;
	int y2 = (dude.y + 30) >> 5;
	if (maze_id[x + y * 20] == WALL || maze_id[x2 + y * 20] == WALL ||
		maze_id[x + y2 * 20] == WALL || maze_id[x2 + y2 * 20] == WALL)
	{
		dude.x -= dude.xv; 
		dude.y -= dude.yv;
		moving = false;
		
	}
	if (moving) PlaySound("walking.wav", NULL, SND_LOOP | SND_ASYNC | SND_NOSTOP);
	else PlaySound(NULL, NULL, NULL);

	if (++dude.counter >= dude.threshold)
	{
		dude.counter = 0;
		dude.threshold = 20;
	
		dude.curr_frame++;
	}

	if (direction == 3)
	{
		if (moving)
		{
			if (dude.curr_frame >= 3) dude.curr_frame = 1;
		}
		else dude.curr_frame = 0;
	}
	else if (direction == 2)
	{
		if (moving)
		{
			if (dude.curr_frame >= 6 || dude.curr_frame < 3) dude.curr_frame = 4;
		}
		else dude.curr_frame = 3;
		
	}
	else if (direction == 1)
	{	
		if (moving)
		{
			if (dude.curr_frame >= 9 || dude.curr_frame < 6) dude.curr_frame = 7;
		}
		else dude.curr_frame = 6;
	}
	else if (direction == 0)
	{
		if (moving)
		{
			if (dude.curr_frame >= 12 || dude.curr_frame < 9) dude.curr_frame = 10;
		}
		else dude.curr_frame = 9;
	}
	DDRAW_INIT_STRUCT(ddsd);

	DDrawClearSurface(lpddsback, 0);

	/*if (FAILED(lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)))
	{

		return(0);
	}

	if (FAILED(lpddsback->Unlock(NULL)))
	{

		return(0);
	}*/
	DrawBOB32(&maze, lpddsback);
	DrawBOB32(&dude, lpddsback);
	
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
		if (SCREEN_BPP == 16)
		{
			if (!(bitmap->buffer=(UCHAR *)malloc(2 * bitmap->bitmapinfoheader.biWidth * bitmap->bitmapinfoheader.biHeight)))
			{
				CloseHandle(hFile);
				free(temp_buffer);
				return(0);
			} 
			ReadFile(hFile, temp_buffer, bitmap->bitmapinfoheader.biSizeImage, &nBytesRead, NULL);
			for (index=0; index < bitmap->bitmapinfoheader.biWidth*bitmap->bitmapinfoheader.biHeight; index++)
			{
			   USHORT color;
			   UCHAR blue  = (temp_buffer[index*3 + 0]) >> 3,
					 green = (temp_buffer[index*3 + 1]) >> 2,
					 red   = (temp_buffer[index*3 + 2]) >> 3;

	           
			   color = _RGB16BIT565(red,green,blue);
			   ((USHORT *)bitmap->buffer)[index] = color;
			}
			bitmap->bitmapinfoheader.biBitCount = 16;
		}
		else
		{
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
		}
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

int DDrawClearSurface(LPDIRECTDRAWSURFACE7 lpdds, int color)
{
	DDBLTFX ddbltfx; 

	DDRAW_INIT_STRUCT(ddbltfx);

	ddbltfx.dwFillColor = color;


	lpdds->Blt(NULL,     
           NULL,       
           NULL,       
           DDBLT_COLORFILL | DDBLT_WAIT, 
           &ddbltfx);
	return(1);
}

LPDIRECTDRAWSURFACE7 DDrawCreateSurface(int width, int height, int mem_flags)
{
	DDSURFACEDESC2 ddsd;
	LPDIRECTDRAWSURFACE7 lpdds;
	DDRAW_INIT_STRUCT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;
	if (FAILED(lpdd7->CreateSurface(&ddsd,&lpdds,NULL)))
		return(0);
	DDCOLORKEY color_key;
	color_key.dwColorSpaceLowValue = 0;
	color_key.dwColorSpaceHighValue = 0;
	lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);
	return(lpdds);
}

int CreateBOB(BOB_PTR bob, int x, int y, int width, int height, int num_frames, int mem_flags)
{
	DDSURFACEDESC2 ddsd;

	bob->x = x;
	bob->y = y;
	bob->xv = 0;
	bob->yv = 0;
	bob->width = width;
	bob->height = height;
	bob->curr_frame = 0;
	bob->num_frames = num_frames;
	bob->counter = 0;
	bob->threshold = 12;

	for (int i = 0; i < bob->num_frames; i++)
	{
		DDRAW_INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.dwWidth = width;
		ddsd.dwHeight = height;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | mem_flags;

		if (FAILED(lpdd7->CreateSurface(&ddsd,&bob->images[i],NULL)))
			return(0);

		DDCOLORKEY color_key;
		color_key.dwColorSpaceLowValue = 0;
		color_key.dwColorSpaceHighValue = 0;
		bob->images[i]->SetColorKey(DDCKEY_SRCBLT, &color_key);
	}
	return(1);
}

int LoadFrameBOB(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy)
{
	DDSURFACEDESC2 ddsd;
	UCHAR *dest_ptr,
		*src_ptr;
	DDRAW_INIT_STRUCT(ddsd);
	src_ptr = (UCHAR *)bitmap->buffer + cx + cy * bitmap->bitmapinfoheader.biWidth;
	
	if (FAILED(bob->images[frame]->Lock(NULL, &ddsd, NULL, NULL)))
		return(0);
	dest_ptr = (UCHAR *)ddsd.lpSurface;
	for (int y = 0; y < bob->height; y++)
	{
		memcpy(dest_ptr, src_ptr, bob->width);
		src_ptr += bitmap->bitmapinfoheader.biWidth;
		dest_ptr += ddsd.lPitch;
	}

	if (FAILED(bob->images[frame]->Unlock(NULL)))
		return(0);
	return(1);
}

int LoadFrameBOB16(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy)
{
	DDSURFACEDESC2 ddsd;
	USHORT *dest_ptr,
		*src_ptr;
	DDRAW_INIT_STRUCT(ddsd);
	src_ptr = (USHORT *)bitmap->buffer + cx + cy * bitmap->bitmapinfoheader.biWidth;
	
	if (FAILED(bob->images[frame]->Lock(NULL, &ddsd, NULL, NULL)))
		return(0);
	dest_ptr = (USHORT *)ddsd.lpSurface;
	for (int y = 0; y < bob->height; y++)
	{
		memcpy(dest_ptr, src_ptr, bob->width * 2);
		src_ptr += bitmap->bitmapinfoheader.biWidth;
		dest_ptr += ddsd.lPitch >> 1;
	}

	if (FAILED(bob->images[frame]->Unlock(NULL)))
		return(0);
	return(1);
}

int LoadFrameBOB32(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy)
{
	DDSURFACEDESC2 ddsd;
	DWORD *dest_ptr,
		*src_ptr;
	DDRAW_INIT_STRUCT(ddsd);
	src_ptr = (DWORD *)bitmap->buffer + cx + cy * bitmap->bitmapinfoheader.biWidth;
	
	if (FAILED(bob->images[frame]->Lock(NULL, &ddsd, NULL, NULL)))
		return(0);
	dest_ptr = (DWORD *)ddsd.lpSurface;
	for (int y = 0; y < bob->height; y++)
	{
		memcpy(dest_ptr, src_ptr, bob->width * 4);
		src_ptr += bitmap->bitmapinfoheader.biWidth;
		dest_ptr += ddsd.lPitch >> 2;
	}

	if (FAILED(bob->images[frame]->Unlock(NULL)))
		return(0);
	return(1);
}

int DestroyBOB(BOB_PTR bob)
{
	if (!bob)
		return(0);
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		if (bob->images[i])	
		{
			bob->images[i]->Release();
			bob->images[i] = 0;
		}
	}
	return(1);
}

int DrawBOB32(BOB_PTR bob, LPDIRECTDRAWSURFACE7 dest)
{
	RECT dest_rect, src_rect;
	dest_rect.left = bob->x;
	dest_rect.top = bob->y;
	dest_rect.right = bob->x + bob->width - 1;
	dest_rect.bottom = bob->y + bob->height - 1;

	src_rect.left = 0;
	src_rect.top = 0;
	src_rect.right = bob->width - 1;
	src_rect.bottom = bob->height - 1;

	if (FAILED(dest->Blt(&dest_rect, bob->images[bob->curr_frame], &src_rect, (DDBLT_WAIT | DDBLT_KEYSRC), NULL)))
		return(0);
	return(1);
}