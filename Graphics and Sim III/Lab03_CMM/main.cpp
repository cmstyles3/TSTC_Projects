// Collin Munson

// NEW-----------------------------------------------//
// Revised for Lab 3                                ///
// Now the map is loaded and saved using an x file  ///
// Class: Graphics and Simulation 3                 ///
// Date: May 22, 2008                               ///
// --------------------------------------------------//

// Graphics and Simulation II
// Final Project
// December 4, 2007

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

#include "resource.h"
#include "d3dbase.h"
#include "input.h"

#include "xFileSave.h"
#include "xFileLoad.h"


using namespace std;
// Defines
//---------------------------------------------------------------

// NEW
// GUID FOR MAP
// {96D531CA-2813-4b45-A45D-393B7F921F89}
DEFINE_GUID(GUID_MAP, 
0x96d531ca, 0x2813, 0x4b45, 0xa4, 0x5d, 0x39, 0x3b, 0x7f, 0x92, 0x1f, 0x89);


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MURDER Release

#define KILL(x) if (x) { x->MURDER(); x = 0; }

#define CLASSNAME "Final Project"
#define WINDOWNAME "Shoot and Pop"

#define MAX_TEXTURES 32
#define SPEED 90.0f

#define CELL_SIZE 20.0f

#define SKYBOX_SIZE 1300.0f

#define MAP_X 18
#define MAP_Z 18

#define GRASS 0
#define TILE  1
#define WALL  2

#define EMPTY 0
#define AMMO 1
#define START 2

enum GAMESTATE{INTRO, MENU, SETUPGAME, PLAYGAME, RESETMENU, GAMEOVER, EDITMAP};

#define NUM_BULLETS 50
struct
{
	float x, y, z;
	float xv, yv, zv;
	int state;
	float life;
}Bullets[NUM_BULLETS];

#define NUM_ENEMIES 25
struct
{
	float x, y, z;
	float xv, yv, zv;
	int state;
	int score_amount;
}Enemies[NUM_ENEMIES];

struct
{
	int id;
	int flag;
	//bool walkable;
}map[MAP_Z][MAP_X];


// NEW STRUCTURE FOR X FILE
struct MAP_INFO
{
	DWORD id[MAP_Z][MAP_X];
	DWORD flag[MAP_Z][MAP_X];
};


// Function Prototypes
//------------------------------------------------------------------------------------------------------------------------
int  GameInit();
int  GameMain(float);
void GameMurder();

//Mesh Creations Functions
void CreateMapMesh(LPDIRECT3DDEVICE9 device, float width, float height, float depth, LPD3DXMESH* mesh);
void CreateAmmoBoxMesh(LPDIRECT3DDEVICE9 device, float width, float height, float depth, LPD3DXMESH* mesh);
void CreateFenceMesh(LPDIRECT3DDEVICE9 device, float width, float height, LPD3DXMESH *mesh);
void CreateSkyBoxMesh(LPDIRECT3DDEVICE9 device, float spacing, LPD3DXMESH *mesh);

void DrawSkyBox(float x, float y, float z);

void DrawMapCell(int id, float x, float y, float z);
void DrawMap();

void MovePlayer();

void MoveBullets(float deltatime);
void DrawBullets();

void MoveEnemies(float deltatime);
void DrawEnemies();

void ProcessMenuControls();
void ProcessMenuLogic(float deltatime);
void DrawMenu();

void SetupGame();

void ProcessGameControls(float deltatime);
void ProcessGameLogic(float deltatime);
void DrawGame();
void DrawGameHUD();

void DrawGameOver();

void ProcessEditorControls();
void DrawEditor();

void LoadMap();
void SaveMap();

void DrawMirror();
void RenderMirror();
void RenderShadow();
//------------------------------------------------------------------------------------------------------------------------


// Globals
//-----------------------------------------------------------------------------------------------------------------------------

GAMESTATE gamestate = MENU;

LPD3DXFONT font = 0;

LPDIRECT3DTEXTURE9 tex[MAX_TEXTURES] = {0};
LPDIRECT3DTEXTURE9 skybox_tex[6];

LPD3DXMESH teapot, cell_block, skybox, fence, bullet, enemy, ammo, mirror;

D3DLIGHT9 flashlight, sun_on_walls, sun_on_ground, muzzle_flash;

LPD3DXSPRITE sprite;

Input input;


//camera
float x_pos = 0.0f, y_pos = 22.0f, z_pos = 0.0f;
float x_dir = 0.0f, y_dir = 0.0f, z_dir = 0.0f;
float x_vel = 0.0f, z_vel = 0.0f;
float theta = 0.0f, phi = 0.0f;

float cursor_x = SCREEN_WIDTH/2, cursor_y = SCREEN_HEIGHT/2;

float ammo_rot = 0.0f;

int ammunition = 0;
float gametimer = 0;
int score = 0, num_escaped = 0, num_shots_fired = 0, num_shots_hit = 0;
float hit_per = 0.0f;

HINSTANCE hinstance_app;
HWND hwnd_app;
DWORD starttime = 0, elapsed_time;

char Buffer[256];
fstream fin, fout;

LPD3DXFILE xFile = 0;
LPD3DXFILESAVEOBJECT xFileSave=NULL;
LPDIRECTXFILESAVEOBJECT xFileSave3 = 0;
LPD3DXFILESAVEDATA xFileSave2 = 0;

//-------------------------------------------------------------------------------------------------------------------------------

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
	
	hinstance_app = hinstance;
	MessageBox(0, "Consult README.txt for controls", "INSTRUCTIONS", 0);
	srand((unsigned int)GetTickCount());
	if (!GameInit())
	{
		MessageBox(0, "GameInit() - FAILED", 0, 0);
		return(0);
	}
//	D3DXFileCreate(&xFile);
//
//	char *szTemplate = "xof 0303txt 0032\
//					    template Test \
//						{ \
//							<96D531CA-2813-4b45-A45D-393B7F921F89> \
//							array DWORD id[18][18]; \
//							array DWORD flag[18][18]; \
//						}";
//						
//	if (FAILED(xFile->RegisterTemplates(szTemplate, strlen(szTemplate))))
//	{
//		MessageBox(0, "register templates failed", 0, 0);
//		return(0);
//	}
//	if (FAILED(xFile->CreateSaveObject("stuff.x", D3DXF_FILESAVE_TOFILE, D3DXF_FILEFORMAT_BINARY , &xFileSave)))
//	{
//		MessageBox(0, "CreateSaveObject Failed", 0, 0);
//		return(0);
//	}
//	
//	LPDIRECTXFILEDATA Data;
//	
//	Test test;
//	ZeroMemory(&test, sizeof(Test));
//	//test.num = 16;
//	for (int z = 0; z < MAP_Z; z++)
//	{
//		for (int x = 0; x < MAP_X; x++)
//		{
//			test.id[z][x] = map[z][x].id;
//			test.flag[z][x] = map[z][x].id;
//		}
//	}
//	
//
////	xFileSave->GetFile(&xFile);
//
//	if (FAILED(xFileSave->AddDataObject(GUID_TEST, "test", 0, sizeof(Test), &test , &xFileSave2)))
//		return(0);
//	
//
//	
//
//	if (FAILED(xFileSave->Save()))
//		return(0);
//	xFileSave->Release();
//	xFile->Release();
//	return(0);
	EnterMsgLoop(GameMain);

	GameMurder();
	
	return(0);
}

int GameInit()
{
	// Initialize Direct3D -------------------------------------------------------------------
	if (!InitD3D(CLASSNAME, WINDOWNAME, SCREEN_WIDTH, SCREEN_HEIGHT, false, D3DDEVTYPE_HAL))
	{	
		MessageBox(0,"InitD3D() - FAILED", 0, 0);
		return(0);
	}
	
	// Initialize DirectInput ----------------------------------------------------------------
	if (!input.initDI())
	{
		MessageBox(0, "could not initialize direct input", 0, 0);
		return(0);
	}

	// Initialize Keyboard with DirectInput ---------------------------------------------------
	if (!input.initKB())
	{
		MessageBox(0, "could not initialize keyboard", 0, 0);
		return(0);
	}

	// Initialize Mouse with DirectInput ---------------------------------------------------
	if (!input.initMouse())
	{
		MessageBox(0, "could not initialize mouse", 0, 0);
		return(0);
	}

	// SETUP FONT FOR OUTPUTING TEXT -------------------------------------------------------
	D3DXFONT_DESC FontStruct; 
	FontStruct.Height = 22; 
	FontStruct.Width = 11; 
	FontStruct.Weight = 500; 
	FontStruct.MipLevels = 0; 
	FontStruct.Italic = false; 
	FontStruct.CharSet = DEFAULT_CHARSET; 
	FontStruct.OutputPrecision = OUT_DEFAULT_PRECIS; 
	FontStruct.Quality = ANTIALIASED_QUALITY; 
	FontStruct.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE; 
	strcpy(FontStruct.FaceName, "Arial");  
	D3DXCreateFontIndirect(Device, &FontStruct, &font);
	

	// CREATE MESHES -----------------------------------------------------------------------
	D3DXCreateTeapot(Device, &teapot, 0);

	D3DXCreateSphere(Device, 3.0f, 5, 5, &bullet, 0);

	CreateMapMesh(Device, CELL_SIZE, CELL_SIZE * 2.0f, CELL_SIZE, &cell_block);
	CreateAmmoBoxMesh(Device, 5.0f, 5.0f, 5.0f, &ammo);

	CreateSkyBoxMesh(Device, SKYBOX_SIZE, &skybox);

	CreateFenceMesh(Device, 650.0f, 20.0f, &fence);

	D3DXCreateSphere(Device, 10.0f, 8, 8, &enemy, 0);
	enemy->CloneMeshFVF(0,D3DFVF_CUSTOM,Device,&enemy);
		
	Vertex* v;
	enemy->LockVertexBuffer(0, (void**)&v);
	
	for(int i = 0; i < enemy->GetNumVertices(); i++)
	{
		v[i]._v = -v[i]._ny / 2 + 0.5f;
		v[i]._u = v[i]._nx / 2 + 0.5f;
	}
	enemy->UnlockVertexBuffer();
	
	// CREATE SPRITE -----------------------------------------------------------------------
	D3DXCreateSprite(Device, &sprite);

	// LOAD TEXTURES ------------------------------------------------------------------------
	D3DXCreateTextureFromFile(Device, "Textures\\desert_evening_north.bmp", &skybox_tex[0]);
	D3DXCreateTextureFromFile(Device, "Textures\\desert_evening_west.bmp", &skybox_tex[1]);
	D3DXCreateTextureFromFile(Device, "Textures\\desert_evening_south.bmp", &skybox_tex[2]);
	D3DXCreateTextureFromFile(Device, "Textures\\desert_evening_east.bmp", &skybox_tex[3]);
	D3DXCreateTextureFromFile(Device, "Textures\\desert_evening_up.bmp", &skybox_tex[4]);
	D3DXCreateTextureFromFile(Device, "Textures\\desert_evening_down.bmp", &skybox_tex[5]);

	D3DXCreateTextureFromFile(Device, "Textures\\floor.bmp", &tex[0]);
	D3DXCreateTextureFromFile(Device, "Textures\\wall.bmp", &tex[1]);
	D3DXCreateTextureFromFile(Device, "Textures\\barbedwire.dds", &tex[2]);
	D3DXCreateTextureFromFile(Device, "Textures\\grass.bmp", &tex[3]);
	D3DXCreateTextureFromFile(Device, "Textures\\enemy.bmp", &tex[4]);
	D3DXCreateTextureFromFile(Device, "Textures\\crosshair.dds", &tex[5]);
	D3DXCreateTextureFromFile(Device, "Textures\\menu.dds", &tex[6]);
	D3DXCreateTextureFromFile(Device, "Textures\\cursor.dds", &tex[7]);
	D3DXCreateTextureFromFile(Device, "Textures\\grass-small.bmp", &tex[8]);
	D3DXCreateTextureFromFile(Device, "Textures\\floor-small.bmp", &tex[9]);
	D3DXCreateTextureFromFile(Device, "Textures\\wall-small.bmp", &tex[10]);
	D3DXCreateTextureFromFile(Device, "Textures\\ammo.bmp", &tex[11]);
	D3DXCreateTextureFromFile(Device, "Textures\\ammo-small.dds", &tex[12]);

	// SET TEXTURE FILTERS -------------------------------------------------------------------
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// Use alpha channel in Textures for alpha -----------------------------------------------
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// set blending factors so that alpha component determines transparency
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	// SETUP LIGHTS -------------------------------------------------
	D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);
	D3DXCOLOR c(WHITE);

	flashlight = InitSpotLight(&pos,&dir, &c);
	Device->SetLight(0, &flashlight);

	sun_on_walls = InitPointLight(&pos, &c);
	Device->SetLight(1, &sun_on_walls);
	Device->LightEnable(1, true);

	sun_on_ground = InitPointLight(&pos, &c);
	Device->SetLight(2, &sun_on_ground);
	Device->LightEnable(2, true);
	sun_on_ground.Range = 1500.0f;

	c.r = 0.8f;
	c.g = 0.5f;
	c.b = 0.2f;
	muzzle_flash = InitPointLight(&pos, &c);
	Device->SetLight(3, &muzzle_flash);
	muzzle_flash.Range = 40.0f;
	

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);


	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.35f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1300.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	
	Device->SetRenderState(D3DRS_LIGHTING, true);

	LoadMap();
	
	return(1);
}

//kill everything, make sure there are absolutely no survivors
void GameMurder()
{
	for (int i = 0; i < MAX_TEXTURES; i++)
		KILL(tex[i]);

	for (int i = 0; i < 6; i++)
		KILL(skybox_tex[i]);

	KILL(sprite);
	KILL(ammo);
	KILL(enemy);
	KILL(fence);
	KILL(teapot);
	KILL(font);
	KILL(cell_block);
	KILL(skybox);
	
	//kill direct input
	input.killMouse();
	input.killKB();
	input.killDI();

	//kill device
	KillD3D();
}

int GameMain(float deltatime)
{
	starttime = GetTickCount();
	if (Device)
	{
		
		input.readMouse();
		input.readKB();
		if (kb_state[DIK_F1])
		{
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		}
		if (kb_state[DIK_F2])
		{
			Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
		switch(gamestate)
		{
			case MENU:
			{
				ProcessMenuControls();
				ProcessMenuLogic(deltatime);
			}break;
			case SETUPGAME:
			{
				SetupGame();
				gamestate = PLAYGAME;
			}break;
			case PLAYGAME:
			{
				ProcessGameControls(deltatime);
				ProcessGameLogic(deltatime);
			}break;
			case RESETMENU:
			{
				gamestate = MENU;
				Device->LightEnable(0, false);
				Device->LightEnable(3, false);
				Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
				ammo_rot = 0;

			}break;
			case GAMEOVER:
			{
				if (kb_state[DIK_ESCAPE])
					gamestate = RESETMENU;
			}break;
			case EDITMAP:
			{
				ProcessEditorControls();
			}break;
		}
		
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
		Device->BeginScene();
		switch(gamestate)
		{
			
			case MENU:
				DrawMenu();
				break;
			case GAMEOVER:
			case PLAYGAME:
				DrawGame();
				if (gamestate == GAMEOVER)
					DrawGameOver();
				else DrawGameHUD();
				break;
			case EDITMAP:
				DrawEditor();
				break;

		}
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
		
	}
	elapsed_time = GetTickCount() - starttime;
	return(1);
}


void CreateMapMesh(LPDIRECT3DDEVICE9 device, float width, float height, float depth, LPD3DXMESH *mesh)
{
	D3DXCreateMeshFVF(12, 24, 0, D3DFVF_CUSTOM, device, mesh);
	
	Vertex *v;
	(*mesh)->LockVertexBuffer(0,(void**)&v);
	//front face
	v[0] = Vertex(-width/2,0.0f,-depth/2, 0.0f, 0.0f,-1.0f, 0.0f, 2.0f);
	v[1] = Vertex(-width/2, height,-depth/2, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f);
	v[2] = Vertex( width/2, height,-depth/2, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f);
	v[3] = Vertex( width/2,0.0f,-depth/2, 0.0f, 0.0f,-1.0f, 1.0f, 2.0f);

	//right face
	v[4] = Vertex( width/2,  0.0f, -depth/2, 1.0f, 0.0f, 0.0f, 0.0f, 2.0f);
	v[5] = Vertex( width/2, height,-depth/2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[6] = Vertex( width/2, height, depth/2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[7] = Vertex( width/2, 0.0f, depth/2,1.0f, 0.0f, 0.0f, 1.0f, 2.0f);

	//back face
	v[8]  = Vertex( width/2,0.0f, depth/2, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f);
	v[9]  = Vertex( width/2, height, depth/2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[10] = Vertex(-width/2, height, depth/2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-width/2,0.0f, depth/2, 0.0f, 0.0f, 1.0f, 1.0f, 2.0f);

	//left face
	v[12] = Vertex(-width/2,0.0f, depth/2,-1.0f, 0.0f, 0.0f, 0.0f, 2.0f);
	v[13] = Vertex(-width/2, height, depth/2,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[14] = Vertex(-width/2, height,-depth/2,-1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[15] = Vertex(-width/2,0.0f,-depth/2,-1.0f, 0.0f, 0.0f, 1.0f, 2.0f);

	//top face
	v[16] = Vertex(-width/2, height,-depth/2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-width/2, height, depth/2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex( width/2, height, depth/2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex( width/2, height,-depth/2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	//bottom face
	v[20] = Vertex(-width/2, 0.0f, -depth/2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[21] = Vertex(-width/2, 0.0f,  depth/2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex( width/2, 0.0f,  depth/2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[23] = Vertex( width/2, 0.0f, -depth/2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	(*mesh)->UnlockVertexBuffer();

	WORD *i;
	(*mesh)->LockIndexBuffer(0,(void**)&i);
	for (int j = 0, k = 0; j < 36; j += 6, k += 4)
	{
		i[j + 0] = k; i[j + 1] = k + 1; i[j + 2] = k + 2; //first tri of face
		i[j + 3] = k; i[j + 4] = k + 2; i[j + 5] = k + 3; //second tri of face
	}
	(*mesh)->UnlockIndexBuffer();

	DWORD* a;
	(*mesh)->LockAttributeBuffer(0, &a);
	//left wall
	for (int i = 0; i < 10; i++)
		a[i] =	WALL;
	for (int i = 10; i < 12; i++)
		a[i] = TILE;
	

	(*mesh)->UnlockAttributeBuffer();
}

void CreateAmmoBoxMesh(LPDIRECT3DDEVICE9 device, float width, float height, float depth, LPD3DXMESH *mesh)
{
	D3DXCreateMeshFVF(12, 24, 0, D3DFVF_CUSTOM, device, mesh);
	
	Vertex *v;
	(*mesh)->LockVertexBuffer(0,(void**)&v);
	//front face
	v[0] = Vertex(-width/2,0.0f,-depth/2, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-width/2, height,-depth/2, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f);
	v[2] = Vertex( width/2, height,-depth/2, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f);
	v[3] = Vertex( width/2,0.0f,-depth/2, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f);

	//right face
	v[4] = Vertex( width/2,  0.0f, -depth/2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[5] = Vertex( width/2, height,-depth/2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[6] = Vertex( width/2, height, depth/2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[7] = Vertex( width/2, 0.0f, depth/2,1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//back face
	v[8]  = Vertex( width/2,0.0f, depth/2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[9]  = Vertex( width/2, height, depth/2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[10] = Vertex(-width/2, height, depth/2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-width/2,0.0f, depth/2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	//left face
	v[12] = Vertex(-width/2,0.0f, depth/2,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[13] = Vertex(-width/2, height, depth/2,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[14] = Vertex(-width/2, height,-depth/2,-1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[15] = Vertex(-width/2,0.0f,-depth/2,-1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//top face
	v[16] = Vertex(-width/2, height,-depth/2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-width/2, height, depth/2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex( width/2, height, depth/2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex( width/2, height,-depth/2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	//bottom face
	
	v[20] = Vertex(-width/2,0.0f, depth/2, 0.0f,-1.0f, 0.0f, 0.0f, 1.0f);
	v[21] = Vertex(-width/2,0.0f,-depth/2, 0.0f,-1.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex( width/2,0.0f,-depth/2, 0.0f,-1.0f, 0.0f, 1.0f, 0.0f);
	v[23] = Vertex( width/2,0.0f, depth/2, 0.0f,-1.0f, 0.0f, 1.0f, 1.0f);
	(*mesh)->UnlockVertexBuffer();

	WORD *i;
	(*mesh)->LockIndexBuffer(0,(void**)&i);
	for (int j = 0, k = 0; j < 36; j += 6, k += 4)
	{
		i[j + 0] = k; i[j + 1] = k + 1; i[j + 2] = k + 2; //first tri of face
		i[j + 3] = k; i[j + 4] = k + 2; i[j + 5] = k + 3; //second tri of face
	}
	(*mesh)->UnlockIndexBuffer();

	DWORD* a;
	(*mesh)->LockAttributeBuffer(0, &a);
	//left wall
	for (int i = 0; i < 10; i++)
		a[i] =	WALL;
	for (int i = 10; i < 12; i++)
		a[i] = TILE;
	

	(*mesh)->UnlockAttributeBuffer();
}

void CreateFenceMesh(LPDIRECT3DDEVICE9 device, float width, float height, LPD3DXMESH *mesh)
{
	D3DXCreateMeshFVF(2, 4, 0, D3DFVF_CUSTOM, device, mesh);
	
	Vertex *v;
	(*mesh)->LockVertexBuffer(0,(void**)&v);
	//front face
	v[0] = Vertex(-width/2,-height/2, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-width/2, height/2, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f);
	v[2] = Vertex( width/2, height/2, 0.0f, 0.0f, 0.0f,-1.0f, 32.0f, 0.0f);
	v[3] = Vertex( width/2,-height/2, 0.0f, 0.0f, 0.0f,-1.0f, 32.0f, 1.0f);
	(*mesh)->UnlockVertexBuffer();

	WORD *i;
	(*mesh)->LockIndexBuffer(0,(void**)&i);
	
	i[0] = 0; i[1] = 1; i[2] = 2; //first tri of face
	i[3] = 0; i[4] = 2; i[5] = 3; //second tri of face
	
	(*mesh)->UnlockIndexBuffer();
}

void CreateSkyBoxMesh(LPDIRECT3DDEVICE9 device, float spacing, LPD3DXMESH *mesh)
{
	D3DXCreateMeshFVF(12, 24, D3DXMESH_MANAGED, D3DFVF_CUSTOM, Device, mesh);

	Vertex* v;
	(*mesh)->LockVertexBuffer(0, (void**)&v);
	//front wall
	v[0] = Vertex(-spacing/2, -spacing/2, spacing/2, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-spacing/2,  spacing/2, spacing/2, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex( spacing/2,  spacing/2, spacing/2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = Vertex( spacing/2, -spacing/2, spacing/2, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	//left wall
	v[4] = Vertex(-spacing/2, -spacing/2, -spacing/2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[5] = Vertex(-spacing/2,  spacing/2, -spacing/2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[6] = Vertex(-spacing/2,  spacing/2,  spacing/2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[7] = Vertex(-spacing/2, -spacing/2,  spacing/2, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//back wall
	v[8]  = Vertex( spacing/2, -spacing/2, -spacing/2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[9]  = Vertex( spacing/2,  spacing/2, -spacing/2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[10] = Vertex(-spacing/2,  spacing/2, -spacing/2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-spacing/2, -spacing/2, -spacing/2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	//right wall
	v[12] = Vertex(spacing/2,  -spacing/2,  spacing/2, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[13] = Vertex(spacing/2,   spacing/2,  spacing/2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[14] = Vertex(spacing/2,   spacing/2, -spacing/2, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[15] = Vertex(spacing/2,  -spacing/2, -spacing/2, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//ceiling
	v[16] = Vertex(-spacing/2,  spacing/2,  spacing/2, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-spacing/2,  spacing/2, -spacing/2, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex( spacing/2,  spacing/2, -spacing/2, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex( spacing/2,  spacing/2,  spacing/2, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);

	//floor
	v[20] = Vertex(-spacing/2, -spacing/2, -spacing/2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[21] = Vertex(-spacing/2, -spacing/2,  spacing/2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex( spacing/2, -spacing/2,  spacing/2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[23] = Vertex( spacing/2, -spacing/2, -spacing/2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	(*mesh)->UnlockVertexBuffer();

	WORD* i;
	(*mesh)->LockIndexBuffer(0, (void**)&i);
	for (int j = 0, k = 0; j < 24; j += 4, k += 6)
	{
		i[k]   = j; i[k+1] = j+1; i[k+2] = j+2;
		i[k+3] = j; i[k+4] = j+2; i[k+5] = j+3;
	}
	(*mesh)->UnlockIndexBuffer();

	DWORD* a;
	(*mesh)->LockAttributeBuffer(0, &a);
	//left wall
	for (int i = 0, j = 0 ; i < 12; i += 2, j++)
		a[i] = a[i+1] = j;
	

	(*mesh)->UnlockAttributeBuffer();
}

void DrawSkyBox(float x, float y, float z)
{
	D3DXMATRIX T;

	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	Device->SetRenderState(D3DRS_LIGHTING, false);
	D3DXMatrixTranslation(&T, x, y, z);
	Device->SetTransform(D3DTS_WORLD, &T);
	for (int i = 0; i < 6; i++)
	{
		Device->SetTexture(0, skybox_tex[i]);
		skybox->DrawSubset(i);
	}

	D3DXMatrixTranslation(&T, -20.0f, 0.0f, 10.0f);
	Device->SetTransform(D3DTS_WORLD, &T);
	Device->SetRenderState(D3DRS_LIGHTING, true);
}

void DrawMapCell(int id, int flag, float x, float y, float z)
{
	D3DXMATRIX T;
	if (id == WALL || id == TILE)
	{
		D3DXMatrixTranslation(&T, x, y, z);
		Device->SetTransform(D3DTS_WORLD, &T);
		Device->SetTexture(0, tex[id - 1]);
		cell_block->DrawSubset(id);
	}
	if (flag == AMMO)
	{
		D3DXMATRIX R;
		D3DXMatrixRotationY(&R, ammo_rot);
		D3DXMatrixTranslation(&T, x, y + 5.0f, z);
		Device->SetTransform(D3DTS_WORLD, &(R*T));
		Device->SetTexture(0, tex[11]);
		ammo->DrawSubset(1);
		ammo->DrawSubset(2);
	}
}


void DrawMap()
{
	D3DXMATRIX T;
	Device->LightEnable(1, false);
	Device->LightEnable(2, true);
	for (int z = 0; z < 50; z++)
	{
		for(int x = 0; x < 50; x++)
		{
			D3DXMatrixTranslation(&T, (x*CELL_SIZE) - 25 * CELL_SIZE, -0.1f, (z*CELL_SIZE) - 25 * CELL_SIZE);
			Device->SetTransform(D3DTS_WORLD, &T);
			Device->SetTexture(0, tex[3]);
			cell_block->DrawSubset(1);
		}
	}
	Device->LightEnable(2, false);
	Device->LightEnable(1, true);
	for (int z = 0; z < MAP_Z; z++)
	{
		for (int x = 0; x < MAP_X; x++)
		{
			DrawMapCell(map[z][x].id, map[z][x].flag, (x * CELL_SIZE) - CELL_SIZE * (MAP_X/2) , 0.0f, (z * -CELL_SIZE) + CELL_SIZE * (MAP_Z/2));
		}
	}
}

void MovePlayer()
{
	//advance the movement of camera position by velocities obtained from user input
	x_pos += x_vel;
	z_pos += z_vel;

	if (x_pos <= -320.0f || x_pos >= 320.0f)
		x_pos -= x_vel;
	if (z_pos <= -320.0f || z_pos >= 320.0f)
		z_pos -= z_vel;

	
	float x_index = (( x_pos+(CELL_SIZE/2 + CELL_SIZE * MAP_X/2))/CELL_SIZE);
	float z_index = ((-z_pos+(CELL_SIZE/2 + CELL_SIZE * MAP_Z/2))/CELL_SIZE);

	if (x_index >= 0 && x_index < MAP_X && z_index >= 0 && z_index < MAP_Z)
	{
		if (map[(int)z_index][(int)x_index].flag == AMMO && ammunition < 100)
		{
			ammunition += 30;
			if (ammunition > 100) ammunition = 100;
			PlaySound(MAKEINTRESOURCE(AMMO_ID), hinstance_app, SND_ASYNC | SND_RESOURCE);
			map[(int)z_index][(int)x_index].flag = EMPTY;
		}
	}

	float temp_x1 = x_pos - 5.0f;
	float temp_x2 = x_pos + 5.0f;
	
	float temp_z1 = z_pos - 5.0f;
	float temp_z2 = z_pos + 5.0f;

	temp_x1 -= x_vel;
	temp_x2 -= x_vel;
	
	float x_index1 = ((temp_x1+(CELL_SIZE/2 + CELL_SIZE * MAP_X/2))/CELL_SIZE);
	float x_index2 = ((temp_x2+(CELL_SIZE/2 + CELL_SIZE * MAP_X/2))/CELL_SIZE);
	
	float z_index1 = ((-temp_z1+(CELL_SIZE/2 + CELL_SIZE * MAP_Z/2))/CELL_SIZE);
	float z_index2 = ((-temp_z2+(CELL_SIZE/2 + CELL_SIZE * MAP_Z/2))/CELL_SIZE);

	bool z_collision = false;
	bool x_collision = false;

	if (x_index1 >= 0 && x_index1 < MAP_X)
	{
		if (z_index1 >= 0 && z_index1 < MAP_Z)
		{
			if (map[(int)z_index1][(int)x_index1].id == WALL)
				z_collision = true;
		}
		if (z_index2 >= 0 && z_index2 < MAP_Z)
		{
			if (map[(int)z_index2][(int)x_index1].id == WALL)
				z_collision = true;
		}
	}
	if (x_index2 >= 0 && x_index2 < MAP_X)
	{
		if (z_index1 >= 0 && z_index1 < MAP_Z)
		{
			if (map[(int)z_index1][(int)x_index2].id == WALL)
				z_collision = true;
		}
		if (z_index2 >= 0 && z_index2 < MAP_Z)
		{
			if (map[(int)z_index2][(int)x_index2].id == WALL)
				z_collision = true;
		}
	}
	if (z_collision) z_pos -= z_vel;
	
	temp_x1 += x_vel;
	temp_x2 += x_vel;
	
	temp_z1 -= z_vel;
	temp_z2 -= z_vel;

	x_index1 = ((temp_x1+(CELL_SIZE/2 + CELL_SIZE * MAP_X/2))/CELL_SIZE);
	x_index2 = ((temp_x2+(CELL_SIZE/2 + CELL_SIZE * MAP_X/2))/CELL_SIZE);
	
	z_index1 = ((-temp_z1+(CELL_SIZE/2 + CELL_SIZE * MAP_Z/2))/CELL_SIZE);
	z_index2 = ((-temp_z2+(CELL_SIZE/2 + CELL_SIZE * MAP_Z/2))/CELL_SIZE);

	if (x_index1 >= 0 && x_index1 < MAP_X)
	{
		if (z_index1 >= 0 && z_index1 < MAP_Z)
		{
			if (map[(int)z_index1][(int)x_index1].id == WALL)
				x_collision = true;
		}
		if (z_index2 >= 0 && z_index2 < MAP_Z)
		{
			if (map[(int)z_index2][(int)x_index1].id == WALL)
				x_collision = true;
		}
	}
	if (x_index2 >= 0 && x_index2 < MAP_X)
	{
		if (z_index1 >= 0 && z_index1 < MAP_Z)
		{
			if (map[(int)z_index1][(int)x_index2].id == WALL)
				x_collision = true;
		}
		if (z_index2 >= 0 && z_index2 < MAP_Z)
		{
			if (map[(int)z_index2][(int)x_index2].id == WALL)
				x_collision = true;
		}
	}
	if (x_collision) x_pos -= x_vel;
}
	

void MoveBullets(float deltatime)
{
	for (int i = 0; i < NUM_BULLETS; i++)
	{
		if (Bullets[i].state)
		{
			Bullets[i].x += Bullets[i].xv * deltatime;
			Bullets[i].y += Bullets[i].yv * deltatime;
			Bullets[i].z += Bullets[i].zv * deltatime;

			float temp_x = Bullets[i].x;
			float temp_z = Bullets[i].z;

			float x_index, z_index;

			bool z_collision = false, x_collision = false;

			temp_x -= Bullets[i].xv * deltatime;

			x_index = ((temp_x+(CELL_SIZE/2 + CELL_SIZE * MAP_X/2))/CELL_SIZE);
			
			z_index = ((-temp_z+(CELL_SIZE/2 + CELL_SIZE * MAP_Z/2))/CELL_SIZE);

			if (x_index >= 0 && x_index < MAP_X)
			{
				if (z_index >= 0 && z_index < MAP_Z)
				{
					if (map[(int)z_index][(int)x_index].id == WALL && Bullets[i].y >= 0 && Bullets[i].y <= CELL_SIZE * 2.0f)
					{
						z_collision = true;
					}
				}
			}
			temp_x += Bullets[i].xv * deltatime;
			temp_z -= Bullets[i].zv * deltatime;

			x_index = ((temp_x+(CELL_SIZE/2 + CELL_SIZE * MAP_X/2))/CELL_SIZE);
			
			z_index = ((-temp_z+(CELL_SIZE/2 + CELL_SIZE * MAP_Z/2))/CELL_SIZE);

			if (x_index >= 0 && x_index < MAP_X)
			{
				if (z_index >= 0 && z_index < MAP_Z)
				{
					if (map[(int)z_index][(int)x_index].id == WALL && Bullets[i].y >= 0 && Bullets[i].y <= CELL_SIZE * 2.0f)
					{
						x_collision = true;
					}
				}
			}
			if (z_collision || x_collision) Bullets[i].state = 0;

			for (int j = 0; j < NUM_ENEMIES; j++)
			{
				if (Enemies[j].state)
				{
					float x = Bullets[i].x - Enemies[j].x;
					float y = Bullets[i].y - Enemies[j].y;
					float z = Bullets[i].z - Enemies[j].z;
					float dist = sqrt(x*x + y*y + z*z);
					if (dist < 13.0f)
					{
						Bullets[i].state = 0;
						Enemies[j].state = 0;
						score += Enemies[j].score_amount;
						num_shots_hit++;
						PlaySound(MAKEINTRESOURCE(HIT_ID), hinstance_app, SND_ASYNC | SND_RESOURCE);
						break;
					}
				}
			}

			Bullets[i].life -= 20.0f * deltatime;
			if (Bullets[i].life <= 0)
			{
				Bullets[i].state = 0;
				
			}
		}
	}
}

void DrawBullets()
{
	D3DXMATRIX T;
	Device->SetMaterial(&RED_MTRL);
	for (int i = 0; i < NUM_BULLETS; i++)
	{
		if (Bullets[i].state)
		{
			D3DXMatrixTranslation(&T, Bullets[i].x, Bullets[i].y, Bullets[i].z);
			Device->SetTransform(D3DTS_WORLD, &T);
			bullet->DrawSubset(0);
		}
	}
}



void MoveEnemies(float deltatime)
{
	for (int i = 0; i < NUM_ENEMIES; i++)
	{
		if (Enemies[i].state)
		{
			Enemies[i].y += Enemies[i].yv * deltatime;
			if (Enemies[i].y > 400.0f)
				Enemies[i].score_amount = 250;
			if (Enemies[i].y > 500.0f)
				Enemies[i].score_amount = 500;
			if (Enemies[i].y > 600.0f)
				Enemies[i].score_amount = 1000;
			if (Enemies[i].y > 700.0f) 
			{
				Enemies[i].state = 0;
				num_escaped++;
			}
			float x = Enemies[i].x - x_pos;
			float y = Enemies[i].y - y_pos;
			float z = Enemies[i].z - z_pos;
			float dist = sqrt(x*x + y*y + z*z);
			//if (dist < 20.0f) PostQuitMessage(0);
		}
		else 
		{
			switch(rand()%4)
			{
				case 0:
					Enemies[i].x = (float)(rand() % 80 + 200);
					Enemies[i].y = 0.0f;
					Enemies[i].z = (float)(rand() % 400) - 200.0f;
					
					Enemies[i].state = 1;
					break;
				case 1:
					Enemies[i].x = -(float)(rand() % 80 + 200);
					Enemies[i].y = 0.0f;
					Enemies[i].z = (float)(rand() % 400) - 200.0f;
					Enemies[i].state = 1;
					break;
				case 2:
					Enemies[i].x = (float)(rand() % 400) - 200.0f;
					Enemies[i].y = 0.0f;
					Enemies[i].z = -(float)(rand() % 80 + 200);
					Enemies[i].state = 1;
					break;
				case 3:
					Enemies[i].x = (float)(rand() % 400) - 200.0f;
					Enemies[i].y = 0.0f;
					Enemies[i].z = (float)(rand() % 80 + 200);
					Enemies[i].state = 1;
					break;
			}
	
			Enemies[i].score_amount = 100;
			if (Enemies[i].yv == 0.0f) Enemies[i].yv = 10.0f;
			else Enemies[i].yv *= 1.05f;
		}
	}
}

void DrawEnemies()
{
	D3DXMATRIX T;
	Device->SetMaterial(&WHITE_MTRL);
	Device->SetTexture(0, tex[4]);
	for (int i = 0; i < NUM_ENEMIES; i++)
	{
		if (Enemies[i].state)
		{
			D3DXMatrixTranslation(&T, Enemies[i].x, Enemies[i].y, Enemies[i].z);
			Device->SetTransform(D3DTS_WORLD, &T);
			enemy->DrawSubset(0);
		}
	}
}

void ProcessMenuControls()
{
	if (kb_state[DIK_1])
	{
		gamestate = SETUPGAME;
	}
	if (kb_state[DIK_2])
	{
		gamestate = EDITMAP;
	}
	if (kb_state[DIK_3] || (kb_state[DIK_ESCAPE] && !prev_kb_state[DIK_ESCAPE]))
	{
		PostQuitMessage(0);
	}
}

void ProcessMenuLogic(float deltatime)
{
	static float theta = 0.0f, height = 5.0f;
	static bool go_up = true;
	theta += 0.5f * deltatime;
	if (go_up)
		height += 25.5f*deltatime;
	else
		height -= 25.5f*deltatime;
	if (height < 5.0f) go_up = true;
	else if (height > 300.0f) go_up = false;

	if (theta > 6.28f) theta -= 6.28f;


	sun_on_walls.Position.x = -500.0f;
	sun_on_walls.Position.y = 50.0f;
	sun_on_walls.Position.z = 290.0f;
	Device->SetLight(1, &sun_on_walls);

	sun_on_ground.Position.x = - 500.0f;
	sun_on_ground.Position.y = 50.0f;
	sun_on_ground.Position.z = 290.0f;	
	Device->SetLight(2, &sun_on_ground);

	// UPDATE VIEW MATRIX --------------------------------------------
	x_dir = (cosf(phi) * sinf(theta));
	y_dir = (sinf(phi)              );
	z_dir = (cosf(phi) * cosf(theta));

	x_pos = 300.0f * sinf(theta);
	z_pos = 300.0f * cosf(theta);
	D3DXVECTOR3 position(x_pos, height, z_pos);		
	D3DXVECTOR3 target(0.0f, 20.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);
}

void DrawMenu()
{
	D3DXMATRIX T, R;
	Device->SetMaterial(&WHITE_MTRL);
	//DrawSkyBox(0.0f, -50.0f, 0.0f);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	DrawMap();
	// Draw Fence ----------------------------------------------------------
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_LIGHTING, false);

	D3DXMatrixTranslation(&T, 0.0f, 10.0f, 325.0f);
	Device->SetTransform(D3DTS_WORLD, &T);
	Device->SetTexture(0, tex[2]);
	
	fence->DrawSubset(0);
	D3DXMatrixRotationY(&R, 0.5f * D3DX_PI);

	Device->SetTransform(D3DTS_WORLD, &(T * R));
	fence->DrawSubset(0);

	D3DXMatrixRotationY(&R, D3DX_PI);
	Device->SetTransform(D3DTS_WORLD, &(T * R));
	fence->DrawSubset(0);

	D3DXMatrixRotationY(&R, 1.5f * D3DX_PI);
	Device->SetTransform(D3DTS_WORLD, &(T * R));
	fence->DrawSubset(0);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_LIGHTING, true);
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	D3DXVECTOR3 pos(SCREEN_WIDTH/2 - 256, SCREEN_HEIGHT/2 - 256, 0.0f);	
	sprite->Draw(tex[6], NULL , NULL, &pos, WHITE);
	sprite->End();
}

void SetupGame()
{
	// reset all variables
	x_pos = 0.0f, y_pos = 12.0f, z_pos = -300.0f;
	theta = 0.0f, phi = 0.0f,
	ammunition = 100, score = 0, num_escaped = 0, 
	num_shots_fired = 0, num_shots_hit = 0,
	hit_per = 0.0f;
	gametimer = 120.0f;
	ammo_rot = 0.0f;
	
	for (int i = 0; i < NUM_ENEMIES; i++)
	{
		Enemies[i].state = 0;
		Enemies[i].yv = 0;
		Enemies[i].score_amount = 100;
	}

	LoadMap();	
	
	Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
}

void ProcessGameControls(float deltatime)
{
	x_vel = 0.0f, z_vel = 0.0f;
	//static bool light_on = true;
	static bool flashlight_on = false;
	theta += mouse_state.lX * .0025f;
	phi -= mouse_state.lY * .0025f;

	static DWORD mb_left_time = 0;
	if ((mouse_state.rgbButtons[0] || kb_state[DIK_SPACE]) && GetTickCount() - mb_left_time > 100)
	{
		if (ammunition > 0)
		{
			for (int i = 0; i < NUM_BULLETS; i++)
			{
				mb_left_time = GetTickCount();
				if (!Bullets[i].state)
				{
					phi += 0.02f;
					Bullets[i].x = x_pos + x_dir * 10.0f;
					Bullets[i].y = y_pos + y_dir * 10.0f;
					Bullets[i].z = z_pos + z_dir * 10.0f;

					Bullets[i].xv = x_dir * 800.0f;
					Bullets[i].yv = y_dir * 800.0f;
					Bullets[i].zv = z_dir * 800.0f;

					Bullets[i].life = 100.0f;
					Bullets[i].state = 1;
					Device->LightEnable(3, true);
					num_shots_fired++;
					ammunition--;
					PlaySound(MAKEINTRESOURCE(GUN_ID), hinstance_app, SND_ASYNC | SND_RESOURCE);
					
					break;
				}
			}
		}
	}
	else if (GetTickCount() - mb_left_time  > 50)Device->LightEnable(3, false);
	
	
	if (kb_state[DIK_LEFT])
	{
		theta -= 1.5f * deltatime;
	}
	if (kb_state[DIK_RIGHT])
	{
		theta += 1.5f * deltatime;
	}
	if (kb_state[DIK_UP])
	{
		phi += 1.5f * deltatime;
	}
	if (kb_state[DIK_DOWN])
	{
		phi -= 1.5f * deltatime;
	}
	if (kb_state[DIK_W])
	{
		x_vel += (sinf(theta) * SPEED) * deltatime;
		z_vel += (cosf(theta) * SPEED) * deltatime;
	}
	if (kb_state[DIK_S])
	{
		x_vel += -(sinf(theta) * SPEED) * deltatime;
		z_vel += -(cosf(theta) * SPEED) * deltatime;
	}
	if (kb_state[DIK_A])
	{
		x_vel += -(cosf(theta) * SPEED) * deltatime;
		z_vel +=  (sinf(theta) * SPEED) * deltatime;
	}
	if (kb_state[DIK_D])
	{
		x_vel +=  (cosf(theta) * SPEED) * deltatime;
		z_vel += -(sinf(theta) * SPEED) * deltatime;
	}
	if (kb_state[DIK_F] && !prev_kb_state[DIK_F])
	{
		if (flashlight_on)
		{
			flashlight_on = false;
			Device->LightEnable(0, false);
		}
		else
		{
			flashlight_on = true;
			Device->LightEnable(0, true);
		}
	}
	if (kb_state[DIK_C])
	{
		y_pos = 8.0f;
	}
	else
	{
		y_pos = 15.0f;
	}

	
	if (kb_state[DIK_ESCAPE])
	{
		gamestate = RESETMENU;
	}
}

void ProcessGameLogic(float deltatime)
{
	//make sure angle stays 0.0f - 6.28f (2pi R or 360 degrees)
	if (theta > 6.28f)
		theta -= 6.28f;
	else if (theta < 0.0f)
		theta += 6.28f;

	//make sure angle stays between -1.57f and 1.57f (-pi/2 and pi/2 or -90 and 90 degrees)
	if (phi > 1.57f)
		phi = 1.57f;
	else if (phi < -1.57f)
		phi = -1.57f;

	gametimer -= (float)elapsed_time/1000.0f;
	if (gametimer <= 0)
	{
		gamestate = GAMEOVER;
		hit_per = 100.0f * (float)num_shots_hit/(num_shots_fired?num_shots_fired:1);
		float calc_score = score + (hit_per/100.0f * (score + num_shots_hit)) - (num_escaped * 50.0f);
		score = (int)calc_score;
	}

	ammo_rot += 1.5f * deltatime;
	if (ammo_rot >= 6.28f) ammo_rot -= 6.28f;

	MovePlayer();

	MoveEnemies(deltatime);

	MoveBullets(deltatime);
	
	
	// UPDATE VIEW MATRIX --------------------------------------------
	x_dir = (cosf(phi) * sinf(theta));
	y_dir = (sinf(phi)              );
	z_dir = (cosf(phi) * cosf(theta));

	D3DXVECTOR3 position(x_pos, y_pos, z_pos);		
	D3DXVECTOR3 target(x_pos + x_dir, y_pos + y_dir, z_pos + z_dir);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);


	// UPDATE LIGHTS --------------------------------------------------
	flashlight.Direction.x = x_dir;
	flashlight.Direction.y = y_dir;
	flashlight.Direction.z = z_dir;
	flashlight.Position.x = x_pos;
	flashlight.Position.y = y_pos;
	flashlight.Position.z = z_pos;
	Device->SetLight(0, &flashlight);
	
	sun_on_walls.Position.x = -500.0f;
	sun_on_walls.Position.y = 50.0f;
	sun_on_walls.Position.z = 290.0f;
	Device->SetLight(1, &sun_on_walls);

	sun_on_ground.Position.x = x_pos - 500.0f;
	sun_on_ground.Position.y = y_pos + 50.0f;
	sun_on_ground.Position.z = z_pos + 290.0f;	
	Device->SetLight(2, &sun_on_ground);
			
	muzzle_flash.Position.x = x_pos + x_dir * 20.0f;
	muzzle_flash.Position.y = y_pos;
	muzzle_flash.Position.z = z_pos + z_dir * 20.0f;
	Device->SetLight(3, &muzzle_flash);
	
	//------------------------------------------------------------
}

void DrawGame()
{
	D3DXMATRIX R, T;
		
	D3DXMatrixIdentity(&T);
	DrawSkyBox(x_pos, y_pos, z_pos);
		
	D3DXMatrixIdentity(&T);
	Device->SetTransform(D3DTS_WORLD, &T);
	Device->SetMaterial(&RED_MTRL);
	teapot->DrawSubset(0);

	DrawBullets();		
	DrawEnemies();

	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	Device->SetMaterial(&WHITE_MTRL);

	DrawMap();
	
		
	// Draw Fence ----------------------------------------------------------
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_LIGHTING, false);

	D3DXMatrixTranslation(&T, 0.0f, 10.0f, 325.0f);
	Device->SetTransform(D3DTS_WORLD, &T);
	Device->SetTexture(0, tex[2]);
	
	fence->DrawSubset(0);
	D3DXMatrixRotationY(&R, 0.5f * D3DX_PI);

	Device->SetTransform(D3DTS_WORLD, &(T * R));
	fence->DrawSubset(0);

	D3DXMatrixRotationY(&R, D3DX_PI);
	Device->SetTransform(D3DTS_WORLD, &(T * R));
	fence->DrawSubset(0);

	D3DXMatrixRotationY(&R, 1.5f * D3DX_PI);
	Device->SetTransform(D3DTS_WORLD, &(T * R));
	fence->DrawSubset(0);

	
	Device->SetRenderState(D3DRS_LIGHTING, true);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	/*DrawMirror();*/
	
	// End Draw Fence -------------------------------------------------------

}

void DrawGameHUD()
{
	RECT rect;
	rect.top = 20;
	rect.bottom = rect.top + 200;
	rect.left = 20;
	rect.right = rect.left + 600;
	
	sprintf(Buffer, "Game Timer %.0f\nAmmo %i\nScore %i", gametimer, ammunition, score);
	font->DrawText(NULL,Buffer,-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(255,0,0));
	

	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	D3DXVECTOR3 pos(SCREEN_WIDTH/2 - 16, SCREEN_HEIGHT/2 - 16, 0.0f);	
	sprite->Draw(tex[5], NULL , NULL, &pos, WHITE);
	sprite->End();
}

void DrawGameOver()
{
	RECT rect;
	rect.top = 80;
	rect.bottom = rect.top + 400;
	rect.left = 150;
	rect.right = rect.left + 399;

	
	sprintf(Buffer, "GAME RESULTS\n--------------------------------------------\nFinal Score\nShots Hit/Fired \nHit Percent \n# Escaped ");
	font->DrawText(NULL,Buffer,-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(0,0,255));
	rect.left = 400;
	rect.right = rect.left + 100;
	sprintf(Buffer, "\n\n%i\n%i/%i\n%.2f%%\n%i", score, num_shots_hit, num_shots_fired, hit_per, num_escaped);
	font->DrawText(NULL,Buffer,-1,&rect,DT_TOP|DT_RIGHT,D3DCOLOR_XRGB(0,0,255));
}

void ProcessEditorControls()
{
	//static float cursor_x = 0.0f, cursor_y = 0.0f;
	static int id = 0;
	static int flag = AMMO;
	cursor_x += mouse_state.lX * 0.5f;
	cursor_y += mouse_state.lY * 0.5f;
	if (cursor_x < 0) 
		cursor_x = 0;
	else if (cursor_x > MAP_X * 32) 
		cursor_x = (MAP_X * 32) - 1;
	if (cursor_y < 0)
		cursor_y = 0;
	else if (cursor_y >= MAP_Z * 32)
		cursor_y = (MAP_Z * 32) - 1;
	if (mouse_state.rgbButtons[0])
	{
		if (cursor_x >= 0 && cursor_x < MAP_X * 32 && cursor_y >= 0 && cursor_y < MAP_Z * 32)
		{
			if (id == WALL)
				map[(int)(cursor_y/32)][(int)(cursor_x/32)].flag = EMPTY;
			map[(int)(cursor_y/32)][(int)(cursor_x/32)].id = id;
		}
	}
	if (mouse_state.rgbButtons[1])
	{
		if (cursor_x >= 0 && cursor_x < MAP_X * 32 && cursor_y >= 0 && cursor_y < MAP_Z * 32)
		{
			if (map[(int)(cursor_y/32)][(int)(cursor_x/32)].id != WALL)
				map[(int)(cursor_y/32)][(int)(cursor_x/32)].flag = flag;
		}
	}
	if (kb_state[DIK_ESCAPE])
	{
		gamestate = MENU;
	}
	if (kb_state[DIK_1]) id = GRASS;
	if (kb_state[DIK_2]) id = TILE;
	if (kb_state[DIK_3]) id = WALL;
	if (kb_state[DIK_4]) flag = EMPTY;
	if (kb_state[DIK_5]) flag = AMMO;
	if (kb_state[DIK_S]) 
	{
		SaveMap();
	}

	
}

void DrawEditor()
{
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	for (int z = 0; z < MAP_Z; z++)
	{
		for (int x = 0; x < MAP_X; x++)
		{
			D3DXVECTOR3 pos(x * 32, z * 32, 0.0f);
			sprite->Draw(tex[map[z][x].id + 8], NULL, NULL, &pos, WHITE);
			//pos.z = 0.1f;
			if (map[z][x].flag == AMMO)
				sprite->Draw(tex[12], NULL, NULL, &pos, WHITE);
		}
	}
	D3DXVECTOR3 pos(cursor_x, cursor_y, 0.0f);
	sprite->Draw(tex[7], NULL, NULL, &pos, WHITE);
	sprite->End();
}


void LoadMap()
{
	// NEW 
	// LOAD MAP USING X FILE
	CXFileLoad xFileLoad;
	xFileLoad.LoadFromFile("map\\MapData.x");
	char *szTemplate = "xof 0303txt 0032\
					    template MAP_INFO \
						{ \
							<96D531CA-2813-4b45-A45D-393B7F921F89> \
							array DWORD id[18][18]; \
							array DWORD flag[18][18]; \
						}";
	xFileLoad.RegisterTemplates(szTemplate, sizeof(szTemplate));
	
	SIZE_T count;
	count = xFileLoad.GetChildCount();
	
	for (int counter = 0; counter < count; counter++)
	{
		CDataObject *object = xFileLoad.GetChild(counter);
		
		object->GetData();
		
		if (object->GetType() == GUID_MAP)
		{
			MAP_INFO *mapInfo;
			SIZE_T size = sizeof(MAP_INFO);
			object->Lock(&size, (const void**)&mapInfo);
			for (int z = 0; z < MAP_Z; z++)
			{
				for (int x = 0; x < MAP_X; x++)
				{
					map[z][x].id = mapInfo->id[z][x];
					map[z][x].flag = mapInfo->flag[z][x];
				}
			}
			object->Unlock();
		}
		
		delete object;
	}
	
	/*fin.open("Map\\map.txt",ios_base::in);
	for (int z = 0; z < MAP_Z; z++)
		for (int x = 0; x < MAP_X; x++)
			fin >> map[z][x].id >> map[z][x].flag;
	fin.close();*/
}

void SaveMap()
{
	// NEW
	// SAVE MAP USING X FILE
	CXFileSave xFileSave;
	xFileSave.CreateFile("map\\MapData.x");
	char *szTemplate = "xof 0303txt 0032\
				    template MAP_INFO \
					{ \
						<96D531CA-2813-4b45-A45D-393B7F921F89> \
						array DWORD id[18][18]; \
						array DWORD flag[18][18]; \
					}";
	xFileSave.RegisterTemplates(szTemplate, strlen(szTemplate));

	MAP_INFO mapInfo;
	ZeroMemory(&mapInfo, sizeof(MAP_INFO));
	//test.num = 16;
	for (int z = 0; z < MAP_Z; z++)
	{
		for (int x = 0; x < MAP_X; x++)
		{
			mapInfo.id[z][x] = map[z][x].id;
			mapInfo.flag[z][x] = map[z][x].flag;
		}
	}
	xFileSave.CreateSaveDataObject(GUID_MAP, "Map", 0, sizeof(MAP_INFO), &mapInfo);
	xFileSave.Save();

	/*fout.open("Map\\map.txt",ios_base::out | ios_base::trunc);
	for (int z = 0; z < MAP_Z; z++)
		for (int x = 0; x < MAP_X; x++)
			fout << map[z][x].id << " " << map[z][x].flag << endl;
	fout.close();*/
}
