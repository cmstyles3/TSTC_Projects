// AI 2 FINAL PROJECT
// TITLE: ZOMBIE STALKER
// DESCRIPTION: ///////////////////////////////////////////////////////////////
// This program is a 2D shooter game which uses a grid based game world of   //
// 25x15.  Each Tile is 32x32 pixels and has an id and a flag. The id can    //
// represent either wall or floor. The flag is used for placing starting     //
// position for player, enemy spawns, and pick-ups such as ammunition and    //
// coke. This game uses the A* algorithm for path finding and a finite state //
// machine for the enemy AI.                                                 //
// ////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY: COLLIN MUNSON

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <stdlib.h>
#include "d3dbase.h"
#include "CInput.h"
#include "Sprite.h"
#include "dxutil/dxutil.h"
#include "CFont.h"
#include "FMOD/inc/fmod.hpp"
#include <vector>

using namespace std;

// Defines
//-------------------------------------------------------------
#define CLASSNAME "ZOMBIE"
#define WINDOWNAME "ZOMBIE STALKER"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MAP_WIDTH 25
#define MAP_HEIGHT 15

#define TILE_SIZE 32

#define NUM_TILES 2
#define NUM_FLAGS 5

enum TILEID {TILEWALL, TILEFLOOR};
enum FLAGID {EMPTY, START, ENEMY_SPAWN, AMMO, COKE};

enum GAMESTATE {MENU, SETUPGAME, PLAYGAME, EDITOR, ENDGAME};

enum ENEMYSTATE {DEAD, RESPAWN, CHASE, EVADE, ATTACK, PAUSE};
enum DIRECTION {DOWN, UP, LEFT, RIGHT};

struct PLAYER_INFO
{
	int x, y; //positions
	int xv, yv; //velocities
	int health;
	int ammo;
	int currFrame;
	int threshold;
	int counter;
	int score;
	int kills;
	int hits, shotsfired;
	DIRECTION direction;
};

PLAYER_INFO Player;


#define MAX_ENEMIES 150


struct ENEMY_INFO
{
	int x, y;
	int xv, yv;
	bool newpath;
	int dirx, diry;
	int targetx, targety;
	int health;
	int state;
	int currFrame;
	int threshold;
	float counter;
	DIRECTION direction;
	float respawnTimer;
	bool animate;
};

ENEMY_INFO Enemies[MAX_ENEMIES];

#define MAX_BULLETS 20

struct BULLET_INFO
{
	int x, y;
	int xv, yv;
	bool active;
};

BULLET_INFO Bullets[MAX_BULLETS];


struct SPAWN_POINTS
{
	int x, y;
};
vector<SPAWN_POINTS> spawnPoints;

// Function Prototypes
//------------------------------------------------------------------------------------------------------------------------
int  GameInit();
int  GameMain(float);
void GameMurder();

void InitMap();
void DrawMap();

void DrawDisplay();

void LoadMap();
void SaveMap();

void ProcessPlayerControls(float);

//PATH FINDING FUNCTION
void FindPath(int startx, int starty, int targetx, int targety, int &nextx, int &nexty);

//PLAYER FUNCTIONS
void InitPlayer();
void UpdatePlayer();
void MovePlayer();
void DrawPlayer();

//ENEMY FUNCTIONS
void InitEnemy(int index);
void InitEnemies();
void UpdateEnemies(float deltatime);
void DrawEnemies();

//BULLET FUNCTIONS
void CreateBullet();
void InitBullets();
void UpdateBullets();
void DrawBullets();

//SOUND SYSTEM FUNCTIONS
int InitSoundSystem();
void KillSoundSystem();
void PlaySound(FMOD::Sound *sound);
int LoadSound(char filename[], FMOD::Sound **sound);
void UpdateSoundSystem();

void DrawMines();

HINSTANCE g_hInstance = 0;
HWND g_hWnd = 0;

GAMESTATE GameState = MENU;

LPD3DXSPRITE g_d3dxSprite = 0;

CFont g_Font;
CFont g_FontSmall;
CInput g_Input;

CSprite g_spriteTiles;
CSprite g_spritePlayer;
CSprite g_spriteMenu;
CSprite g_spriteCursor;
CSprite g_spriteFlags;
CSprite g_spriteZombie;
CSprite g_spriteBullet;

int Map[MAP_WIDTH][MAP_HEIGHT];
int MapItem[MAP_WIDTH][MAP_HEIGHT];
int MapPath[MAP_WIDTH][MAP_HEIGHT];
int MapMark[MAP_WIDTH][MAP_HEIGHT];

float gameTimer = 140;

D3DXVECTOR2 g_cursorPos(0,0);

DWORD framelock = GetTickCount();

// SOUND ----------------------------------
FMOD::System *pfmodSystem = 0;
enum {GUNSHOT, RELOAD, DRINK, HIT, ZOMBIE};
FMOD::Sound *sound[8] = {0};


//GLOBALS USED BY EDITOR
int currTile = 0, currFlag = 0; // used by editor
char *currTileStr[] = { "WALL", "FLOOR" };
char *currFlagStr[] = { "EMPTY", "START", "ENEMY SPAWN", "AMMUNITION", "COKE" };
int startx = 0, starty = 0;

int EnemyCount = 0;
int MaxEnemies = 5;

float TimeSurvive = 0;

struct Mine
{
	D3DXVECTOR2 pos;
	bool hit;
	float timer;
	int killCount;
};

std::vector<Mine> mines;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
			
	}
	return(DefWindowProc(hwnd,msg,wparam,lparam));
}


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int nshowcmd)
{
	//MessageBox(0, "Consult README.txt for controls", "INSTRUCTIONS", 0);
	g_hInstance = hinstance;

	mines.clear();

	Mine mine;
	mines.push_back(mine);
	
	srand((unsigned int)GetTickCount());
	if (!GameInit())
	{
		MessageBox(0, "GameInit() - FAIILED", 0, 0);
		return(0);
	}
	
	EnterMsgLoop(GameMain);

	GameMurder();

	UnregisterClass(CLASSNAME, g_hInstance);
	
	return(0);
}

int GameInit()
{
	// Initialize Direct3D -------------------------------------------------------------------
	if (!InitD3D(CLASSNAME, WINDOWNAME, SCREEN_WIDTH, SCREEN_HEIGHT, false,  D3DDEVTYPE_HAL))
	{	
		MessageBox(0,"InitD3D() - FAILED", 0, 0);
		return(0);
	}
	
	// Initialize DirectInput ----------------------------------------------------------------
	if (!g_Input.InitDI())
	{
		MessageBox(0, "could not initialize direct input", 0, 0);
		return(0);
	}

	// Initialize Keyboard with DirectInput ---------------------------------------------------
	if (!g_Input.InitKB())
	{
		MessageBox(0, "could not initialize keyboard", 0, 0);
		return(0);
	}

	// Initialize Mouse with DirectInput ---------------------------------------------------
	if (!g_Input.InitMouse())
	{
		MessageBox(0, "could not initialize mouse", 0, 0);
		return(0);
	}

	if (!InitSoundSystem())
	{
		MessageBox(0, "InitSoundSytem Failed", 0 ,0);
		return(0);
	}

	LoadSound("sounds\\gun.wav", &sound[GUNSHOT]);
	LoadSound("sounds\\ammo.wav", &sound[RELOAD]);
	LoadSound("sounds\\coke.wav", &sound[DRINK]);
	LoadSound("sounds\\hit.wav", &sound[HIT]);
	LoadSound("sounds\\zombiemoan.wav", &sound[ZOMBIE]);

	

	D3DXCreateSprite(Device, &g_d3dxSprite);
	
	g_Font.Init(Device, g_d3dxSprite, "graphics\\font.dds", 32, 32);
	g_FontSmall.Init(Device, g_d3dxSprite, "graphics\\font-small.dds", 16, 16);

	g_spriteTiles.Init(Device, g_d3dxSprite, 2);
	g_spriteTiles.Load("graphics\\tiles.dds");

	//Load Tiles
	g_spriteTiles.SetFrame(0, 0, 0, 32, 32);
	g_spriteTiles.SetFrame(1, 32, 0, 32, 32);

	g_spriteFlags.Init(Device, g_d3dxSprite, NUM_FLAGS);
	g_spriteFlags.Load("graphics\\TileFlags.dds");
	//Load Flag Tiles
	g_spriteFlags.SetFrame(EMPTY, 0, 0, 32, 32);
	g_spriteFlags.SetFrame(START, 32, 0, 32, 32);
	g_spriteFlags.SetFrame(ENEMY_SPAWN, 64, 0, 32, 32);
	g_spriteFlags.SetFrame(AMMO, 96, 0, 32, 32);
	g_spriteFlags.SetFrame(COKE, 0, 32, 32, 32);
	
	//g_spriteTiles.SetFrame(2, 64, 0, 

	g_spritePlayer.Init(Device, g_d3dxSprite, 12);
	g_spritePlayer.Load("graphics\\player.dds");
	g_spritePlayer.SetFrame(0, 0, 0, 32, 32);
	g_spritePlayer.SetFrame(1, 32, 0, 32, 32);
	g_spritePlayer.SetFrame(2, 64, 0, 32, 32);
	g_spritePlayer.SetFrame(3, 96, 0, 32, 32);
	g_spritePlayer.SetFrame(4, 128, 0, 32, 32);
	g_spritePlayer.SetFrame(5, 160, 0, 32, 32);

	g_spritePlayer.SetFrame(6, 0, 32, 32, 32);
	g_spritePlayer.SetFrame(7, 32, 32, 32, 32);
	g_spritePlayer.SetFrame(8, 64, 32, 32, 32);
	g_spritePlayer.SetFrame(9, 96, 32, 32, 32);
	g_spritePlayer.SetFrame(10, 128, 32, 32, 32);
	g_spritePlayer.SetFrame(11, 160, 32, 32, 32);
	
	
	g_spriteMenu.Init(Device, g_d3dxSprite, 1);
	g_spriteMenu.Load("graphics\\menu.bmp");
	g_spriteMenu.SetFrame(0, 0, 0, 800, 600);
	
	g_spriteCursor.Init(Device, g_d3dxSprite, 1);
	g_spriteCursor.Load("graphics\\cursor.dds");
	g_spriteCursor.SetFrame(0, 0, 0, 32, 32);

	g_spriteZombie.Init(Device, g_d3dxSprite, 12);
	g_spriteZombie.Load("graphics\\zombie.dds");

	//DOWN FACING
	g_spriteZombie.SetFrame(0, 0, 0, 32, 32);
	g_spriteZombie.SetFrame(1, 32, 0, 32, 32);
	g_spriteZombie.SetFrame(2, 64, 0, 32, 32);

	//UP FACING
	g_spriteZombie.SetFrame(3, 96, 0, 32, 32);
	g_spriteZombie.SetFrame(4, 128, 0, 32, 32);
	g_spriteZombie.SetFrame(5, 160, 0, 32, 32);

	//RIGHT FACING
	g_spriteZombie.SetFrame(6, 0, 32, 32, 32);
	g_spriteZombie.SetFrame(7, 32, 32, 32, 32);
	g_spriteZombie.SetFrame(8, 64, 32, 32, 32);

	//LEFT FACING
	g_spriteZombie.SetFrame(9, 96, 32, 32, 32);
	g_spriteZombie.SetFrame(10, 128, 32, 32, 32);
	g_spriteZombie.SetFrame(11, 160, 32, 32, 32);

	g_spriteBullet.Init(Device, g_d3dxSprite, 1);
	g_spriteBullet.Load("graphics\\bullet.dds");
	g_spriteBullet.SetFrame(0, 0, 0, 8, 8);
	

	// SETUP PROJECTION
	/*D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.45f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, DRAW_DISTANCE);
	Device->SetTransform(D3DTS_PROJECTION, &proj);*/
	srand(GetTickCount());
	
	return(1);
}

//kill everything, make sure there are absolutely no survivors
void GameMurder()
{

	SAFE_RELEASE(g_d3dxSprite);

	KillSoundSystem();
	
	//kill direct input
	g_Input.KillMouse();
	g_Input.KillKB();
	g_Input.KillDI();

	//kill device
	KillD3D();
}



int GameMain(float timeDelta)
{
	
	static int Kills = 0;
	static int Score = 0;
	static bool survive = true;
	
	if (Device)
	{
		framelock = timeGetTime();
		int nextX = 0, nextY = 0;
		//if (timeGetTime() > framelock+16)
		{
		//Read Keyboard
		g_Input.ReadKB();
		g_Input.ReadMouse();
		//UPDATE
		switch(GameState)
		{
			case MENU:
				if ((g_Input.IsKeyDown(DIK_ESCAPE) && !g_Input.WasKeyDown(DIK_ESCAPE)) ||
					(g_Input.IsKeyDown(DIK_3)      && !g_Input.WasKeyDown(DIK_3)))
					PostQuitMessage(0);

				if (g_Input.IsKeyDown(DIK_1))
				{
					GameState = SETUPGAME;
					
					PlaySound(sound[0]);
				}

				if (g_Input.IsKeyDown(DIK_2))
				{
					LoadMap();
					GameState = EDITOR;
				}


				break;
			case SETUPGAME:
				gameTimer = 120;
				survive = true;
				TimeSurvive = 0;
				spawnPoints.clear();
				mines.clear();
				InitPlayer();
				LoadMap();

				EnemyCount = 0;
				MaxEnemies = 5;
				
				InitEnemies();
				InitBullets();
				GameState = PLAYGAME;
				break;
			case PLAYGAME:
			{
				//Update Timer
				gameTimer -= timeDelta;
				TimeSurvive += timeDelta;
				if (gameTimer <= 0 || Player.health <= 0)
				{
					Player.score += TimeSurvive * 100;
					if (Player.health <= 0)
						survive = false;
					GameState = ENDGAME;
					break;
				}
				
				else if (gameTimer < 40) MaxEnemies = 100;
				else if (gameTimer < 60) MaxEnemies = 50;
				else if (gameTimer < 80) MaxEnemies = 25;
				else if (gameTimer < 100) MaxEnemies = 10;
				else if (gameTimer < 120) MaxEnemies = 5;
				
				Player.xv = Player.yv = 0;
				ProcessPlayerControls(timeDelta);
				
				UpdateBullets();
				UpdateEnemies(timeDelta);
				//enemyVel.x = enemyVel.y = 0;
				
					
					
				}break;
			case ENDGAME:
			{
				if (g_Input.IsKeyDown(DIK_ESCAPE))
					GameState = MENU;

			}break;

			case EDITOR:
				
				if (g_Input.IsKeyDown(DIK_ESCAPE))
					GameState = MENU;
				if (g_Input.IsKeyDown(DIK_S))
					SaveMap();
				if (g_Input.IsKeyDown(DIK_C))
				{
					memset(Map, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
					memset(MapItem, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);
				}
				if (g_Input.IsKeyDown(DIK_V))
				{
					for (int y = 0; y < MAP_HEIGHT; y++)
						for (int x = 0; x < MAP_WIDTH; x++)
						{
							if ((Map[x][y] = currTile) == TILEWALL)
								MapItem[x][y] = EMPTY;
						}
				}
				if (g_Input.IsKeyDown(DIK_B))
				{
					for (int y = 0; y < MAP_HEIGHT; y++)
						for (int x = 0; x < MAP_WIDTH; x++)
							MapItem[x][y] = currFlag;
				}
				if (g_Input.IsKeyDown(DIK_LEFT) && !g_Input.WasKeyDown(DIK_LEFT))
				{
					currTile --;
					if (currTile < 0) currTile = 1;
				}
				if (g_Input.IsKeyDown(DIK_RIGHT) && !g_Input.WasKeyDown(DIK_RIGHT))
				{
					currTile++;
					if (currTile > 1) currTile = 0;
				}
				if (g_Input.IsKeyDown(DIK_UP)  && !g_Input.WasKeyDown(DIK_UP))
				{
					currFlag--;
					if (currFlag < 0) currFlag = NUM_FLAGS - 1;
				}
				if (g_Input.IsKeyDown(DIK_DOWN)  && !g_Input.WasKeyDown(DIK_DOWN))
				{
					currFlag++;
					if (currFlag >= NUM_FLAGS) currFlag = 0;
				}
				g_cursorPos.x += g_Input.m_MouseState.lX * 0.5;
				g_cursorPos.y += g_Input.m_MouseState.lY * 0.5;

				if (g_cursorPos.x < 0) g_cursorPos.x = 0;
				if (g_cursorPos.y < 0) g_cursorPos.y = 0;
				if (g_cursorPos.x > SCREEN_WIDTH  - 1) g_cursorPos.x = SCREEN_WIDTH - 1;
				if (g_cursorPos.y > SCREEN_HEIGHT - 1) g_cursorPos.y = SCREEN_HEIGHT - 1;

				if (g_cursorPos.x < TILE_SIZE * MAP_WIDTH && g_cursorPos.y < TILE_SIZE * MAP_HEIGHT)
				{
					int id = Map[(int)g_cursorPos.x/32][(int)g_cursorPos.y/32];
					if (g_Input.IsButtonDown(0))
					{
						id = Map[(int)g_cursorPos.x/32][(int)g_cursorPos.y/32] = currTile;
						if (id == TILEWALL)
						{
							MapItem[(int)g_cursorPos.x/32][(int)g_cursorPos.y/32] = EMPTY;
						}
						
					}
					if (g_Input.IsButtonDown(1))
					{
						if (currFlag == START)
						{
							MapItem[startx][starty] = EMPTY;
							startx = g_cursorPos.x/TILE_SIZE;
							starty = g_cursorPos.y/TILE_SIZE;
						}
						if (id != TILEWALL)
							MapItem[(int)g_cursorPos.x/32][(int)g_cursorPos.y/32] = currFlag;

					}
				}
				break;
		}
		

		// RENDER
		Device->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
		Device->BeginScene();
		g_d3dxSprite->Begin(D3DXSPRITE_ALPHABLEND);

		switch(GameState)
		{
			case MENU:
				g_spriteMenu.Draw(0, 0, 0);
				break;
			case SETUPGAME:
				break;
			case PLAYGAME:
				
				DrawMap();
				DrawMines();
				DrawPlayer();
				DrawEnemies();
				DrawBullets();
				DrawDisplay();
				g_Font.Print(0,560,D3DCOLOR_XRGB(255, 0,0), "EnemyCount = %i, MaxEnemies = %i", EnemyCount, MaxEnemies);
				
				
				
				break;
			case ENDGAME:
				g_Font.Print(0,0,D3DCOLOR_XRGB(255,0,0), "END OF GAME STATS");
				g_Font.Print(0,32,D3DCOLOR_XRGB(255,0,0), "SCORE = %i", Player.score);
				g_Font.Print(0,64,D3DCOLOR_XRGB(255,0,0), "KILLS = %i", Player.kills);
				g_Font.Print(0,96,D3DCOLOR_XRGB(255,0,0), "HITS = %i", Player.hits);
				g_Font.Print(0,128,D3DCOLOR_XRGB(255,0,0), "SHOTS FIRED = %i", Player.shotsfired);
				g_Font.Print(0,160,D3DCOLOR_XRGB(255,0,0), "HIT PER = %.0f%%", 100*((float)Player.hits/Player.shotsfired));
				if (survive)
					g_Font.Print(0,192,D3DCOLOR_XRGB(255,0,0), "YOU WINS, YOU SURVIVED");
				else
					g_Font.Print(0,192,D3DCOLOR_XRGB(255,0,0), "YOU DIED, BETTER LUCK NEXT TIME");
				g_Font.Print(0, 550, D3DCOLOR_XRGB(0,255,0), "PRESS ESCAPE TO GO BACK TO MENU");
				break;
			case EDITOR:
				DrawMap();
				
				g_FontSmall.Print(15, 530, D3DCOLOR_XRGB(255,255,255), currTileStr[currTile]);
				g_spriteTiles.Draw(currTile, 20, 550);
				g_FontSmall.Print(70, 530, D3DCOLOR_XRGB(255,255,255), currFlagStr[currFlag]);
				g_spriteFlags.Draw(currFlag, 80, 550);
				
				g_FontSmall.Print(200, 480, 0xffffffff, "use left and right arrows to change current tile");
				g_FontSmall.Print(200, 500, 0xffffffff, "use up and down arrows to change current flag");
				g_FontSmall.Print(200, 520, 0xffffffff, "move mouse cursor over tile and click left button to change to the");
				g_FontSmall.Print(200, 540, 0xffffffff, "current tile or click the right button to change to the curren flag");
				g_FontSmall.Print(200, 560, 0xffff0000, "Press 'S' key to save map to be used in the game");
				g_FontSmall.Print(200, 580, 0xffff0000, "Press 'Esc' key to go back to menu");

				g_spriteCursor.Draw(0, g_cursorPos.x, g_cursorPos.y);
				break;

		}
		
		
		
		g_d3dxSprite->End();
		Device->EndScene();
		Device->Present(0,0,0,0);
		
		}
		UpdateSoundSystem();
		while(timeGetTime() - framelock < 15);

	}
	return(0);
}

void InitPlayer()
{
	Player.x = Player.y = 0;
	Player.xv = Player.yv = 0;
	Player.health = 10000;
	Player.ammo = 300;
	Player.threshold = 0;
	Player.counter = 0;
	Player.currFrame = 0;
	Player.hits = 0;
	Player.shotsfired = 0;
	Player.kills = 0;
	Player.score = 0;
	Player.direction = DOWN;
}


void ProcessPlayerControls(float timeDelta)
{
	if (g_Input.IsKeyDown(DIK_ESCAPE))
		GameState = MENU;
	if (g_Input.IsKeyDown(DIK_LEFT))
		Player.xv = -3;
	if (g_Input.IsKeyDown(DIK_RIGHT))
		Player.xv = 3;
	if (g_Input.IsKeyDown(DIK_UP))
		Player.yv = -3;
	if (g_Input.IsKeyDown(DIK_DOWN))
		Player.yv = 3;

	static DWORD mineTimer = timeGetTime();
	if (g_Input.IsKeyDown(DIK_M) && timeGetTime() - mineTimer > 500)
	{
		//mineTimer = timeGetTime();
		//for (int x = 0; x < 32; x++)
		{
			//for (int y = 0; y < 32; y++)
			{
				Mine mine;
				ZeroMemory(&mine, sizeof(Mine));
				mine.hit = false;
				mine.pos.x = Player.x + 16;
				mine.pos.y = Player.y + 16;

				mine.killCount = 0;

				mines.push_back(mine);
			}
		}
	}
	static DWORD gunTimer = timeGetTime();
	if (g_Input.IsKeyDown(DIK_SPACE) && timeGetTime() - gunTimer > 100 && Player.ammo > 0)
	{
		gunTimer = timeGetTime();
		CreateBullet();
		//PlaySound(sound[0]);
	}

	if (Player.yv > 0)
		Player.direction = DOWN;
	if (Player.yv < 0)
		Player.direction = UP;
	if (Player.xv > 0)
		Player.direction = RIGHT;
	if (Player.xv < 0)
		Player.direction = LEFT;
	
	//Player.x += Player.xv;
	//Player.y += Player.yv;

	Player.x += Player.xv;
	//enemyPos.y += enemyVel.y;
	int y = (Player.y + 30) / TILE_SIZE;
	int x = (Player.x + 12) / TILE_SIZE;
	int x2 = (Player.x + 20) / TILE_SIZE;
	int id[2];
	id[0] = Map[x][y];
	id[1] = Map[x2][y];
	if (id[0] == TILEWALL || id[1] == TILEWALL)
		Player.x -= Player.xv;

	Player.y += Player.yv;
	
	y =  (Player.y + 30) / TILE_SIZE;
	id[0] = Map[x][y];
	id[1] = Map[x2][y];
	if (id[0] == TILEWALL || id[1] == TILEWALL)
		Player.y -= Player.yv;

	x = (Player.x + 16) / TILE_SIZE;
	y = (Player.y + 30) / TILE_SIZE;
	switch(MapItem[x][y])
	{
		case AMMO:
			if (Player.ammo < 300)
			{
				Player.ammo += 50;
				if (Player.ammo > 300) Player.ammo = 300;
				MapItem[x][y] = EMPTY;
				PlaySound(sound[RELOAD]);
			}
			

			break;
		case COKE:
			if (Player.health < 100)
			{
				Player.health += 10;
				if (Player.health > 100) Player.health = 100;
				MapItem[x][y] = EMPTY;
				PlaySound(sound[DRINK]);
			}
			break;
	}

	switch(Player.direction)
	{
		case DOWN:
			Player.currFrame = 0;
			break;
		case UP:
			Player.currFrame = 3;
			break;
		case RIGHT:
			Player.currFrame = 6;
			break;
		case LEFT:
			Player.currFrame = 9;
			break;
	}
}

void DrawPlayer()
{
	g_spritePlayer.Draw(Player.currFrame, Player.x, Player.y);
}

void CreateBullet()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (!Bullets[i].active)
		{
			Bullets[i].active = true;
			Bullets[i].x = Player.x + 16 - 4;
			Bullets[i].y = Player.y + 16 - 4;
			switch(Player.direction)
			{
				case DOWN:
					Bullets[i].xv = 0;
					Bullets[i].yv = 8;
					break;
				case UP:
					Bullets[i].xv =  0;
					Bullets[i].yv = -8;
					break;
				case RIGHT:
					Bullets[i].xv = 8;
					Bullets[i].yv = 0;
					break;
				case LEFT:
					Bullets[i].xv = -8;
					Bullets[i].yv =  0;
					break;
			}
			Player.shotsfired++;
			Player.ammo--;
			PlaySound(sound[0]);
			break;
		}
		
	}
}

void InitBullets()
{
	for (int i = 0; i < MAX_BULLETS;  i++)
	{
		Bullets[i].active = false;
		Bullets[i].x  = Bullets[i].y  = 0;
		Bullets[i].xv = Bullets[i].yv = 0;
	}
}

void UpdateBullets()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (Bullets[i].active)
		{
			Bullets[i].x += Bullets[i].xv;
			Bullets[i].y += Bullets[i].yv;

			if (Bullets[i].x < 0 || Bullets[i].x+8 >= SCREEN_WIDTH ||
				Bullets[i].y < 0 || Bullets[i].y+8 >= MAP_HEIGHT * TILE_SIZE)
			{
				Bullets[i].active = false;
				
			}
			else
			{
				
			
				int x = (Bullets[i].x + 4)/TILE_SIZE;
				int y = (Bullets[i].y + 4)/TILE_SIZE;

				if (Map[x][y] == TILEWALL) Bullets[i].active = false;
				else
				{
					for (int j = 0; j < MAX_ENEMIES; j++)
					{
						ENEMY_INFO *Enemy = &Enemies[j];
						if (Enemy->state >= CHASE)
						{
							if (abs((Bullets[i].x+4) - (Enemy->x+16)) < 22 &&
								abs((Bullets[i].y+4) - (Enemy->y+16)) < 22)
							{
								Enemy->health -= rand()%20+20;
								Enemy->state = PAUSE;
								Bullets[i].active = false;
								Player.hits++;
								PlaySound(sound[HIT]);
								
								break;
							}
						}
					}
				}
			}
		}
	}
}

void DrawBullets()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (Bullets[i].active)
		{
			g_spriteBullet.Draw(0, Bullets[i].x, Bullets[i].y);
		}
	}
}

void InitEnemy(int index)
{
	
	Enemies[index].x       = Enemies[index].y       = 0;
	Enemies[index].xv      = Enemies[index].yv      = 0;
	Enemies[index].dirx    = Enemies[index].diry    = 0;
	Enemies[index].targetx = Enemies[index].targety = 0;
	Enemies[index].health    = 100;
	Enemies[index].state     = DEAD;
	Enemies[index].currFrame = 0;
	Enemies[index].counter   = 0;
	Enemies[index].threshold = 0;
	Enemies[index].newpath = true;
}

void InitEnemies()
{
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		Enemies[i].x       = Enemies[i].y       = 0;
		Enemies[i].xv      = Enemies[i].yv      = 0;
		Enemies[i].dirx    = Enemies[i].diry    = 0;
		Enemies[i].targetx = Enemies[i].targety = 0;
		Enemies[i].health    = 100;
		Enemies[i].state     = DEAD;
		Enemies[i].currFrame = 0;
		Enemies[i].counter   = 0;
		Enemies[i].threshold = 0;
		Enemies[i].newpath = true;
		Enemies[i].respawnTimer = 5;
	}
}

void UpdateEnemies(float dt)
{
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		ENEMY_INFO *Enemy = &Enemies[i];
		Enemy->xv = Enemy->yv = 0;
		bool moved = false;
		static int count = 0;
		if (Enemy->state == DEAD)
		{
			if (EnemyCount < MaxEnemies)
			{
				InitEnemy(i);
				Enemy->state = RESPAWN;
				Enemy->respawnTimer = 5;
				EnemyCount++;
			}
			
		}
		else if (Enemy->state == RESPAWN)
		{
			Enemy->respawnTimer -= dt;
			if (Enemy->respawnTimer <= 0)
			{
				int spot = rand()%(spawnPoints.size());
				Enemy->x = spawnPoints[spot].x;
				Enemy->y = spawnPoints[spot].y;
				Enemy->state = CHASE;
				
				
			}

			
		}
		else
		{
			int switchstate;
			switch(Enemy->state)
			{
				case CHASE:
				{
					if (Enemy->newpath)
					{
						int startx = (Enemy->x+16)/TILE_SIZE;
						int starty = (Enemy->y+30)/TILE_SIZE;
						int x, y;
						FindPath(startx, starty, ((int)Player.x+16)/TILE_SIZE, (Player.y+30)/TILE_SIZE, x, y);
						Enemy->targetx = x * TILE_SIZE + 16;
						Enemy->targety = y * TILE_SIZE + 25;

						Enemy->dirx = (x * TILE_SIZE+16) - (float)(Enemy->x + 16);
						Enemy->diry = (y * TILE_SIZE+25) - (float)(Enemy->y + 30);
						Enemy->newpath = false;
						
					}
		
					if (Enemy->targetx == Enemy->x + 16)
						Enemy->dirx = 0;
						
					if (Enemy->targety == Enemy->y + 30)
						Enemy->diry = 0;
		
					if (Enemy->dirx == 0 && Enemy->diry == 0) 
						Enemy->newpath = true;
					

					
					if (Enemy->dirx > 0)
					{
						Enemy->xv = 1;
						//moved = true;
					}
					else if (Enemy->dirx < 0)
					{
						Enemy->xv = -1;
						//moved = true;
					}
				
					if (Enemy->diry > 0)
					{
						Enemy->yv = 1;
						//moved = true;
					}
					else if (Enemy->diry < 0)
					{
						Enemy->yv = -1;
						//moved = true;
					}
					
					int temp = rand()%1000;
					if (temp > 900)
					{
						Enemy->state = EVADE;
					}
					if (temp < 90 && abs((Enemy->x + 16) - (Player.x + 16)) < 64 && 
						abs((Enemy->y + 16) - (Player.y + 16)) < 64)
					{
						Enemy->state = ATTACK;
						PlaySound(sound[ZOMBIE]);
					}
					

				}break;
				case EVADE:
					if (Enemy->x > Player.x)
						Enemy->xv = 1;
					if (Enemy->x < Player.x)
						Enemy->xv = -1;
					if (Enemy->y > Player.y)
						Enemy->yv = 1;
					if (Enemy->y < Player.y)
						Enemy->yv = -1;
					switchstate = rand()%100;
					if (switchstate > 10)
					{
						Enemy->state = CHASE;
						Enemy->newpath = true;
					}
					break;
				case ATTACK:
					if (Enemy->x > Player.x)
						Enemy->xv = -3;
					if (Enemy->x < Player.x)
						Enemy->xv = 3;
					if (Enemy->y < Player.y)
						Enemy->yv = 3;
					if (Enemy->y > Player.y)
						Enemy->yv = -3; 

					if (abs((Enemy->x + 16) - (Player.x + 16)) < 32 &&
						abs((Enemy->y + 16) - (Player.y + 16)) < 32)
					{
						Player.health --;
						if ( rand()%100 < 10) Enemy->state = EVADE;
					}

					switchstate = rand()%1000;
					if (switchstate > 800)
					{
						Enemy->state = PAUSE;
					}
					break;
				case PAUSE:
					switchstate = rand()%1000;
					if (switchstate > 930)
					{
						Enemy->state = CHASE;
						Enemy->newpath = true;
					}
					break;
			}
		}
		
		
		if (Enemy->yv > 0)
		{
			Enemy->direction = DOWN;
			moved = true;
		}
		if (Enemy->yv < 0)
		{
			Enemy->direction = UP;
			moved = true;
		}
		if (Enemy->xv > 0)
		{
			Enemy->direction = RIGHT;
			moved = true;
		}
		if (Enemy->xv < 0)
		{
			Enemy->direction = LEFT;
			moved = true;
		}
		
		//Enemy->x += Enemy->xv;
		//Enemy->y += Enemy->yv;

		Enemy->x += Enemy->xv;
		//enemyPos.y += enemyVel.y;
		int y = (Enemy->y + 30) / TILE_SIZE;
		int x = (Enemy->x + 12) / TILE_SIZE;
		int x2 = (Enemy->x + 20) / TILE_SIZE;
		int id[2];
		id[0] = Map[x][y];
		id[1] = Map[x2][y];
		if (id[0] == TILEWALL || id[1] == TILEWALL)
			Enemy->x -= Enemy->xv;

		Enemy->y += Enemy->yv;
		
		y =  (Enemy->y + 30) / TILE_SIZE;
		id[0] = Map[x][y];
		id[1] = Map[x2][y];
		if (id[0] == TILEWALL || id[1] == TILEWALL)
			Enemy->y -= Enemy->yv;

		if (Enemy->counter == 0)
		{
			Enemy->counter = 5;
			
		}
		else
		{
			Enemy->counter -= dt;
			if (Enemy->counter <= 0)
			{
				
				Enemy->currFrame++;
				Enemy->animate = true;
				
			}
		}

		
		switch(Enemy->direction)
		{
			case DOWN:
				if (moved)
				{
					if (Enemy->currFrame > 2)
						Enemy->currFrame = 1;
					else if (Enemy->currFrame < 1) Enemy->currFrame = 2;
				}
				else Enemy->currFrame = 0;
				break;
			case UP:
				if (moved)
				{
					if (Enemy->currFrame > 5)
						Enemy->currFrame = 4;
					else if (Enemy->currFrame < 4) Enemy->currFrame = 5;
				}
				else Enemy->currFrame = 3;
				break;
			case RIGHT:
				if (moved)
				{
					if (Enemy->currFrame > 8)
						Enemy->currFrame = 7;
					else Enemy->currFrame = 8;

				}
				else Enemy->currFrame = 6;
				
				break;
			case LEFT:
				if (moved)
				{
					if (Enemy->currFrame > 11)
						Enemy->currFrame = 10;
					else Enemy->currFrame = 11;
					
				}
				else Enemy->currFrame = 9;
				
				break;
		}
		

		//CHECK COLLISION With mine
		for (int j = 0; j < mines.size(); j++)
		{
			if (mines[j].hit)
			{
				if (abs((Enemy->x+16) - mines[j].pos.x) < 100 &&
					abs((Enemy->y+30) - mines[j].pos.y) < 100)
				{
					Enemy->state = DEAD;
					Player.kills++;
					Player.score += 10;
					mines[j].killCount++;
					EnemyCount--;
				}
				
				
				mines[j].timer -= dt;
				if (mines[j].timer < 0)
				{
					mines[j] = mines[mines.size()-1];
					mines.pop_back();
				}
				
			}
			else
			{
				if (abs((Enemy->x+16) - mines[j].pos.x) < 8 &&
					abs((Enemy->y+30) - mines[j].pos.y) < 8)
				{
					Enemy->state = DEAD;
					Player.kills++;
					Player.score += 5 * mines[j].killCount;
					mines[j].hit = true;
					mines[j].timer = 1;
					mines[j].killCount++;
					EnemyCount--;
				}
				
			}
		}

		if (Enemy->health <= 0)
		{
			Enemy->state = DEAD;
			Enemy->health = 100;
			EnemyCount--;
			count--;
			Player.kills++;
			Player.score += 25;
		}
	}
}

void DrawEnemies()
{
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		if (Enemies[i].state >= CHASE)
		{
			g_spriteZombie.Draw(Enemies[i].currFrame, Enemies[i].x, Enemies[i].y);
		}
	}
}

void InitMap()
{
	for (int x = 0; x < MAP_WIDTH; x++)
		for (int y = 0; y < MAP_HEIGHT; y++)
			Map[x][y] = rand()%2;
}

void DrawMap()
{
	for (int x = 0; x < MAP_WIDTH; x++)
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				g_spriteTiles.Draw(Map[x][y], x * TILE_SIZE, y * TILE_SIZE);
				
				g_spriteFlags.Draw(MapItem[x][y], x * TILE_SIZE, y * TILE_SIZE);
			}
}

void DrawDisplay()
{
	char minutes[3], seconds[3];
	sprintf(minutes, "%i", (int)gameTimer/60);
	sprintf(seconds, "%i", (int)gameTimer%60);
	if ((int)gameTimer%60 < 10)
		g_Font.Print(0, 500, D3DCOLOR_XRGB(0,255,0), "Timer %s:0%s", minutes, seconds);
	else
		g_Font.Print(0, 500, D3DCOLOR_XRGB(0,255,0), "Timer %s:%s", minutes, seconds);
	g_Font.Print(200, 500, D3DCOLOR_XRGB(0,255, 0), "Health = %i", Player.health);
	g_Font.Print(400, 500, D3DCOLOR_XRGB(0, 255,0), "Ammo = %i", Player.ammo);
}


void FindPath(int startx, int starty, int targetx, int targety, int &ox, int &oy)//find the path
{
	POINT ptStart;
	POINT ptEnd;
	POINT ptPath;
	ptStart.x=-1;
	ptEnd.x=-1;
	int x;
	int y;
	int nx;
	int ny;
	bool found;
	int lowvalue;
	//find the start
	ptStart.x = startx;
	ptStart.y = starty;
	
	//find the end
	ptEnd.x=targetx;
	ptEnd.y=targety;
	
	//if no start or end, exit function
	if(ptStart.x==-1 || ptEnd.x==-1) return;
	//fill out the path array
	for(x=0;x<MAP_WIDTH;x++)
	{
		for(y=0;y<MAP_HEIGHT;y++)
		{
			switch(Map[x][y])
			{
			case TILEFLOOR://place a 0 at the start
				{
					MapPath[x][y]=-1;
				}break;
			case TILEWALL://place a 999 at any blocks
				{
					MapPath[x][y]=999;
				}break;
			default://anything else, place a -1
				{
					MapPath[x][y]=-1;
				}break;
			}
		}
	}
	MapPath[ptStart.x][ptStart.y] = 0;
	//scan for pathable tiles
	do
	{
		//havent found one yet
		found=false;
		//scan the map
		for(x=0;x<MAP_WIDTH;x++)
		{
			for(y=0;y<MAP_HEIGHT;y++)
			{
				MapMark[x][y]=false;
				//make sure this is a "-1" square
				if(MapPath[x][y]==-1)
				{
					//scan the neighbors
					for(nx=x-1;nx<=x+1;nx++)
					{
						for(ny=y-1;ny<=y+1;ny++)
						{
							//make sure the neighbor is on the map
							if(nx>=0 && ny>=0 && (nx == x || ny == y) && nx<MAP_WIDTH && ny<MAP_HEIGHT && !(nx==x && ny==y))
							{
								//check against negatives and 999
								if(MapPath[nx][ny]>=0 && MapPath[nx][ny]!=999)
								{
									//mark the map
									MapMark[x][y]=true;
									//mark it as found
									found=true;
								}
							}
						}
					}
				}
			}
		}
		//now scan the marks
		for(x=0;x<MAP_WIDTH;x++)
		{
			for (y=0;y<MAP_HEIGHT;y++)
			{
				//if this square is marked
				if (MapMark[x][y])
				{
					//set low value very high
					lowvalue=999;
					//loop through neighbors
					for (nx=x-1;nx<=x+1;nx++)
					{
						for (ny=y-1;ny<=y+1;ny++)
						{
							//make sure the neighbor is on the map
							if (nx>=0 && ny>=0 &&  (nx == x || ny == y) && nx<MAP_WIDTH && ny<MAP_HEIGHT)
							{
								if (MapPath[nx][ny] >= 0)//must be a non-negative value
								{
									//assign the value if it is lower
									if (MapPath[nx][ny] < lowvalue) lowvalue = MapPath[nx][ny];
								}
							}
						}
					}
					//assign the value to the path map
					MapPath[x][y]=lowvalue+1;
				}
			}
		}
	}
	while(found);
	//done with pathfinding
	//check to see that ptEnd has found a path
	if(MapPath[ptEnd.x][ptEnd.y]!=-1)
	{
		//start the path
		ptPath=ptEnd;
		//take the value from the map
		lowvalue=MapPath[ptEnd.x][ptEnd.y];
		while(lowvalue>0)
		{
			found=false;
			do
			{
				do
				{
					//pick a random neighbor
					nx=rand()%3-1;
					ny=rand()%3-1;
				}while((nx && ny) || (nx==0 && ny==0) || (ptPath.x+nx)<0 || (ptPath.x+nx)>=MAP_WIDTH || (ptPath.y+ny)<0 || (ptPath.y+ny)>=MAP_HEIGHT);
				//check to see if the value is lower
				if(MapPath[ptPath.x+nx][ptPath.y+ny]<lowvalue)
				{
					//found!
					found=true;
					//set tile to path tile
					//Map[ptPath.x][ptPath.y]=TILEPATH;
					char buffer[256];
					
					//move the path
					ptPath.x+=nx;
					ptPath.y+=ny;
					lowvalue=MapPath[ptPath.x][ptPath.y];
				}
			}
			while(!found);
		}
		//get output for next path point
		if (startx == targetx && starty == targety)
		{
			ox = ptPath.x;
			oy = ptPath.y;
		}
		else
		{
		ox = ptPath.x-nx;
		oy = ptPath.y-ny;
		}
		
	}
	
}

int InitSoundSystem()
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&pfmodSystem);
	if (result != FMOD_OK)
		return(0);

	result = pfmodSystem->init(200, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
		return(0);

	return(1);
}

void KillSoundSystem()
{
	pfmodSystem->release();
}

int LoadSound(char filename[], FMOD::Sound **sound)
{
	if (pfmodSystem->createSound(filename, FMOD_DEFAULT, 0, sound) != FMOD_OK)
		return(0);
	return(1);
}

void PlaySound(FMOD::Sound *sound)
{
	FMOD::Channel *channel;
	pfmodSystem->playSound(FMOD_CHANNEL_FREE, sound, false, 0);
	//channel->setVolume(1.0);
}

void UpdateSoundSystem()
{
	pfmodSystem->update();
}

void SaveMap()
{
	fstream outfile("level.dat", ios_base::out | ios_base::binary | ios_base::trunc);
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{	
			outfile << Map[x][y] << " ";
			outfile << MapItem[x][y] << " ";
		}
	}
	outfile.close();
}

void LoadMap()
{
	fstream infile("level.dat", ios_base::in | ios_base::binary);
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			infile >> Map[x][y];
			infile >> MapItem[x][y];
			switch(MapItem[x][y])
			{
				case EMPTY:
					MapItem[x][y] = EMPTY;
					break;
				case START:
					startx = x;
					starty = y;
					Player.x = x * TILE_SIZE;
					Player.y = y * TILE_SIZE;
					break;
				case ENEMY_SPAWN:
					SPAWN_POINTS sp;
					sp.x = x * TILE_SIZE;
					sp.y = y * TILE_SIZE;
					spawnPoints.push_back(sp);
					break;
			}
		}
	}
	infile.close();
}

void DrawMines()
{
	
	for (int i = 0; i < mines.size(); i++)
	{
		//g_spriteBullet.SetColor(D3DCOLOR_XRGB(255, 0, 0));
		g_spriteBullet.Draw(0, mines[i].pos.x, mines[i].pos.y);
	}
}