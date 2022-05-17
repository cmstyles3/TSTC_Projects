//Collin Munson && Elizabeth Hawes
//Graphics and Simulation II
//"DigDug with Avengence"
//

#define WIN32_LEAN_AND_MEAN
#define INITGUID

// Includes

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <mmsystem.h>
#include "ddraw.h"
#include "resource.h"

// Defines

#define WINDOW_CLASS_NAME "DIGDUG"

#define KEYDOWN(vk) (GetAsyncKeyState(vk) & 0x8000) ? 1 : 0
#define KEYUP(vk) (GetAsyncKeyState(vk) & 0x8000) ? 0 : 1

#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SCREEN_BPP 32

#define MAX_FRAMES 64

#define	NONE 0x00
#define TOP_LEFT 0x01
#define TOP_RIGHT 0x02
#define BOTTOM_LEFT 0x04
#define BOTTOM_RIGHT 0x08

#define ALL (TOP_LEFT | BOTTOM_LEFT | TOP_RIGHT | BOTTOM_RIGHT) //0x0F

#define LEFT (TOP_LEFT | BOTTOM_LEFT)  // 0x05
#define RIGHT (TOP_RIGHT | BOTTOM_RIGHT) // 0x0A
#define TOP (TOP_LEFT | TOP_RIGHT) //0x03
#define BOTTOM (BOTTOM_LEFT | BOTTOM_RIGHT) //0x0C

#define NO_ENEMY 0
#define NO_ITEM 0

#define X_OFFSET 176
#define Y_OFFSET 56

#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

#define _RGB16BIT555(r,g,b) ((b & 31) + ((g & 31) << 5) + ((r & 31) << 10))
#define _RGB16BIT565(r,g,b) ((b & 31) + ((g & 63) << 5) + ((r & 31) << 11))
#define _RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))

#define BITMAP_ID 0x4D42

#define MAX_CELL_X 14
#define MAX_CELL_Y 16

#define MAX_ENEMIES 16
#define ENEMY_ALIVE 2
#define ENEMY_DYING 1
#define ENEMY_DEAD 0

enum GAME_STATE{MENU, INITGAME, PLAYGAME, INSTRUCTIONS, EXIT};
GAME_STATE game_state;
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

struct 
{
	float x, y;
	float xv, yv;
	int state;
	int lives;
	int score;
	bool moved;
	bool shoot;
}player;

struct
{
	float x, y;
	float xv, yv;
	int curr_frame;
	
	int state;
	int counter, threshold;
	bool moved;
	bool newdirection;
}enemies[MAX_ENEMIES];

struct
{
	int tile_id;
	int enemy_id;
	int item_id;
}cell[MAX_CELL_X][MAX_CELL_Y];

int LoadBitmapFile(BITMAP_FILE_PTR bitmap, char *filename);
int FlipBitmap(UCHAR *image, int bytes_per_line, int height);
int UnloadBitmapFile(BITMAP_FILE_PTR bitmap);

int DDrawClearSurface(LPDIRECTDRAWSURFACE7, int color);

int CreateBOB(BOB_PTR bob, int x, int y, int width, int height, int num_frames, int mem_flags = 0);
int DestroyBOB(BOB_PTR bob);
int LoadFrameBOB(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy);

int LoadFrameBOB32(BOB_PTR bob, BITMAP_FILE_PTR bitmap, int frame, int cx, int cy);
int DrawBOB32(BOB_PTR bob, LPDIRECTDRAWSURFACE7 dest);

LPDIRECTDRAWSURFACE7 DDrawCreateSurface(int width, int height, int mem_flags);
int DrawBitmap(LPDIRECTDRAWSURFACE7 image, int x, int y);

// Function Prototypes


int GameMain();
int GameInit();
int GameKill();

void InitCells();
void DrawCells();

void InitEnemies();
void MoveEnemies();
void DrawEnemies();

void Printf(int x,int y,const char *fmt,...);
// Global variables
HWND g_hwnd = 0;
HINSTANCE g_hinstance = 0;

LPDIRECTDRAW7 lpdd7 = 0;
LPDIRECTDRAWSURFACE7 lpddsprimary = 0;
LPDIRECTDRAWSURFACE7 lpddsback = 0;
DDSURFACEDESC2 ddsd = {0};
LPDIRECTDRAWPALETTE lpddpal = 0;

enum FILE_ID{MENU_BMP, SHOTGUN_BMP, DUDE_BMP, LEVEL1_4_BMP};

BOB menu, dude, tile, back, menu_item_playgame, menu_item_instructions, menu_item_exit, numbers, windup, windup_death, instructions, chain; 
//int cell_id[MAX_CELL_X][MAX_CELL_Y];
char buffer[80];

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
	wc.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(ICON_ID));
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hinstance;
	wc.lpfnWndProc = WinProc;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.lpszMenuName = NULL;
	wc.style = CS_DBLCLKS | CS_OWNDC | CS_VREDRAW | CS_HREDRAW;

	
	if(!RegisterClassEx(&wc))
		return(0);
	if(!(g_hwnd = CreateWindowEx(NULL, WINDOW_CLASS_NAME, "Dig Thug", WS_VISIBLE | WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hinstance, NULL)))
		return(0);

	ShowCursor(false);
	if (!GameInit())
		return(0);
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
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
	

	LoadBitmapFile(&bitmap, "artwork\\menu.bmp");
	CreateBOB(&menu, 0, 0, 640, 480, 1);
	LoadFrameBOB32(&menu, &bitmap, 0, 0, 0);
	if(!UnloadBitmapFile(&bitmap))
		return(0);
	
	LoadBitmapFile(&bitmap, "artwork\\digthug.bmp");
	CreateBOB(&dude,176,56,32,32,8);
	for (int i = 0; i < 8; i++)
		LoadFrameBOB32(&dude, &bitmap, i, i * 32, 32);
	if(!UnloadBitmapFile(&bitmap))
		return(0);

	LoadBitmapFile(&bitmap, "artwork\\tiles.bmp");
	CreateBOB(&tile,0,0,32,32, 16);
	for (int i = 0; i < 16; i++)
		LoadFrameBOB32(&tile, &bitmap, i, i * 32, 0);
	//LoadFrameBOB32(&tile, &bitmap, 1, 32, 0);
	if(!UnloadBitmapFile(&bitmap))
		return(0);

	LoadBitmapFile(&bitmap, "artwork\\back.bmp");
	CreateBOB(&back,0,0,800,600,1);
	LoadFrameBOB32(&back, &bitmap, 0, 0, 0);
	if(!UnloadBitmapFile(&bitmap))
		return(0);

	LoadBitmapFile(&bitmap, "artwork\\menu.bmp");
	CreateBOB(&menu,0,0,800,600,1);
	LoadFrameBOB32(&menu,&bitmap, 0, 0, 0);
	if(!UnloadBitmapFile(&bitmap))
		return(0);
	
	if (!LoadBitmapFile(&bitmap, "artwork\\menu_items.bmp"))
		return(0);

	CreateBOB(&menu_item_playgame,265,375,256,32,2);
	LoadFrameBOB32(&menu_item_playgame,&bitmap,0,0,0);
	LoadFrameBOB32(&menu_item_playgame,&bitmap,1,256,0);

	CreateBOB(&menu_item_instructions,265,408,256,32,2);
	LoadFrameBOB32(&menu_item_instructions,&bitmap,0,0,32);
	LoadFrameBOB32(&menu_item_instructions,&bitmap,1,256,32);

	CreateBOB(&menu_item_exit,265,440,256,32,2);
	LoadFrameBOB32(&menu_item_exit,&bitmap,0,0,64);
	LoadFrameBOB32(&menu_item_exit,&bitmap,1,256,64);

	if (!UnloadBitmapFile(&bitmap))
		return(0);


	if (!LoadBitmapFile(&bitmap, "artwork\\numbers.bmp"))
		return(0);
	CreateBOB(&numbers,0,0,16,16,10);
	for (int i = 0; i < 10; i++)
	{
		LoadFrameBOB32(&numbers,&bitmap,i,i*16,0);
	}
	if (!UnloadBitmapFile(&bitmap))
		return(0);

	if (!LoadBitmapFile(&bitmap, "artwork\\windup.bmp"))
		return(0);
	CreateBOB(&windup,0,0,32,32,4);
	for (int i = 0; i < 4; i++)
		LoadFrameBOB32(&windup,&bitmap,i,i*32,0);
	if (!UnloadBitmapFile(&bitmap))
		return(0);

	if (!LoadBitmapFile(&bitmap, "artwork\\instructions.bmp"))
		return(0);
	CreateBOB(&instructions,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,1);
	LoadFrameBOB32(&instructions,&bitmap,0,0,0);
	if (!UnloadBitmapFile(&bitmap))
		return(0);

	if (!LoadBitmapFile(&bitmap, "artwork\\chain.bmp"))
		return(0);
	CreateBOB(&chain,0,0,32,32,2);
	LoadFrameBOB32(&chain,&bitmap,0,0,0);
	LoadFrameBOB32(&chain,&bitmap,1,32,0);
	if (!UnloadBitmapFile(&bitmap))
		return(0);

	if (!LoadBitmapFile(&bitmap, "artwork\\windup_death.bmp"))
		return(0);
	CreateBOB(&windup_death,0,0,64,64,8);
	for (int i = 0; i < 8; i++)
		LoadFrameBOB32(&windup_death,&bitmap,i,i*64,0);
	//LoadFrameBOB32(&chain,&bitmap,1,32,0);
	if (!UnloadBitmapFile(&bitmap))
		return(0);

	InitCells();
	InitEnemies();
	//game_world[i] = ALL;
	

	return(1);
}

int GameKill()
{
	DestroyBOB(&windup_death);
	DestroyBOB(&windup);
	DestroyBOB(&chain);
	DestroyBOB(&instructions);
	DestroyBOB(&numbers);
	DestroyBOB(&menu_item_exit);
	DestroyBOB(&menu_item_playgame);
	DestroyBOB(&menu_item_instructions);
	DestroyBOB(&back);
	DestroyBOB(&tile);
	DestroyBOB(&dude);
	DestroyBOB(&menu);
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
	DWORD start_time = GetTickCount();
	DDRAW_INIT_STRUCT(ddsd);
	DDrawClearSurface(lpddsback, 0);
	switch(game_state)
	{
		case MENU:
		{
			static int curr_menu_item = 0;
			static int prev_up = 0, prev_down = 0; 
			int curr_up = KEYDOWN(VK_UP), curr_down = KEYDOWN(VK_DOWN);
			if (curr_up && !prev_up)
			{
				//curr_up = KEYDOWN(VK_UP);
				curr_menu_item--;
				//Sleep(100);
			}
			if (curr_down && !prev_down)
			{
				curr_menu_item++;
				//Sleep(100);
			}
			prev_up = curr_up;
			prev_down = curr_down;
			if (KEYDOWN(VK_RETURN))
			{
				switch(curr_menu_item)
				{
					case 0:
						game_state = INITGAME;
						break;
					case 1:
						game_state = INSTRUCTIONS;
						break;
					case 2:
						game_state = EXIT;
						break;
				}
			}
			if (curr_menu_item >= 3) curr_menu_item = 0;
			else if (curr_menu_item < 0) curr_menu_item = 2;
			/*	game_state = INITGAME;
			if (KEYDOWN(VK_2))
				game_state = INSTRUCTIONS;
			if (KEYDOWN(VK_3))
				game_state = EXIT;*/
			menu_item_playgame.curr_frame = 0;
			menu_item_instructions.curr_frame = 0;
			menu_item_exit.curr_frame = 0;
			switch(curr_menu_item)
			{
				case 0:
					menu_item_playgame.curr_frame = 1;
					break;
				case 1:
					menu_item_instructions.curr_frame = 1;
					break;
				case 2:
					menu_item_exit.curr_frame = 1;
					break;
			}
			DrawBOB32(&back, lpddsback);
			DrawCells();
			Printf(X_OFFSET + 70,0,"%d",player.score);
			Printf(600,584,"%d",1);
			DrawBOB32(&menu, lpddsback);
			DrawBOB32(&menu_item_playgame, lpddsback);
			DrawBOB32(&menu_item_instructions, lpddsback);
			DrawBOB32(&menu_item_exit, lpddsback);
		}break;
		case INITGAME:
		{
			player.x = X_OFFSET;
			player.y = Y_OFFSET;
			player.score = 0;
			for (int i = 0; i < 4; i++)
			{
				enemies[i].state = ENEMY_ALIVE;
				enemies[i].x = 210;
				enemies[i].y = 216;
			}
			enemies[4].state = ENEMY_ALIVE;
			enemies[4].x = 274;
			enemies[4].y = 408;
			enemies[5].state = ENEMY_ALIVE;
			enemies[5].x = 464;
			enemies[5].y = 150;
			enemies[6].state = ENEMY_ALIVE;
			enemies[6].x = 464;
			enemies[6].y = 408;
			InitCells();
			game_state = PLAYGAME;
		}break;
		case PLAYGAME:
		{
			bool moved = false;
			static int direction = 1;
			player.shoot = false;
			//void  process_player_controls()
			//{
			int x, y;
			x = ((int)player.x - X_OFFSET) % 32, y = ((int)player.y - Y_OFFSET) % 32;

			if (KEYDOWN(VK_ESCAPE))
				game_state = MENU;
			if (KEYDOWN(VK_SPACE))
			{
				player.shoot = true;
			}
			else
			{
				if (KEYDOWN(VK_LEFT))
				{
					if(!y)
					{
						player.xv = -2;
						player.yv = 0;
						
					}
					moved = true;
				}
				else if (KEYDOWN(VK_RIGHT))
				{
					if (!y)
					{
						player.xv = 2;
						player.yv = 0;
						
					}
					moved = true;
				}
				else if (KEYDOWN(VK_UP))
				{
					if(!x)
					{
						player.yv = -2;
						player.xv = 0;
						
					}
					moved = true;
				}
				else if (KEYDOWN(VK_DOWN))
				{
					if(!x)
					{
						player.yv = 2;
						player.xv = 0;
						
					}
					moved = true;
				}
			}
			if (moved)
			{
				player.x += player.xv;
				player.y += player.yv;

				if (player.x < 176) 
				{
					player.x = 176;
					moved = false;
				}
				else if (player.x + 32 > 624)
				{
					player.x -= player.xv;
					moved = false;
				}

				if (player.y < 56)
				{
					player.y = 56;
					moved = false;
				}
				else if (player.y + 32 > 568) 
				{
					player.y -= player.yv;
					moved = false;
				}
				
				
			}

			if (player.shoot) 
			{
				if (player.xv < 0)
				{
					chain.x = player.x - 32;
					chain.y = player.y;
					chain.curr_frame = 0;
				}
				if (player.xv > 0)
				{
					chain.x = player.x + 32;
					chain.y = player.y;
					chain.curr_frame = 0;
				}
				if (player.yv < 0)
				{
					chain.x = player.x;
					chain.y = player.y - 32;
					chain.curr_frame = 1;
				}
				if (player.yv > 0)
				{
					chain.x = player.x;
					chain.y = player.y + 32;
					chain.curr_frame = 1;
				}
			}
			MoveEnemies();


			float x1 = (player.x  - X_OFFSET) / 32.0f;
			float y1 = (player.y  -  Y_OFFSET) / 32.0f;
			float x2 = (player.x + 31 - X_OFFSET) / 32.0f;
			float y2 = (player.y + 31 - Y_OFFSET) / 32.0f;

			if (x1 >= 0 && y1 >= 0)
			{
			/*	if (cell_id[(int)x1][(int)y1] || cell_id[(int)x2][(int)y1] || cell_id[(int)x1][(int)y2] || cell_id[(int)x2][(int)y2])
				{
					//player.x -= player.xv;
					//player.y -= player.yv;
				}*/
				if (moved)
				{
					if (player.xv < 0)
					{
						int x = x1 * 2;
						if (x % 2)
							cell[(int)x1][(int)y1].tile_id &= ~RIGHT;
						else
							cell[(int)x1][(int)y1].tile_id &= ~LEFT;
					}
					else if (player.xv > 0)
					{
						int x = x2 * 2;
						if (x % 2)
							cell[(int)x2][(int)y1].tile_id &= ~RIGHT;
						else
							cell[(int)x2][(int)y1].tile_id &= ~LEFT;
					}
					else if (player.yv < 0)
					{
						int y = y1 * 2;
						if (y % 2)
							cell[(int)x1][(int)y1].tile_id &= ~BOTTOM;
						else
							cell[(int)x1][(int)y1].tile_id &= ~TOP;
					}
					else if (player.yv > 0)
					{
						int y = y2 * 2;
						if (y % 2)
							cell[(int)x1][(int)y2].tile_id &= ~BOTTOM;
						else
							cell[(int)x1][(int)y2].tile_id &= ~TOP;
					}
				}
			}
			
			if (player.xv > 0) direction = 0;
			else if (player.xv < 0) direction = 1;
			if (player.yv > 0) direction = 2;
			else if (player.yv < 0) direction = 3;

			if (++dude.counter >= dude.threshold)
			{
				dude.counter = 0;
				dude.threshold = 5;
				dude.curr_frame++;
			}

			if (direction == 0)
			{
				if (moved)
				{
					if (dude.curr_frame >= 2) dude.curr_frame = 0;
				}
				else dude.curr_frame = 0;
			}
			else if (direction == 1)
			{
				if (moved)
				{
					if (dude.curr_frame >= 4 || dude.curr_frame < 2) dude.curr_frame = 2;
				}
				else dude.curr_frame = 2;
				
			}
			else if (direction == 2)
			{	
				if (moved)
				{
					if (dude.curr_frame >= 6 || dude.curr_frame < 4) dude.curr_frame = 4;
				}
				else dude.curr_frame = 4;
			}
			else if (direction == 3)
			{
				if (moved)
				{
					if (dude.curr_frame >= 8 || dude.curr_frame < 6) dude.curr_frame = 6;
				}
				else dude.curr_frame = 6;
			}



			

			/*if (FAILED(lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)))
			{

				return(0);
			}

			if (FAILED(lpddsback->Unlock(NULL)))
			{

				return(0);
			}*/


			DrawBOB32(&back, lpddsback);
			DrawCells();
			
			dude.x = player.x;
			dude.y = player.y;
			DrawBOB32(&dude, lpddsback);
			DrawEnemies();
			
			if (player.shoot) 
			{
				if (player.xv < 0)
				{
					chain.x = player.x - 32;
					chain.y = player.y;
					chain.curr_frame = 0;
				}
				if (player.xv > 0)
				{
					chain.x = player.x + 32;
					chain.y = player.y;
					chain.curr_frame = 0;
				}
				if (player.yv < 0)
				{
					chain.x = player.x;
					chain.y = player.y - 32;
					chain.curr_frame = 1;
				}
				if (player.yv > 0)
				{
					chain.x = player.x;
					chain.y = player.y + 32;
					chain.curr_frame = 1;
				}
				DrawBOB32(&chain, lpddsback);
			}
			Printf(X_OFFSET + 70,0,"%d",player.score);
			Printf(600,584,"%d",1);
			
		}break;
		case INSTRUCTIONS:
		{
			if (KEYDOWN(VK_ESCAPE))
				game_state = MENU;
			DrawBOB32(&back, lpddsback);
			DrawCells();
			DrawBOB32(&instructions,lpddsback);
		}break;
		case EXIT:
		{
			SendMessage(g_hwnd, WM_DESTROY, 0, 0);
		}break;

	}
	
	
	lpddsprimary->Flip(NULL, DDFLIP_WAIT);
	
	return(1);
}

void InitCells()
{
	for (int x = 0; x < MAX_CELL_X; x++)
	{
		for (int y = 0; y < MAX_CELL_Y; y++)
		{
			if (y == 0)
				cell[x][y].tile_id = NONE;
			if (y == 1)
				cell[x][y].tile_id = BOTTOM;
			else cell[x][y].tile_id = ALL;
		}
	}
	cell[1][3].tile_id = 0;
	cell[1][4].tile_id = 0;
	cell[1][5].tile_id = 0;
	cell[1][6].tile_id = 0;
	cell[1][7].tile_id = 0;

	cell[9][3].tile_id = 0;
	cell[10][3].tile_id = 0;
	cell[11][3].tile_id = 0;
	cell[12][3].tile_id = 0;

	cell[2][11].tile_id = 0;
	cell[3][11].tile_id = 0;
	cell[4][11].tile_id = 0;
	cell[5][11].tile_id = 0;

	cell[9][10].tile_id = 0;
	cell[9][11].tile_id = 0;
	cell[9][12].tile_id = 0;
	cell[9][13].tile_id = 0;
	cell[9][14].tile_id = 0;

	
	cell[5][8].tile_id = 0;
	cell[6][8].tile_id = 0;
	cell[7][8].tile_id = 0;
	
}

void  DrawCells()
{
	DrawBOB32(&back, lpddsback);
	tile.x = 288;
	tile.y = 64;
	for (int y = 1; y < 16; y++)
	{
		tile.y = (y << 5) + 56;
		for (int x = 0; x < 14; x++)
		{
			tile.x = (x << 5) + 176;
			tile.curr_frame = cell[x][y].tile_id;
			DrawBOB32(&tile, lpddsback);
		}
	}
}

void InitEnemies()
{
	memset(enemies,0,MAX_ENEMIES);
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		enemies[i].newdirection = true;
	}
}

void MoveEnemies()
{
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		if (enemies[i].state == ENEMY_ALIVE)
		{
			int x, y;
			x = ((int)enemies[i].x - X_OFFSET) % 32, y = ((int)enemies[i].y - Y_OFFSET) % 32;
			//if ((++enemies[i].counter > enemies[i].threshold))
			//{
			//	enemies[i].counter = 0;
			//	enemies[i].threshold = rand()%10 + 20;
			//	enemies[i].newdirection = true;
			//}
			if (enemies[i].newdirection)
			{
				enemies[i].newdirection = false;
				/*if (dude.x < enemies[i].x)
				{
					if (!y)
					{
						enemies[i].xv = -2;
						enemies[i].yv = 0;
						enemies[i].curr_frame = 1;
					}
					enemies[i].moved = true;
				}
				else if (dude.x > enemies[i].x)
				{
					if (!y)
						{
							enemies[i].xv = 2;
							enemies[i].yv = 0;
							enemies[i].curr_frame = 0;
						}
						enemies[i].moved = true;
				}
				if (dude.y < enemies[i].y)
				{
					if (!x)
						{
							enemies[i].xv = 0;
							enemies[i].yv = -2;
							enemies[i].curr_frame = 3;
						}
						enemies[i].moved = true;
				}
				else if (dude.y > enemies[i].y)
				{
					if (!x)
						{
							enemies[i].xv = 0;
							enemies[i].yv = 2;
							enemies[i].curr_frame =2;
						}
						enemies[i].moved = true;

				}*/
				switch(rand()%4)
				{
					case 0:
					{
						if (!y)
						{
							enemies[i].xv = -2;
							enemies[i].yv = 0;
							enemies[i].curr_frame = 1;
						}
						enemies[i].moved = true;
					}break;
					case 1:
					{
						if (!y)
						{
							enemies[i].xv = 2;
							enemies[i].yv = 0;
							enemies[i].curr_frame = 0;
						}
						enemies[i].moved = true;
					}break;
					case 2:
					{
						if (!x)
						{
							enemies[i].xv = 0;
							enemies[i].yv = -2;
							enemies[i].curr_frame = 3;
						}
						enemies[i].moved = true;
					}break;
					case 3:
					{
						if (!x)
						{
							enemies[i].xv = 0;
							enemies[i].yv = 2;
							enemies[i].curr_frame =2;
						}
						enemies[i].moved = true;
					}break;
					default: 
						enemies[i].xv = enemies[i].yv = 0;
						enemies[i].moved = false;
				}
			}
			if (enemies[i].moved)
			{
				enemies[i].x += enemies[i].xv;
				enemies[i].y += enemies[i].yv;

				if (enemies[i].x < 176) 
				{
					enemies[i].x = 176;
					enemies[i].moved = false;
					enemies[i].newdirection = true;
				}
				else if (enemies[i].x + 32 > 624)
				{
					enemies[i].x -= enemies[i].xv;
					enemies[i].moved = false;
					enemies[i].newdirection = true;
				}

				if (enemies[i].y < 56)
				{
					enemies[i].y = 56;
					enemies[i].moved = false;
					enemies[i].newdirection = true;
				}
				else if (enemies[i].y + 32 > 568) 
				{
					enemies[i].y -= enemies[i].yv;
					enemies[i].moved = false;
					enemies[i].newdirection = true;
				}
				
				float x1 = (enemies[i].x  - X_OFFSET) / 32.0f;
				float y1 = (enemies[i].y  -  Y_OFFSET) / 32.0f;
				float x2 = (enemies[i].x + 31 - X_OFFSET) / 32.0f;
				float y2 = (enemies[i].y + 31 - Y_OFFSET) / 32.0f;

				if (x1 >= 0 && y1 >= 0)
				{
					if (cell[(int)x1][(int)y1].tile_id || cell[(int)x2][(int)y1].tile_id || cell[(int)x1][(int)y2].tile_id || cell[(int)x2][(int)y2].tile_id)
					{
						enemies[i].x -= enemies[i].xv;
						enemies[i].y -= enemies[i].yv;
						enemies[i].newdirection = true;
						enemies[i].xv = -enemies[i].xv;
						enemies[i].yv = -enemies[i].yv;
						
					}
					
				}

				if (player.shoot)
				{
					if (player.xv)
					{
						if (chain.y + 16 >= enemies[i].y && chain.y + 16 <= enemies[i].y + 32  && (chain.x >= enemies[i].x && chain.x <= enemies[i].x + 32 ||
							chain.x + 32 >= enemies[i].x && chain.x + 32 <= enemies[i].x + 32) )
						{
							player.score += rand()%500 + 1000;
							enemies[i].state = ENEMY_DYING;
							enemies[i].curr_frame = 0;
							//PlaySound("audio\\dying.wav",g_hinstance,SND_ASYNC);
						}
							
					}
					else if (player.yv)
					{
						if (chain.x + 16 >= enemies[i].x && chain.x + 16 <= enemies[i].x + 32  && (chain.y >= enemies[i].y && chain.y <= enemies[i].y + 32 ||
							chain.y + 32 >= enemies[i].y && chain.y + 32 <= enemies[i].y + 32) )
						{
							player.score += rand()%500 + 1000;
							enemies[i].state = ENEMY_DYING;
							enemies[i].curr_frame = 0;
							//PlaySound("audio\\dying.wav",g_hinstance,SND_ASYNC);
						}
					}
				}
			}
		}

	}
}

void DrawEnemies()
{
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		if (enemies[i].state == ENEMY_ALIVE)
		{
			windup.curr_frame = enemies[i].curr_frame;
			windup.x = enemies[i].x;
			windup.y = enemies[i].y;
			DrawBOB32(&windup,lpddsback);
		}
		else if (enemies[i].state == ENEMY_DYING)
		{
			if (++enemies[i].counter >= enemies[i].threshold)
			{
				enemies[i].counter = 0;
				enemies[i].threshold = 8;
				if (++enemies[i].curr_frame > 8)
				{
					enemies[i].state = ENEMY_DEAD;
				}
			}
			windup_death.curr_frame = enemies[i].curr_frame;
			windup_death.x = enemies[i].x - 16;
			windup_death.y = enemies[i].y - 16;
			DrawBOB32(&windup_death,lpddsback);
		}
	}
}

void Printf(int x,int y,const char *fmt,...)
{
	int index,x_val,length;
	char text[256];
	va_list ap;
	if (fmt == 0)
		return;
	va_start(ap, fmt);
	vsprintf(text, fmt, ap);
	va_end(ap);
	length = int(strlen(text));
	
	index = 0;
	x_val = x;
	while(index < length)
	{
		numbers.curr_frame = text[index]-48;
		numbers.x = x_val;
		numbers.y = y;
		DrawBOB32(&numbers,lpddsback);
		index++;
		x_val += 14;
	}
	
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
	dest_rect.right = bob->x + bob->width;
	dest_rect.bottom = bob->y + bob->height;

	src_rect.left = 0;
	src_rect.top = 0;
	src_rect.right = bob->width;
	src_rect.bottom = bob->height;

	if (FAILED(dest->Blt(&dest_rect, bob->images[bob->curr_frame], &src_rect, (DDBLT_WAIT | DDBLT_KEYSRC), NULL)))
		return(0);
	return(1);
}