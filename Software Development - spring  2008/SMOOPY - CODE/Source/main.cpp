// Collin Munson - EVERYTHING
// Justin Knox - bullets that weren't used
// Doug Ayers - collision detection thats not used
// Brandon Hillin - Models and stuff
// Software Development

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <vector>
#include "resource.h"
#include "d3dbase.h"
#include "CInput.h"
#include "CCamera.h"
#include "CMesh.h"
#include "Vertex.h"
#include "Sprite.h"
#include "Terrain.h"
#include "dxutil/dxutil.h"
#include "CFont.h"
#include "PTPMulti.h"
#include "../FMOD/inc/fmod.hpp"

using namespace std;

// Defines
//-------------------------------------------------------------
#define CLASSNAME "SMOOPY"
#define WINDOWNAME "STYLES"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// TERRAIN DEFINE ------------
#define HEIGHT_SCALE 50.5f

#define NUM_TERRAIN_PATCHES 64

#define NUM_VERTS_PER_ROW 65
#define NUM_VERTS_PER_COL 65

#define NUM_CELLS_PER_ROW 64
#define NUM_CELLS_PER_COL 64

#define NUM_PATCHES_PER_ROW 8
#define NUM_PATCHES_PER_COL 8

#define CELL_SPACING 800
// ---------------------------

// Used for the setting up projection matrix
#define FARZ_PLANE 100000.0f

#define NUM_SHIPS 6

enum GAMESTATE {START, LOADING, MENU, SHIP_SELECTION_MENU, PLANET_SELECTION_MENU, PREVIEWMODE, MULTIPLAYER_MENU, MULTIPLAYER, INSTRUCTIONS, CREDITS};

// JEJ0JO - green earthy planet
// KEF_WID - lava planet
// IZE_PRIME - ice planet
enum PLANET {NONE, JEJOJO, KEF_WID, IZE_PRIME};





// Function Prototypes
//------------------------------------------------------------------------------------------------------------------------
int  GameInit();
int  GameMain(float);
void GameMurder();
//------------------------------------------------------------------------------------------------------------------------
void InitWaterMesh();
void DrawWaterMesh();
void AnimateWaterMesh();
void DestroyWaterMesh();

bool MouseOverButton(int cursorX, int cursorY, int btnX, int btnY, int btnWidth, int btnHeight);

//void CreateBullet(DPNID dpowner, D3DXMATRIX R, float x, float y, float z, float xv, float yv, float zv);
void UpdateBullets(float deltatime);
void DrawBullets();
void InitBullets();


int InitSoundSystem();
void KillSoundSystem();
void PlaySound(FMOD::Sound *sound);
int LoadSound(char filename[], FMOD::Sound **sound, FMOD_MODE mode = FMOD_DEFAULT);
void UpdateSoundSystem();

void UpdateNetwork();

FMOD::System *pfmodSystem = 0;
FMOD::Sound *sound[8] = {0};

// Globals
//-----------------------------------------------------------------------------------------------------------------------------

// SPRITE VARIABLES FOR 2D STUFF
LPD3DXSPRITE g_D3DXSprite;
CSprite g_spriteMenu, g_spriteLoading, g_spriteCredits;
CSprite g_spriteCursor;

// TERRAIN OBJECT
//Terrain g_Terrain[3];
Terrain g_Terrain;

// INPUT OBJECT
CInput g_Input;

// CAMERA OBJECTS
CCamera g_Camera, g_Object, g_FreeCam;

CFont g_Font;

// SHIP MODEL CRAP
CMesh g_meshShip[NUM_SHIPS];
enum {FIREFLY, GALIGA, U_WING, FALKEN, FISHY};
char *strShipModels[] = {"firefly.x", "galiga.x", "shipfighter.x", "falken.x", "fishy.x", "swordfish.x"};
int currShip = FIREFLY;


struct SHIP_INFO
{
	float radius;
	float height;
	float offset;
};

SHIP_INFO ShipInfo[NUM_SHIPS];


LPD3DXMESH meshCylinder;
LPDIRECT3DTEXTURE9 waterTex = 0;

HINSTANCE hinstance_app;
HWND hwnd_app;

D3DLIGHT9 g_dirLight;

float cursorX = 0, cursorY = 0;

GAMESTATE gameState = START;
GAMESTATE nextState = START;
PLANET currLoadedPlanet = NONE;
LPDIRECT3DTEXTURE9 g_planetTex[3], g_terrainTex[3];
D3DCOLOR g_clearColor = D3DCOLOR_XRGB(0, 120, 170);

LPDIRECT3DVERTEXBUFFER9 Lines = 0;
float velocity = 0.0f;
float oldx, oldy, oldz;



LPD3DXFONT		pD3DXFont = NULL;
HFONT			hFont = NULL;

char text[16];

LPDIRECT3DSURFACE9 back, source;
bool bText = false;

float shipVelocity = 0.0f;
bool bAccel = false;
bool bDecel = false;



char strLoadingOutput[256];
bool bFreeCam = false;

bool TCPInit = false;
bool Hosting = false;
bool Join = false;
char dpOutput[80];
char testmsg[80];


LPD3DXMESH g_pWaterMesh;
LPDIRECT3DTEXTURE9 g_pWaterTex;


//-------------------------------------------------------------------------------------------------------------------------------
int LoadLevel(int n)
{
	D3DXVECTOR3 dir(0, -1, 0);
	g_Terrain.Kill();
	switch(n)
	{
		
		
		case JEJOJO:
		{
			
			g_Terrain.Init(Device, NUM_TERRAIN_PATCHES, NUM_VERTS_PER_ROW, CELL_SPACING, HEIGHT_SCALE);
			g_Terrain.LoadHeightmap("Heightmap\\Jejojo_heightmap.raw", 513);
			g_Terrain.LoadTextures("Textures\\Planet\\Jejojo\\Source\\Rock01_Tile_512.bmp",
					"Textures\\Planet\\Jejojo\\Source\\Grass_TileImprove_512.bmp",
					"Textures\\Planet\\Jejojo\\Source\\Dirt02_TileImprove_512.bmp",
					"Textures\\Planet\\Jejojo\\Source\\Sand_512.bmp", 
					D3DCOLOR_XRGB(0,0,255));

			for (int z = 0; z < NUM_PATCHES_PER_COL; z++)
				{
					for (int x = 0; x < NUM_PATCHES_PER_ROW; x++)
					{
						int index = (z * NUM_PATCHES_PER_ROW) + x;
						char path[256];
						sprintf(path, "Textures\\Planet\\Jejojo\\Terrain\\Patch%i\.bmp", index); 
						sprintf(strLoadingOutput, "Loading Texture %s", path);
						g_Terrain.SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
						
						g_Terrain.GenerateTexture(path, index, 512, 512);		
					}
				}
			
			g_Terrain.KillSrcTextures();
			g_Terrain.KillHeightMap();
			strcpy(strLoadingOutput, "Setting environment parameters");
			g_clearColor = D3DCOLOR_XRGB(75, 175, 255);
			SetFog(100.0f, 90000.0f, g_clearColor);
			
			D3DXCOLOR c(WHITE);
			g_dirLight = InitDirectionalLight(&dir, &c);
			currLoadedPlanet = JEJOJO;
		}break;
		case KEF_WID:
		{
			g_Terrain.Init(Device, NUM_TERRAIN_PATCHES, NUM_VERTS_PER_ROW, CELL_SPACING, HEIGHT_SCALE);
			g_Terrain.LoadHeightmap("Heightmap\\Kef-Wid_heightmap.raw", 513);
			
			g_Terrain.LoadTextures(
				"Textures\\Planet\\Kef-Wid\\Source\\Rock04.bmp",
				"Textures\\Planet\\Kef-Wid\\Source\\Rock05_Test.bmp",
				"Textures\\Planet\\Kef-Wid\\Source\\Rock02.bmp",
				"Textures\\Planet\\Kef-Wid\\Source\\lava02.bmp", 
				D3DCOLOR_XRGB(255,0,0));

			for (int z = 0; z < 8; z++)
			{
				for (int x = 0; x < 8; x++)
				{
					int index = (z * 8) + x;
					g_Terrain.SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
					char path[256];
					sprintf(path, "Textures\\Planet\\Kef-Wid\\Terrain\\Patch%i\.bmp", index); 
					sprintf(strLoadingOutput, "Loading Texture %s", path);
					g_Terrain.GenerateTexture(path, index, 512, 512);		
				}
			}
			strcpy(strLoadingOutput, "Setting environment parameters");
			g_clearColor = D3DCOLOR_XRGB(250, 120, 0);
			SetFog(100.0f, 90000.0f, g_clearColor);
			D3DXCOLOR c(D3DCOLOR_XRGB(250, 100, 0));
			g_dirLight = InitDirectionalLight(&dir, &c);
			//Device->SetLight(0, &
			currLoadedPlanet = KEF_WID;
		}break;
		case IZE_PRIME:
			currLoadedPlanet = IZE_PRIME;
			break;
	}
	gameState = nextState;
	return(0);
}
int InitLoading(int n)
{
	//g_Terrain.Kill();
	//		g_Terrain.Init(Device, NUM_TERRAIN_PATCHES, NUM_VERTS_PER_ROW, CELL_SPACING, HEIGHT_SCALE);
	//		g_Terrain.LoadHeightmap("Heightmap\\Jejojo_heightmap.raw", 513);
	//		g_Terrain.LoadTextures("Textures\\Planet\\Jejojo\\Source\\Rock01_Tile_512.bmp",
	//				"Textures\\Planet\\Jejojo\\Source\\Grass_TileImprove_512.bmp",
	//				"Textures\\Planet\\Jejojo\\Source\\Dirt02_TileImprove_512.bmp",
	//				"Textures\\Planet\\Jejojo\\Source\\Sand_512.bmp", 
	//				D3DCOLOR_XRGB(0,0,255));
	//		for (int z = 0; z < NUM_PATCHES_PER_COL; z++)
	//			{
	//				for (int x = 0; x < NUM_PATCHES_PER_ROW; x++)
	//				{
	//					int index = (z * NUM_PATCHES_PER_ROW) + x;
	//					char path[256];
	//					sprintf(path, "Textures\\Planet\\Jejojo\\Terrain\\Patch%i\.bmp", index); 
	//					sprintf(strLoadingOutput, "Loading Texture %s", path);
	//					g_Terrain.SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
	//					
	//					g_Terrain.GenerateTexture(path, index, 512, 512);		
	//				}
	//			}
	//		currLoadedPlanet = JEJOJO;
	//LOAD GREEN PLANET TERRAIN
	//if (n == 0)
	//{
	//	g_Terrain[JEJOJO].Kill();
	//			g_Terrain[JEJOJO].Init(Device, NUM_TERRAIN_PATCHES, NUM_VERTS_PER_ROW, CELL_SPACING, HEIGHT_SCALE);
	//			g_Terrain[JEJOJO].LoadHeightmap("Heightmap\\Jejojo_heightmap.raw", 513);
	//			
	//			g_Terrain[JEJOJO].LoadTextures(
	//				"Textures\\Planet\\Jejojo\\Source\\Rock01_Tile_512.bmp",
	//				"Textures\\Planet\\Jejojo\\Source\\Grass_TileImprove_512.bmp",
	//				"Textures\\Planet\\Jejojo\\Source\\Dirt02_TileImprove_512.bmp",
	//				"Textures\\Planet\\Jejojo\\Source\\Sand_512.bmp", 
	//				D3DCOLOR_XRGB(0,0,255));

	//			for (int z = 0; z < NUM_PATCHES_PER_COL; z++)
	//			{
	//				for (int x = 0; x < NUM_PATCHES_PER_ROW; x++)
	//				{
	//					int index = (z * NUM_PATCHES_PER_ROW) + x;
	//					char path[256];
	//					sprintf(path, "Textures\\Planet\\Jejojo\\Terrain\\Patch%i\.bmp", index); 
	//					sprintf(strLoadingOutput, "Loading Texture %s", path);
	//					g_Terrain[JEJOJO].SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
	//					
	//					g_Terrain[JEJOJO].GenerateTexture(path, index, 512, 512);		
	//				}
	//			}
	//			//sprintf(strLoadingOutput, "Destroying temporary Textures");
	//			//g_Terrain[JEJOJO].KillSrcTextures();
	//}
	//else if (n == 1)
	//{
	//			////LOAD LAVA PLANET TERRAIN
	/*g_Terrain[KEF_WID].Init(Device, NUM_TERRAIN_PATCHES, NUM_VERTS_PER_ROW, CELL_SPACING, HEIGHT_SCALE);
	g_Terrain[KEF_WID].LoadHeightmap("Heightmap\\Kef-Wid_heightmap.raw", 513);
	
	g_Terrain[KEF_WID].LoadTextures(
		"Textures\\Planet\\Kef-Wid\\Source\\Rock04.bmp",
		"Textures\\Planet\\Kef-Wid\\Source\\Rock05_Test.bmp",
		"Textures\\Planet\\Kef-Wid\\Source\\Rock02.bmp",
		"Textures\\Planet\\Kef-Wid\\Source\\lava02.bmp", 
		D3DCOLOR_XRGB(255,0,0));

	for (int z = 0; z < 8; z++)
	{
		for (int x = 0; x < 8; x++)
		{
			int index = (z * 8) + x;
			g_Terrain[KEF_WID].SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
			char path[256];
			sprintf(path, "Textures\\Planet\\Kef-Wid\\Terrain\\Patch%i\.bmp", index); 
			sprintf(strLoadingOutput, "Loading Texture %s", path);
			g_Terrain[KEF_WID].GenerateTexture(path, index, 512, 512);		
		}
	}*/
	//}
	//else if (n== 2)
	//{
	////LOAD ICE PLANET TERRAIN
	//g_Terrain[IZE_PRIME].Init(Device, NUM_TERRAIN_PATCHES, NUM_VERTS_PER_ROW, CELL_SPACING, HEIGHT_SCALE);
	//g_Terrain[IZE_PRIME].LoadHeightmap("Heightmap\\Ize-Prime_heightmap.raw", 513);
	//
	//g_Terrain[IZE_PRIME].LoadTextures(
	//	"Textures\\Planet\\Ize-Prime\\Source\\snow.bmp",
	//	"Textures\\Planet\\Ize-Prime\\Source\\Ice3_TileImprove_512.bmp",
	//	"Textures\\Planet\\Ize-Prime\\Source\\Ice2_TileImprove_512.bmp",
	//	"Textures\\Planet\\Ize-Prime\\Source\\Ice4_TileImprove_512.bmp", 
	//	D3DCOLOR_XRGB(180,180,255));

	//for (int z = 0; z < 8; z++)
	//{
	//	for (int x = 0; x < 8; x++)
	//	{
	//		int index = (z * 8) + x;
	//		g_Terrain[IZE_PRIME].SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
	//		char path[256];
	//		sprintf(path, "Textures\\Planet\\Ize-Prime\\Terrain\\Patch%i\.bmp", index); 
	//		sprintf(strLoadingOutput, "Loading Texture %s", path);
	//		g_Terrain[IZE_PRIME].GenerateTexture(path, index, 512, 512);		
	//	}
	//}

	// LOAD MODELS
	for (int i = 0; i < NUM_SHIPS; i++)
	{
		sprintf(strLoadingOutput, "Loading Texture %s", strShipModels[i]);
		g_meshShip[i].LoadXFile(strShipModels[i], "Models", "Textures\\Model");
		g_meshShip[i].Center();
		ShipInfo[i].radius = g_meshShip[i].GetRadius();
		ShipInfo[i].height = g_meshShip[i].GetHeight()*2;
		ShipInfo[i].offset = g_meshShip[i].GetOffset()*2;
		

	}
	
	gameState = MENU;
	
	
	return(0);
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CHAR:
			//text commands
			int len = strlen(text);
			if (bText)
			{
				if (wparam == 13)
				{
					//memset(text, 0, sizeof(text));
					text[len-1] = '\n';
					
				}
				if (wparam == 8 && len >= 0)
				{
					text[len-1] = '\0';
				}
				else if (len < 16)
				{
					text[len] = wparam;
					text[len+1] = '\0';
				}
			}
			break;
		
			
	}
	return(DefWindowProc(hwnd,msg,wparam,lparam));
}


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int nshowcmd)
{
	//MessageBox(0, "Consult README.txt for controls", "INSTRUCTIONS", 0);
	hinstance_app = hinstance;
	
	srand((unsigned int)GetTickCount());
	if (!GameInit())
	{
		MessageBox(0, "GameInit() - FAIILED", 0, 0);
		return(0);
	}
	
	EnterMsgLoop(GameMain);

	GameMurder();
	
	return(0);
}

int GameInit()
{
	// Initialize Direct3D -------------------------------------------------------------------
	if (!InitD3D(CLASSNAME, WINDOWNAME, SCREEN_WIDTH, SCREEN_HEIGHT, true, D3DDEVTYPE_HAL))
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

	LoadSound("Sounds\\sound effects\\laser3.wav", &sound[0]);
	LoadSound("Sounds\\sound effects\\laser1.wav", &sound[1]);
	LoadSound("Sounds\\sound effects\\explosion02.wav", &sound[2]);

	D3DXCreateSprite(Device, &g_D3DXSprite);
	g_spriteMenu.Init(Device, g_D3DXSprite, 11);
	g_spriteMenu.Load("Textures\\User Interface\\menu.dds");
	g_spriteMenu.SetFrame(0,   0,   0, 800, 600); // menu background

	for (int i = 0; i < 5; i++)
	{
		g_spriteMenu.SetFrame(2 * i + 1,   0, 600 + (i * 28), 340, 28); // unselected menu item
		g_spriteMenu.SetFrame(2 * i + 2, 340, 600 + (i * 28), 340, 28);  // selected menu item
	}

	D3DXCreateCylinder(Device, 15.0f, 15.0f, 100.0f, 5, 1, &meshCylinder, 0);
	g_spriteLoading.Init(Device, g_D3DXSprite);
	g_spriteLoading.Load("Textures\\User Interface\\loading.bmp");

	g_spriteCredits.Init(Device, g_D3DXSprite);
	g_spriteCredits.Load("Textures\\User Interface\\Credits copy.jpg");
	
	g_spriteCursor.Init(Device, g_D3DXSprite);
	g_spriteCursor.Load("Textures\\User Interface\\crosshair.dds");
	g_spriteCursor.SetSourceRect(0, 0, 32, 32);
	
	g_Font.Init(Device, g_D3DXSprite, "Textures\\User Interface\\font-small.dds", 16, 16);
	
	// BEGIN LIGHTING --------------------------------------------------
	D3DXVECTOR3 dir(0.0f, -1.0f, 0.0f);
	D3DXCOLOR c(WHITE);

	g_dirLight = InitDirectionalLight(&dir, &c);

	Device->SetLight(0, &g_dirLight);
	Device->LightEnable(0, true);

	Device->SetRenderState(D3DRS_LIGHTING, true);
	// END LIGHTING ---------------------------------------------------

	// SETUP PROJECTION
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.45f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, FARZ_PLANE);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	// USE ALPHA CHANNEL IN TEXTURES FOR ALPHA
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// SET SAMPLING FILTER FOR TEXTURES TO LINEAR
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER,  D3DTEXF_ANISOTROPIC);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER,  D3DTEXF_ANISOTROPIC);
	Device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// set blending factors so that alpha component determines transparency
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	InitWaterMesh();
	//FOG COLOR
	//SNOW PLANET 
	//InitPixelFog(100.0f, 3500.0f, D3DCOLOR_XRGB(0,120, 170));
	
	D3DXCreateFont(Device, 26, 0, 0,0 ,0,0 ,0, PROOF_QUALITY, 0, "arial", &pD3DXFont); 
	InitBullets();
	
	return(1);
}

//kill everything, make sure there are absolutely no survivors
void GameMurder()
{
	DestroyWaterMesh();

	SAFE_RELEASE(meshCylinder);

	SAFE_RELEASE(pD3DXFont);
	
	KillSoundSystem();
	//kill direct input
	g_Input.KillMouse();
	g_Input.KillKB();
	g_Input.KillDI();

	//kill device
	KillD3D();
}

int GameMain(float deltatime)
{

	float yaw = 0.0f, pitch = 0.0f, roll = 0.0f;
	static float x = 0.0f, y = 0.0f, z = 0.0f;
	static float shipYaw = 0.0f, shipPitch = 0.0f, shipRoll = 0.0f;
	static float offset = 0.0f, height = 0.0f;
	static float shipRotate = 0.0f;
	static float shipX = 0, shipY = 13000, shipZ = 0;
	static float shipVelocity = 2000;
	int x_index, z_index;
	D3DXMATRIX V; 
	g_Camera.GetPosition(oldx, oldy, oldz);

	float starttime = timeGetTime();
	bAccel = false;
	bDecel = false;
	if (Device)
	{
		g_Input.ReadMouse();
		g_Input.ReadKB();
		bool bItemSelected[5];
		for (int i = 0; i < 5; i++)
			bItemSelected[i] = false;
		HANDLE thread;
		switch(gameState)
		{
			case START:
				
				gameState = LOADING;
				CreateThread(0, 0, (LPTHREAD_START_ROUTINE)InitLoading, (LPVOID)0, 0, 0);
				
				break;
			case LOADING:
				/*if (g_Input.IsKeyDown(DIK_RETURN))
					gameState = MENU;
				g_Terrain[JEJOJO].Init(Device, NUM_TERRAIN_PATCHES, NUM_VERTS_PER_ROW, CELL_SPACING, HEIGHT_SCALE);
				g_Terrain[JEJOJO].LoadHeightmap("Heightmap\\Jejojo_heightmap.raw", 513);
				
				g_Terrain[JEJOJO].LoadTextures(
					"Textures\\Planet\\Jejojo\\Source\\Rock01_Tile_512.bmp",
					"Textures\\Planet\\Jejojo\\Source\\Grass_TileImprove_512.bmp",
					"Textures\\Planet\\Jejojo\\Source\\Dirt02_TileImprove_512.bmp",
					"Textures\\Planet\\Jejojo\\Source\\Sand_512.bmp", 
					D3DCOLOR_XRGB(0,0,255));

				for (int z = 0; z < 8; z++)
				{
					for (int x = 0; x < 8; x++)
					{
						int index = (z * 8) + x;
						char path[256];
						sprintf(path, "Textures\\Planet\\Jejojo\\Terrain\\Patch%i\.bmp", index); 
						sprintf(strLoadingOutput, "Loading Texture %s", path);
						g_Terrain[JEJOJO].SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
						
						g_Terrain[JEJOJO].GenerateTexture(path, index, 512, 512);		
					}
				}
				for (int i = 0; i < NUM_SHIPS; i++)
	{
		sprintf(strLoadingOutput, "Loading Texture %s", strShipModels[i]);
		g_meshShip[i].LoadXFile(strShipModels[i], "Models", "Textures\\Model");
		g_meshShip[i].Center();
		ShipInfo[i].radius = g_meshShip[i].GetRadius();
		ShipInfo[i].height = g_meshShip[i].GetHeight()*2;
		ShipInfo[i].offset = g_meshShip[i].GetOffset()*2;
		D3DXCreateSphere(Device, ShipInfo[i].radius, 20, 20, &BoundingSphere[i], 0);

	}gameState = MENU;*/
				break;
			case MENU:
			{
				g_Object.SetPosition(0, 0, 0);
				if (g_Input.IsKeyDown(DIK_ESCAPE) && !g_Input.WasKeyDown(DIK_ESCAPE))
					PostQuitMessage(0);
				cursorX += g_Input.m_MouseState.lX * 0.5f;
				cursorY += g_Input.m_MouseState.lY * 0.5f;
				if (cursorX < 0) cursorX = 0;
				else if (cursorX >= SCREEN_WIDTH) cursorX = SCREEN_WIDTH - 1;

				if (cursorY < 0) cursorY = 0;
				else if (cursorY >= SCREEN_HEIGHT) cursorY = SCREEN_HEIGHT - 1;

				if (MouseOverButton(cursorX, cursorY, 230, 230, 340, 28))
					bItemSelected[0] = true;

				else if (MouseOverButton(cursorX, cursorY, 230, 230 + 340, 230 + 48, 230 + 48 + 28))
					bItemSelected[1] = true;

				else if (MouseOverButton(cursorX, cursorY, 230, 230 + 340, 278 + 48, 278 + 48 + 28))
					bItemSelected[2] = true;

				if (g_Input.IsButtonDown(0) && bItemSelected[0])
				{
					g_Object.SetPosition(0, HEIGHT_SCALE * 255 * 2, 0);
					DWORD threadId;
					nextState = PREVIEWMODE;
					gameState = LOADING;
					g_clearColor = D3DCOLOR_XRGB(75, 175, 255);
					SetFog(100.0f, 90000.0f, g_clearColor);
					CreateThread(0, 0, (LPTHREAD_START_ROUTINE)LoadLevel, (LPVOID)1, 0, &threadId);
					
					
				}
				if (g_Input.IsButtonDown(0) && bItemSelected[1])
				{
					gameState = SHIP_SELECTION_MENU;
				}
				if (g_Input.IsKeyDown(DIK_T) && !g_Input.WasKeyDown(DIK_T))
				{
					if (bText)
						bText = false;
					else
						bText = true;
				}
				if (g_Input.IsKeyDown(DIK_Z))
				{
					g_D3DXSprite->Begin(D3DXSPRITE_ALPHABLEND);
					g_spriteCredits.SetPosition(0, 0, 0);
					g_spriteCredits.SetCenter(0, 0);
					g_spriteCredits.Draw(0);
					g_D3DXSprite->End();

				}
				
				

			}break;
		
			case PREVIEWMODE:
			{

				//PROCESS DATA RETRIEVED FROM INPUT
				/*g_Camera.Yaw(g_Input.m_MouseState.lX * 0.0025f);
				g_Camera.Roll(g_Input.m_MouseState.lZ * 0.0025f);*/
				
				if (g_Input.IsKeyDown(DIK_ESCAPE))
					gameState = MENU;
				
				if (g_Input.IsKeyDown(DIK_W))
					bAccel = true;

				if (g_Input.IsKeyDown(DIK_S))
					bDecel = true;

				if (g_Input.IsKeyDown(DIK_A))
				{
					g_Object.Roll(2.0f * deltatime);
					g_Camera.Roll(2.0f * deltatime);
										
				}
					
				if (g_Input.IsKeyDown(DIK_D))
				{
					g_Camera.Roll(-2.0f * deltatime);
					g_Object.Roll(-2.0f * deltatime);	
				}
					
				if (g_Input.IsKeyDown(DIK_UP))
				{
					g_Object.Pitch(2.0f * deltatime);
					g_Camera.Pitch(2.0f * deltatime);
				}
				if (g_Input.IsKeyDown(DIK_DOWN))
				{
					g_Object.Pitch(-2.0f * deltatime);
					g_Camera.Pitch(-2.0f * deltatime);
				}
				if (g_Input.IsKeyDown(DIK_LEFT))
				{	
					g_Camera.Yaw(-0.4f * deltatime);
					g_Object.Yaw(-0.4f * deltatime);
					
				}
				if (g_Input.IsKeyDown(DIK_RIGHT))
				{
					g_Camera.Yaw(0.4f * deltatime);	
					g_Object.Yaw(0.4f * deltatime);
				}
				static DWORD gunTimer = 0;
				if (g_Input.IsKeyDown(DIK_SPACE) && timeGetTime() - gunTimer > 500)
				{
					gunTimer = timeGetTime();
					D3DXVECTOR3 look, right, up;
					g_Object.GetLook(look);
					g_Object.GetRight(right);
					g_Object.GetUp(up);
					float x, y, z;
					g_Object.GetPosition(x, y, z);
					PlayerInfo[0].x = x;
					PlayerInfo[0].y = y;
					PlayerInfo[0].z = z;
					D3DXMATRIX R;
					R(0,0) = right.x; R(0,1) = right.y; R(0,2) = right.z; R(0,3) = 0.0f;
					R(1,0) = up.x; R(1,1) = up.y; R(1,2) = up.z; R(1,3) = 0.0f;
					R(2,0) = look.x; R(2,1) = look.y; R(2,2) = look.z; R(2,3) = 0.0f;
					R(3,0) = 0.0f; R(3,1) = 0.0f; R(3,2) = 0.0f; R(3,3) = 1.0f;

					CreateBullet(PlayerInfo[0].dpnidPlayer, R,
						x + look.x * 200 + right.x * 160 + up.x * -20, 
						y + look.y * 200 + right.y * 160 + up.y * -20, 
						z + look.z * 200 + right.z * 160 + up.z * -20, 
						look.x * 18000, 
						look.y * 18000,
						look.z * 18000);

					CreateBullet(PlayerInfo[0].dpnidPlayer, R,
						x + look.x * 200 + right.x * -160 + up.x * -20, 
						y + look.y * 200 + right.y * -160 + up.y * -20, 
						z + look.z * 200 + right.z * -160 + up.z * -20,
						look.x * 18000, 
						look.y * 18000,
						look.z * 18000);
				}
				if (g_Input.IsKeyDown(DIK_ADD))
				{
					offset += 100.0f * deltatime;
				}
				if (g_Input.IsKeyDown(DIK_SUBTRACT))
				{
					offset -= 100.0f * deltatime;
				}

				if (g_Input.IsKeyDown(DIK_TAB))
				{	
					height -= 100.0f * deltatime;

				}
				if (g_Input.IsKeyDown(DIK_RSHIFT))
				{
					height += 100.0f * deltatime;
				}
				

				if (g_Input.IsKeyDown(DIK_E))
				{
					InitDirectPlay(hwnd_app);
					TCPInit = true;
				}
				if(g_Input.IsKeyDown(DIK_R) && TCPInit == true && Join == false)
				{
					HostGame(hwnd_app);
					Hosting = true;
				}
				if(g_Input.IsKeyDown(DIK_T) && TCPInit == true && Hosting == false)
				{
					JoinGame(hwnd_app);
					Join = true;
				}
				if (g_Input.IsKeyDown(DIK_Q))
				{
					vCleanup();
					TCPInit = false;
					Hosting = false;
					Join = false;
				}
				if (g_Input.IsKeyDown(DIK_P))
				{
					sprintf(testmsg,"Whatz Up My Homie G Dog in thu house");
					
				}

				
				if (g_Input.IsKeyDown(DIK_1))
				{
					
					DWORD ID;
					gameState = LOADING;
					CreateThread(0,0, (LPTHREAD_START_ROUTINE)LoadLevel, (LPVOID)JEJOJO, 0, &ID);
					//g_planet = JEJOJO;

				}
				if (g_Input.IsKeyDown(DIK_2))
				{
					
					//InitPixelFog(100.0f, 34000.0f, g_clearColor);
					DWORD ID;
					gameState = LOADING;
					CreateThread(0,0, (LPTHREAD_START_ROUTINE)LoadLevel, (LPVOID)KEF_WID, 0, &ID);
					//g_planet = KEF_WID;
				}
				if (g_Input.IsKeyDown(DIK_3))
				{
					g_clearColor = D3DCOLOR_XRGB(190, 190, 190);
					//InitPixelFog(10.0f, 33000.0f, g_clearColor);
					DWORD ID;
					gameState = LOADING;
					CreateThread(0,0, (LPTHREAD_START_ROUTINE)LoadLevel, (LPVOID)IZE_PRIME, 0, &ID);
					//g_planet = IZE_PRIME;
				}
				if (g_Input.IsKeyDown(DIK_F) && !g_Input.WasKeyDown(DIK_F))
				{
					if (bFreeCam) bFreeCam = false;
					else bFreeCam = true;
				}

				if (g_Input.IsKeyDown(DIK_Y) && !g_Input.WasKeyDown(DIK_Y))
				{
					currShip--;
					if (currShip < 0) currShip = 1;
					
				}
				if (g_Input.IsKeyDown(DIK_H) && !g_Input.WasKeyDown(DIK_H))
				{
					currShip++;
					
					if (currShip >= NUM_SHIPS) currShip = 0;
					PlayerInfo[0].ship = currShip;
				}
				if (bAccel)
				{
					shipVelocity += 1000.0 * deltatime;
					if (shipVelocity > 5000) shipVelocity = 5000;
				}
				if (bDecel)
				{
					shipVelocity -= 1000.0 * deltatime;
					if (shipVelocity < 4000) shipVelocity = 4000;
				}
				
				//Move Camera
				g_Object.Walk(shipVelocity*deltatime);
				g_Camera.Walk(shipVelocity*deltatime);

				PlayerInfo[0].matrixMagic = g_Object.GetMatrix();

				D3DXMATRIX a;
				g_Object.GetPosition(x, y, z);
				D3DXVECTOR3 look, up, right;
				g_Object.GetLook(look);
				g_Object.GetUp(up);
				g_Object.GetRight(right);
				
				// UPDATE VIEW MATRIX
				offset = ShipInfo[currShip].offset;
				height = ShipInfo[currShip].height;
				D3DXVECTOR3 targetPos(x+look.x*offset + up.x*height, y+look.y*offset + up.y*height, z+look.z*offset + up.z*height);
				
				g_Camera.UpdateThirdPerson(targetPos, &V, deltatime);
				Device->SetTransform(D3DTS_VIEW, &V);

				//TERRAIN COLLISION TEST
				x_index = (int)    (x + CELL_SPACING * 64 * 4) / (CELL_SPACING * 64);
				z_index = (int)abs((z - CELL_SPACING * 64 * 4) / (CELL_SPACING * 64));

				int x_pos   = (int)(x + CELL_SPACING * 64 * 4) % (CELL_SPACING*64) - 0.5 * CELL_SPACING * 64;
				int z_pos   = (int)(z + CELL_SPACING * 64 * 4) % (CELL_SPACING*64) - 0.5 * CELL_SPACING * 64;

				float terrainHeight;
				if (x_index >= 0 && z_index < 8 && z_index >= 0 && z_index < 8)
				{
					terrainHeight = g_Terrain.GetHeight(z_index * 8 + x_index, x_pos, z_pos);
					if (y < terrainHeight + ShipInfo[currShip].radius)
						y = terrainHeight + ShipInfo[currShip].radius;
				}
				
				
				
				if (x < (int)(-CELL_SPACING * 64 * 3))
					x = (int)(-CELL_SPACING * 64 * 3);

				if (x > (int)(CELL_SPACING * 64 * 3))
					x = (int)(CELL_SPACING * 64 * 3);

				if (z < (int)(-CELL_SPACING * 64 * 3))
					z = (int)(-CELL_SPACING * 64 * 3);

				if (z > (int)(CELL_SPACING * 64 * 3))
					z = (int)(CELL_SPACING * 64 * 3);

				if (y > (int)(HEIGHT_SCALE * 255 * 4))
					y = (int)(HEIGHT_SCALE * 255 * 4);
				g_Object.SetPosition(x, y, z);
				
								
				UpdateBullets(deltatime);
				UpdateNetwork();
			}break;
			case SHIP_SELECTION_MENU:
			{
				if (g_Input.IsKeyDown(DIK_ESCAPE))
				{
					gameState = MENU;
				}
				if (g_Input.IsKeyDown(DIK_LEFT) && !g_Input.WasKeyDown(DIK_LEFT))
					currShip--;
				if (g_Input.IsKeyDown(DIK_RIGHT) && !g_Input.WasKeyDown(DIK_RIGHT))
					currShip++;
				if (currShip < 0) currShip = NUM_SHIPS - 1;
				else if (currShip >= NUM_SHIPS) currShip = 0;
				
			}break;

		}
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, g_clearColor, 1.0f, 0);
		Device->BeginScene();
		
		switch(gameState)
		{
			case LOADING:
				g_D3DXSprite->Begin(D3DXSPRITE_ALPHABLEND);
				g_spriteLoading.SetSourceRect(0, 0, 800, 600);
				g_spriteLoading.SetPosition(0, 0, 0);
				g_spriteLoading.SetCenter(0, 0);
				g_spriteLoading.Draw();
				g_Font.Print(0, 0, D3DCOLOR_XRGB(255,255,255), strLoadingOutput);
				g_D3DXSprite->End();
				break;
			case MENU:
			{
				g_D3DXSprite->Begin(D3DXSPRITE_ALPHABLEND);
				
				g_spriteMenu.SetPosition(0, 0, 0);
				g_spriteMenu.SetCenter(0, 0);
				g_spriteMenu.Draw(0);	
				
				for (int i = 0; i < 5; i++)
				{
					//g_Sprite.SetSourceRect(0, 600 + i * 64, 256, 600 + i * 64 + 64);
					int index = 1;
					if (bItemSelected[i])
						index = 2;
						
					g_spriteMenu.SetPosition(230, 230 + i * (28 + 20), 0);
					g_spriteMenu.Draw(i * 2 + index);
					
				}
				g_spriteCursor.SetPosition((int)cursorX, (int)cursorY, 0);
				
				g_Font.Print(0, 0, D3DCOLOR_XRGB(255,255,255), strLoadingOutput);
				g_spriteCursor.SetCenter(15, 15);
				g_spriteCursor.Draw();
				g_D3DXSprite->End();
			}break;

			case PREVIEWMODE:
			{
				D3DXMATRIX T;
				
				for (int i = 0; i < MAX_PLAYERS; i++)
				{
					if (PlayerInfo[i].bActive)
					{
						Device->SetTransform(D3DTS_WORLD, &PlayerInfo[i].matrixMagic);
						g_meshShip[PlayerInfo[i].ship].Render();

					}
				}
				/*Device->SetTransform(D3DTS_WORLD, &(g_Object.GetMatrix()));
				g_meshShip[currShip].Render(false);*/

				D3DXMatrixTranslation(&T, shipX, shipY, shipZ); 
				Device->SetTransform(D3DTS_WORLD, &T);
				g_meshShip[5].Render(true);
				
			
				// BEGIN RENDER TERRAIN  -------------------------------------------------
				Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
				Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
				for (int z = 0; z < NUM_PATCHES_PER_ROW; z++)
				{ 
					float zPatchSpacing = NUM_CELLS_PER_COL * CELL_SPACING;
					int zpos = (z * zPatchSpacing - zPatchSpacing * 3.5); // 64 * CELL_SPACING * (z - 3.5)
					for (int x = 0; x < NUM_PATCHES_PER_COL; x++)
					{
						float xPatchSpacing = NUM_CELLS_PER_ROW * CELL_SPACING;
						int xpos = x * xPatchSpacing - xPatchSpacing * 3.5;

						if (z >= z_index - 2 && z <= z_index + 2 && x >= x_index - 2 && x <= x_index+2)
						{
							D3DXMatrixTranslation(&T, xpos, 0.0f, -zpos);
							g_Terrain.Draw(z * NUM_PATCHES_PER_COL + x, &T, false);
						}
					}
				}
				Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
				Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

				DrawWaterMesh();
				// END RENDER TERRAIN ----------------------------------------------------
				
				DrawBullets();
				g_D3DXSprite->Begin(D3DXSPRITE_ALPHABLEND);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
				if(TCPInit == true)
				{
					g_Font.Print(0,0, D3DCOLOR_XRGB(255,0,0), "TCP Init");
					if(Hosting == true)
					{
						g_Font.Print(0,0, D3DCOLOR_XRGB(255,0,0), "Hosting");
					}
					if(Join == true)
					{
						g_Font.Print(0,0, D3DCOLOR_XRGB(255,0,0), "Joining");
					}
				}
				g_Font.Print(400, 0, 0xffffffff, dpOutput);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
				g_spriteCursor.SetCenter(15, 15);
				g_spriteCursor.Draw(0,SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
				g_D3DXSprite->End();
		
				//END COLLISION TEST
				
				
				
			}break;
			case SHIP_SELECTION_MENU:
			{
				D3DXMATRIX R, T;
				static float rotY = 0;
				rotY += 1.0f * deltatime;
				if (rotY >= D3DX_PI * 2) rotY -= D3DX_PI*2;
				D3DXMatrixRotationY(&R, rotY);
				D3DXMatrixTranslation(&T, 0, -100, 600.0);
				Device->SetTransform(D3DTS_WORLD, &(R*T));
				g_meshShip[currShip].Render();
			}break;
		}
		
		
		
		Device->EndScene();
		
		Device->Present(0, 0, 0, 0);
		//while(timeGetTime() - starttime < 16);
		
	}
	UpdateSoundSystem();
	//elapsed_time = GetTickCount() - starttime;
	return(1);
}

bool MouseOverButton(int cursorX, int cursorY, int btnX, int btnY, int btnWidth, int btnHeight)
{
	if (cursorX >= btnX && cursorX < btnX + btnWidth && 
		cursorY >= btnY && cursorY < btnY + btnHeight)
		return(true);
	return(false);
}

void InitBullets()
{
	D3DXMatrixRotationY(&bulletRot, 1.0 * D3DX_PI);
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		Bullets[i].bActive = false;
	}
}
//
//void CreateBullet(DPNID dpowner, D3DXMATRIX R, float x, float y, float z, float xv, float yv, float zv)
//{
//	int i = 0;
//	BULLET_PACKET pacBul;
//	void *packet;
//	
//	for (i = 0; i < MAX_BULLETS; i++)
//		if (!Bullets[i].bActive)
//			break;
//	if (i == MAX_BULLETS)
//		return;
//	Bullets[i].bActive = true;
//	Bullets[i].dpOwner = dpowner;
//	Bullets[i].x = x;
//	Bullets[i].y = y;
//	Bullets[i].z = z;
//	Bullets[i].xv = xv;
//	Bullets[i].yv = yv;
//	Bullets[i].zv = zv;
//	Bullets[i].life = 100.0;
//	Bullets[i].R = R;
//	PlaySound(sound[1]);
//		
//		
//	if (dpowner == g_dpnidLocalPlayer)
//	{
//		pacBul.size = sizeof(BULLET_PACKET);
//		pacBul.type = PACKET_TYPE_BULLET;
//		pacBul.bullet.bActive = 1;
//		pacBul.bullet.dpOwner = g_dpnidLocalPlayer;
//		pacBul.bullet.x = x;
//		pacBul.bullet.y = y;
//		pacBul.bullet.z = z;
//		pacBul.bullet.xv = xv;
//		pacBul.bullet.yv = yv;
//		pacBul.bullet.zv = zv;
//		pacBul.bullet.life = 100.0;
//		pacBul.bullet.R = R;
//		packet = (VOID*)&pacBul;
//		SendPeerMessage(-1, packet);
//	}
//	/*if (i >= MAX_BULLETS)
//		return;*/
//
//	
//
//}

void UpdateBullets(float deltatime)
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (Bullets[i].bActive)
		{
			Bullets[i].life -= 100.0 * deltatime;
			if (Bullets[i].life < 0) Bullets[i].bActive = false;
			Bullets[i].x += Bullets[i].xv * deltatime;
			Bullets[i].y += Bullets[i].yv * deltatime;
			Bullets[i].z += Bullets[i].zv * deltatime;
			for (int j = 1; j < 6; j++)
			{
				float x = Bullets[i].x - PlayerInfo[j].x;
				float y = Bullets[i].y - PlayerInfo[j].y;
				float z = Bullets[i].z - PlayerInfo[j].z;
				float dist = sqrt(x*x+y*y+z*z);
				if (dist < ShipInfo[0].radius)
				{
					PlaySound(sound[2]);
					Bullets[i].bActive = false;
					break;
				}
			}
		}
	}
}

void DrawBullets()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		if (Bullets[i].bActive)
		{
			D3DXMATRIX T;
			D3DXMatrixTranslation(&T, Bullets[i].x, Bullets[i].y, Bullets[i].z);
			Device->SetTransform(D3DTS_WORLD, &(Bullets[i].R * T));
			Device->SetMaterial(&BULLET_MTRL);
			Device->SetTexture(0,0);
			meshCylinder->DrawSubset(0);
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

int LoadSound(char filename[], FMOD::Sound **sound, FMOD_MODE mode)
{
	if (pfmodSystem->createSound(filename, mode, 0, sound) != FMOD_OK)
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

void InitWaterMesh()
{
	D3DXCreateMeshFVF(2, 4, D3DXMESH_MANAGED, D3DFVF_XYZTEX1, Device, &g_pWaterMesh);
	VertexTex1 *v;
	g_pWaterMesh->LockVertexBuffer(0, (void**)&v);
	v[0] = VertexTex1(-(CELL_SPACING * NUM_CELLS_PER_ROW * 4), 62.5 * HEIGHT_SCALE, -(CELL_SPACING * NUM_CELLS_PER_COL * 4),     0, 500000);
	v[1] = VertexTex1(-(CELL_SPACING * NUM_CELLS_PER_ROW * 4), 62.5 * HEIGHT_SCALE,  (CELL_SPACING * NUM_CELLS_PER_COL * 4),     0,     0);
	v[2] = VertexTex1( (CELL_SPACING * NUM_CELLS_PER_ROW * 4), 62.5 * HEIGHT_SCALE,  (CELL_SPACING * NUM_CELLS_PER_COL * 4), 500000,     0);
	v[3] = VertexTex1( (CELL_SPACING * NUM_CELLS_PER_ROW * 4), 62.5 * HEIGHT_SCALE, -(CELL_SPACING * NUM_CELLS_PER_COL * 4), 500000, 500000);
	g_pWaterMesh->UnlockVertexBuffer();

	WORD *i;
	g_pWaterMesh->LockIndexBuffer(0, (void**)&i);
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;
	g_pWaterMesh->UnlockVertexBuffer();

	D3DXCreateTextureFromFile(Device, "water.dds", &g_pWaterTex);
}

void DrawWaterMesh()
{
	D3DXMATRIX T;
	D3DXMatrixIdentity(&T);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_LIGHTING, false);
	Device->SetTransform(D3DTS_WORLD, &T);
	Device->SetTexture(0, g_pWaterTex);
	g_pWaterMesh->DrawSubset(0);
	Device->SetRenderState(D3DRS_LIGHTING, true);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
}

void DestroyWaterMesh()
{
	SAFE_RELEASE(g_pWaterTex);
	SAFE_RELEASE(g_pWaterMesh);
}

void UpdateNetwork()
{
	void *packet;
	MAGICMATRIX_PACKET pacMagicMatrix;
	SHIP_PACKET pacShip;
	POSITION_PACKET pacPos;
	static DWORD updateTime = timeGetTime();
	if (timeGetTime() > updateTime + 100)
	{
		if (g_lNumberOfActivePlayers > 1)
		{
			pacMagicMatrix.size = sizeof(MAGICMATRIX_PACKET);
			pacMagicMatrix.type = PACKET_TYPE_MAGICMATRIX;
			pacMagicMatrix.magicMatrix = PlayerInfo[0].matrixMagic;
			packet = (VOID*)&pacMagicMatrix;
			SendPeerMessage(-1, packet);


			pacShip.size = sizeof(MAGICMATRIX_PACKET);
			pacShip.type = PACKET_TYPE_SHIP;
			pacShip.ship = PlayerInfo[0].ship;
			packet = (VOID*)&pacShip;
			SendPeerMessage(-1, packet);

			pacPos.size = sizeof(POSITION_PACKET);
			pacPos.type = PACKET_TYPE_POSITION;
			pacPos.x = PlayerInfo[0].x;
			pacPos.y = PlayerInfo[0].y;
			pacPos.z = PlayerInfo[0].z;
			packet = (VOID*)&pacPos;
			SendPeerMessage(-1, packet);

		}
	}
}
