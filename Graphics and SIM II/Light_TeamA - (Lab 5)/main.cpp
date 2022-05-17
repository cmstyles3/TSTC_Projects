// Collin Munson & Justin Knox

#include <stdio.h>
#include <stdlib.h>
#include "d3dbase.h"
#include "input.h"

// Defines
//---------------------------------------------------------------
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MURDER Release

enum{TABLE, CHAIR, TEAPOT, BALL, BOOKSHELF, TVSTAND, CARDS, GAMECASE };

struct
{
	int num_parts;
	D3DXMATRIX local[16];
	D3DMATERIAL9 mtrl[16];
	LPD3DXMESH mesh[16];
}Object[16];

// Function Prototypes
//------------------------------------------------------------------------------------------------------------------------
int  GameMain(float);

int  GameInit();

void GameMurder();

void DrawObject(int index, float x, float y, float z, float yaw, float pitch, float roll);
//------------------------------------------------------------------------------------------------------------------------


// Globals
//-----------------------------------------------------------------------------------------------------------------------------

LPDIRECT3DDEVICE9 Device = 0;

LPD3DXFONT Font = 0;

LPDIRECT3DVERTEXBUFFER9 WALL = 0;

LPD3DXMESH teapot, Box, Box2, Wall;
D3DLIGHT9 spot, Point;
D3DMATERIAL9 Mtrl;
Input input;

D3DXMATRIX ObjMatrices[20];

HINSTANCE hinstance_app;
HWND hwnd_app;

char Buffer[80];

//custom materials for objects
const D3DMATERIAL9 FLOOR_MTRL = InitMtrl(MAGENTA, MAGENTA, RED, BLACK, 2.0f);
const D3DMATERIAL9 WALL_MTRL = InitMtrl(RED, YELLOW, PINK, BLACK, 2.0f);
const D3DMATERIAL9 BOOKSHELF_MTRL = InitMtrl(GREEN, YELLOW, YELLOW, BLACK, 2.0f);
const D3DMATERIAL9 CHAIR_MTRL = InitMtrl(BLUE, CYAN, CYAN, BLACK, 2.0f);
const D3DMATERIAL9 TVSTAND_MTRL = InitMtrl(BLUE, GREEN, GREEN, BLACK, 2.0f);

//-------------------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
				SendMessage(hwnd,WM_DESTROY,wparam,lparam);
			break;
	}
	return(DefWindowProc(hwnd,msg,wparam,lparam));
}


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int nshowcmd)
{
	//MessageBox(0, "Consult readme.txt for controls", "INSTRUCTIONS", 0);

	if (!InitD3D(hinstance, SCREEN_WIDTH, SCREEN_HEIGHT, false, D3DDEVTYPE_HAL, &Device))
	{
		MessageBox(0,"InitD3D() - FAILED", 0, 0);
		return(0);
	}
	hinstance_app = hinstance;
	if (!input.initDI())
	{
		MessageBox(0, "could not initialize direct input", 0, 0);
		return(0);
	}

	if (!input.initKB())
	{
		MessageBox(0, "could not initialize keyboard", 0, 0);
		return(0);
	}

	if (!input.initmouse())
	{
		MessageBox(0, "could not initialize mouse", 0, 0);
		return(0);
	}
	
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

int GameMain(float deltatime)
{
	if (Device)
	{

		static float theta = 0.0f, phi = 0.0f;
		static float posX = -20.0f, posZ = -20.0f;
		static float dirX = 0.0f, dirY = 0.0f, dirZ = 0.0f;
		float posY = 2.0f;
		float velX = 0.0f, velZ = 0.0f;
		static bool lightOn = false;
		static bool flashLightOn = false;

// INPUT & LOGIC ///////////////////////////////////////////////////////////////////

		//read mouse input -----------------
		input.readmouse();
		theta += mouse_state.lX * .0025f;
		phi -= mouse_state.lY * .0025f;

		
		//read keyboard input ---------------
		input.readKB();
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
			velX += (sinf(theta) * 22.0f) * deltatime;
			velZ += (cosf(theta) * 22.0f) * deltatime;
		}
		if (kb_state[DIK_S])
		{
			velX += -(sinf(theta) * 22.0f) * deltatime;
			velZ += -(cosf(theta) * 22.0f) * deltatime;
		}
		if (kb_state[DIK_A])
		{
			velX += -(cosf(theta) * 22.0f) * deltatime;
			velZ +=  (sinf(theta) * 22.0f) * deltatime;
		}
		if (kb_state[DIK_D])
		{
			velX +=  (cosf(theta) * 22.0f) * deltatime;
			velZ += -(sinf(theta) * 22.0f) * deltatime;
		}
		if (kb_state[DIK_F] && !prev_kb_state[DIK_F])
		{
			if (flashLightOn)
			{
				flashLightOn = false;
				Device->LightEnable(0, false);
			}
			else
			{
				flashLightOn = true;
				Device->LightEnable(0, true);
			}
		}
		if (kb_state[DIK_C])
		{
			posY -= 11.0f;
		}
		
		if (kb_state[DIK_SPACE] && !prev_kb_state[DIK_SPACE])
		{
			if (posX >= -2.0f && posX <= 2.0f && posZ >= 33.0f && posZ <= 39.0f &&
				dirZ >= 0.5f && dirZ <= 1.0f && dirY >= -0.75f && dirY <= 0.25f)
			{
				if (lightOn) 
				{
					lightOn = false;
					Device->LightEnable(1, false);
				}
				else 
				{
					lightOn = true;
					Device->LightEnable(1, true);
				}
			}
		}
// LOGIC ///////////////////////////////////////////////////////////////////
		
		posX += velX;
		posZ += velZ;
		if (posX <= -34.0f || posX >= 34.0f)
			posX -= velX;
		if (posZ <= -34.0f || posZ >= 34.0f)
			posZ -= velZ;
		if (theta > 6.28f)
			theta -= 6.28f;
		else if (theta < 0.0f)
			theta += 6.28f;

		if (phi > 1.57f)
			phi = 1.57f;
		else if (phi < -1.57f)
			phi = -1.57f;

		
		// set view matrix
		//-------------------------------------------------------------
	
		dirX = (cosf(phi) * sinf(theta));
		dirY = (sinf(phi)              );
		dirZ = (cosf(phi) * cosf(theta));

		spot.Direction.x = dirX;
		spot.Direction.y = dirY;
		spot.Direction.z = dirZ;
		
		spot.Position.x = posX;
		spot.Position.y = posY;
		spot.Position.z = posZ;

		D3DXVECTOR3 position(posX, posY, posZ);
		
		D3DXVECTOR3 target(posX + dirX, posY + dirY, posZ + dirZ);
		
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);
	
		Device->SetLight(0, &spot);
		
		//------------------------------------------------------------
	
	
// RENDER ///////////////////////////////////////////////////////////////////////////////////////////////////		

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
		Device->BeginScene();
		Device->SetMaterial(&SILVER_MTRL);
		
		D3DXMATRIX S,S2;
		D3DXMATRIX R, T;
		D3DXMatrixIdentity(&T);
		
		D3DXMatrixTranslation(&(T), 0.0f, -8.0f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &(T));
		//teapot->DrawSubset(0);
		
		//Draw Table
		DrawObject(TABLE, 20.0f, -20.0f, -20.0f, 0.0f, 0.0f, 0.0f);

		//Position and Draw chairs
		//4 around table
		DrawObject(CHAIR, 20.0f, -18.0f, -10.0f, 0.0f, 0.0f, 0.0f);
		DrawObject(CHAIR, 10.0f, -18.0f, -20.0f, D3DX_PI * -0.5f, 0.0f, 0.0f);
		DrawObject(CHAIR, 30.0f, -18.0f, -20.0f, D3DX_PI * 0.5f, 0.0f, 0.0f);
		DrawObject(CHAIR, 20.0f, -18.0f, -30.0f, D3DX_PI, 0.0f, 0.0f);
		
		//1 in front of TV
		DrawObject(CHAIR, -15.0f, -18.0f, 15.0f, D3DX_PI * 0.75f, 0.0f, 0.0f);


		//Position and Draw a couple balls
		Object[BALL].mtrl[0] = GREEN_MTRL;
		DrawObject(BALL, -30.0f, -18.0f, -25.0f, 0.0f, 0.0f, 0.0f);
		Object[BALL].mtrl[0] = BLUE_MTRL;
		DrawObject(BALL, -35.0f, -18.0f, -28.0f, 0.0f, 0.0f, 0.0f);

		DrawObject(TEAPOT, 20.0f, -8.6f, -20.0f, D3DX_PI * -.25f, 0.0f, 0.0f);

		DrawObject(BOOKSHELF, 20.0f, -9.0f, 38.0f, 0.0f, 0.0f, 0.0f);

		//Position and Draw TV & STAND which also includes an xbox 360
		DrawObject(TVSTAND, -30.0f, -9.0f, 30.0f, D3DX_PI * -.25f, 0.0f, 0.0f);
		

		//Position and Draw two a group of gamecases
		DrawObject(GAMECASE, -30.0f, -13.5f, 30.0f, 0.0f, 0.0f, 0.0f);
		DrawObject(GAMECASE, -35.0f, -19.5f, 18.0f, 0.0f, 0.0f, 0.0f);

		//Position and Draw 4 groups of cards
		DrawObject(CARDS, 20.0f, -8.9f, -20.0f, D3DX_PI * -.25f, 0.0f, 0.0f );
		DrawObject(CARDS, 20.0f, -8.9f, -20.0f, D3DX_PI * .25f, 0.0f, 0.0f );
		DrawObject(CARDS, 20.0f, -8.9f, -20.0f, D3DX_PI * .75f, 0.0f, 0.0f );
		DrawObject(CARDS, 20.0f, -8.9f, -20.0f, D3DX_PI * -.75f, 0.0f, 0.0f );

		//Draw Light Switch ---------------------------------
		Device->SetMaterial(&WHITE_MTRL);
		D3DXMatrixTranslation(&S, 0.0f, 0.0f, 39.75f);
		Device->SetTransform(D3DTS_WORLD, &S);
		Box->DrawSubset(0);
		Device->SetMaterial(&RED_MTRL);
		D3DXMATRIX R2;
		if (lightOn)
			D3DXMatrixRotationX(&R2, D3DX_PI * .25);
		else D3DXMatrixRotationX(&R2, D3DX_PI * -.25);
		D3DXMatrixTranslation(&S2, 0.0f, 0.0f, -0.2f);
		D3DXMATRIX T3;
		D3DXMatrixTranslation(&T3, 0.0f, 0.0f, -0.32f);
		Device->SetTransform(D3DTS_WORLD, &(T3*R2 * S * S2));
		Box2->DrawSubset(0);
		//End Light Switch ------------------------------------

		
		//Room ----------------------------------------
		//Draw Walls
		Device->SetMaterial(&WALL_MTRL);
		D3DXMatrixTranslation(&T, 0.0f, 0.0f, 40.0f);
		Device->SetTransform(D3DTS_WORLD, &(T));
		Wall->DrawSubset(0);
		D3DXMatrixRotationY(&R, D3DX_PI * .50f);
		Device->SetTransform(D3DTS_WORLD, &(T * R));
		Wall->DrawSubset(0);
		D3DXMatrixRotationY(&R, D3DX_PI);
		Device->SetTransform(D3DTS_WORLD, &(T * R));
		Wall->DrawSubset(0);
		D3DXMatrixRotationY(&R, D3DX_PI * 1.50f);
		Device->SetTransform(D3DTS_WORLD, &(T * R));
		Wall->DrawSubset(0);

		//Draw Floor
		Device->SetMaterial(&FLOOR_MTRL);
		D3DXMatrixTranslation(&T, 0.0f, -20.0f, 20.0f);
		D3DXMatrixRotationX(&R, D3DX_PI * .50f);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Wall->DrawSubset(0);
		D3DXMatrixTranslation(&T, 0.0f, -20.0f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Wall->DrawSubset(0);
		D3DXMatrixTranslation(&T, 0.0f, -20.0f, -20.0f);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Wall->DrawSubset(0);
		
		//Draw Ceiling
		Device->SetMaterial(&WHITE_MTRL);
		D3DXMatrixTranslation(&T, 0.0f, 20.0f, 20.0f);
		D3DXMatrixRotationX(&R, D3DX_PI * 1.50f);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Wall->DrawSubset(0);
		D3DXMatrixTranslation(&T, 0.0f, 20.0f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Wall->DrawSubset(0);
		D3DXMatrixTranslation(&T, 0.0f, 20.0f, -20.0f);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Wall->DrawSubset(0);
		//End Room --------------------------------------------

		RECT rect;
		rect.top = 100;
		rect.bottom = 150;
		rect.left = 160;
		rect.right = 660;
		
		if (posX >= -2.0f && posX <= 2.0f && posZ >= 33.0f && posZ <= 39.0f &&
				dirZ >= 0.5f && dirZ <= 1.0f && dirY >= -0.75f && dirY <= 0.25f)
		{	
			if (lightOn)
				Font->DrawText(NULL,"Press Space Bar to Flip Light Switch Off",-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(0,130,255));
			else Font->DrawText(NULL,"Press Space Bar to Flip Light Switch On",-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(0,130,255));
		}
		Device->EndScene();
		
		Device->Present(0, 0, 0, 0);
		
	}
	return(1);
}

void DrawObject(int index, float x, float y, float z, float yaw, float pitch, float roll)
{
	D3DXMATRIX T, R, S;
	D3DXMatrixTranslation(&T, x, y, z);
	D3DXMatrixRotationYawPitchRoll(&R, yaw, pitch, roll);
	
	for (int i = 0; i < Object[index].num_parts; i++)
	{
		Device->SetMaterial(&Object[index].mtrl[i]);
		Device->SetTransform(D3DTS_WORLD, &(Object[index].local[i] * R * T));
		Object[index].mesh[i]->DrawSubset(0);
	}
}

int GameInit()
{
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
	D3DXCreateFontIndirect(Device, &FontStruct, &Font);
	
	D3DXCreateBox(Device, 2.0f, 2.0f, 0.5f, &Box, 0);
	D3DXCreateBox(Device, 0.3f, 0.3f, 0.75f, &Box2, 0);

	D3DXMATRIX temp, temp2;

	//Define Table -----------------------------------------------------------
	Object[0].num_parts = 5;

	//Table Top
	D3DXCreateCylinder(Device, 10.0f, 10.0f, 1.0f, 20, 4, &Object[0].mesh[0], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp2, 0.0f, 10.0f, 0.0f);
	Object[0].local[0] = temp * temp2;
	Object[0].mtrl[0] = BLUE_MTRL;

	//Table Legs
	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[1], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp2, 5.0f, 5.0f, 5.0f);
	Object[0].local[1] = temp * temp2;
	Object[0].mtrl[1] = BLUE_MTRL;

	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[2], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp2, -5.0f, 5.0f, 5.0f);
	Object[0].local[2] = temp * temp2;
	Object[0].mtrl[2] = BLUE_MTRL;

	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[3], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp2, -5.0f, 5.0f, -5.0f);
	Object[0].local[3] = temp * temp2;
	Object[0].mtrl[3] = BLUE_MTRL;

	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[4], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp2, 5.0f, 5.0f, -5.0f);
	Object[0].local[4] = temp * temp2;
	Object[0].mtrl[4] = BLUE_MTRL;
	//End Table ----------------------------------------------------

	//Define Chair
	Object[1].num_parts = 6;

	D3DXCreateBox(Device, 5.0f, 6.0f, 1.0f, &Object[1].mesh[0], 0);
	D3DXMatrixTranslation(&Object[1].local[0], 0.0f, 5.5f, 2.5f);
	Object[1].mtrl[0] = CHAIR_MTRL;

	D3DXCreateBox(Device, 5.0f, 2.0f, 4.0f, &Object[1].mesh[1], 0);
	D3DXMatrixTranslation(&Object[1].local[1], 0.0f, 3.5f, 0.0f);
	Object[1].mtrl[1] = CHAIR_MTRL;

	D3DXCreateBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[2], 0);
	D3DXMatrixTranslation(&Object[1].local[2], 1.5f, 0.5f, 2.5f);
	Object[1].mtrl[2] = CHAIR_MTRL;

	D3DXCreateBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[3], 0);
	D3DXMatrixTranslation(&Object[1].local[3], -1.5f, 0.5f, 2.5f);
	Object[1].mtrl[3] = CHAIR_MTRL;

	D3DXCreateBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[4], 0);
	D3DXMatrixTranslation(&Object[1].local[4], -1.5f, 0.5f, -1.5f);
	Object[1].mtrl[4] = CHAIR_MTRL;

	D3DXCreateBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[5], 0);
	D3DXMatrixTranslation(&Object[1].local[5], 1.5f, 0.5f, -1.5f);
	Object[1].mtrl[5] = CHAIR_MTRL;
	//end chair

	//teapot
	Object[2].num_parts = 1;
	D3DXCreateTeapot(Device, &Object[2].mesh[0], 0);
	D3DXMatrixIdentity(&Object[2].local[0]);
	Object[2].mtrl[0] = SILVER_MTRL;
	//

	//ball
	Object[3].num_parts = 1;
	D3DXCreateSphere(Device, 2.0f, 20, 20, &Object[3].mesh[0], 0);
	D3DXMatrixIdentity(&Object[3].local[0]);
	Object[3].mtrl[0] = RED_MTRL;

	//bookshelf
	Object[4].num_parts = 8;
	D3DXCreateBox(Device, 15.0f, 22.0f, 0.5f, &Object[4].mesh[0], 0);
	D3DXMatrixIdentity(&Object[4].local[0]);
	Object[4].mtrl[0] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 15.0f, 0.5f, 5.0f, &Object[4].mesh[1], 0);
	D3DXMatrixTranslation(&Object[4].local[1], 0.0f, 10.75f, -2.75f);
	Object[4].mtrl[1] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 15.0f, 0.5f, 5.0f, &Object[4].mesh[2], 0);
	D3DXMatrixTranslation(&Object[4].local[2], 0.0f, -10.75f, -2.75f);
	Object[4].mtrl[2] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 0.5f, 22.0f, 5.5f, &Object[4].mesh[3], 0);
	D3DXMatrixTranslation(&Object[4].local[3], 7.5f, 0.0f, -2.5f);
	Object[4].mtrl[3] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 0.5f, 22.0f, 5.5f, &Object[4].mesh[4], 0);
	D3DXMatrixTranslation(&Object[4].local[4], -7.5f, 0.0f, -2.5f);
	Object[4].mtrl[4] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 14.5f, 0.5f, 5.0f, &Object[4].mesh[5], 0);
	D3DXMatrixTranslation(&Object[4].local[5], 0.0f, 3.58f, -2.75f);
	Object[4].mtrl[5] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 14.5f, 0.5f, 5.0f, &Object[4].mesh[6], 0);
	D3DXMatrixTranslation(&Object[4].local[6], 0.0f, -3.58f, -2.75f);
	Object[4].mtrl[6] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 1.0f, 5.0f, 4.0f, &Object[4].mesh[7], 0);
	D3DXMatrixRotationZ(&temp, D3DX_PI * 0.15f);
	D3DXMatrixTranslation(&Object[4].local[7], -5.6f, -1.0f, -2.75f);
	Object[4].local[7] = temp * Object[4].local[7];
	Object[4].mtrl[7] = RED_MTRL;
	
	//TV & STAND
	//tv box
	Object[5].num_parts = 11;
	D3DXCreateBox(Device, 16.0f, 9.0f, 4.5f, &Object[5].mesh[0], 0);
	D3DXMatrixTranslation(&Object[5].local[0], 0.0f, 2.25f, 0.0f);
	Object[5].mtrl[0] = SILVER_MTRL;

	//tv screen
	D3DXCreateBox(Device, 12.8f, 7.2f, 0.1f, &Object[5].mesh[1], 0);
	D3DXMatrixTranslation(&Object[5].local[1], 0.0f, 2.25f, -2.25f);
	Object[5].mtrl[1] = BLACK_MTRL;

	//tv stand
	D3DXCreateBox(Device, 18.0f, 0.5f, 5.0f, &Object[5].mesh[2], 0);
	D3DXMatrixTranslation(&Object[5].local[2], 0.0f, -2.5f, 0.0f);
	Object[5].mtrl[2] = TVSTAND_MTRL;

	D3DXCreateBox(Device, 18.0f, 0.5f, 5.0f, &Object[5].mesh[3], 0);
	D3DXMatrixTranslation(&Object[5].local[3], 0.0f, -6.5f, 0.0f);
	Object[5].mtrl[3] = TVSTAND_MTRL;

	D3DXCreateBox(Device, 18.0f, 0.5f, 5.0f, &Object[5].mesh[4], 0);
	D3DXMatrixTranslation(&Object[5].local[4], 0.0f, -10.5f, 0.0f);
	Object[5].mtrl[4] = TVSTAND_MTRL;
	

	D3DXCreateBox(Device, 0.5f, 9.0f, 5.5f, &Object[5].mesh[5], 0);
	D3DXMatrixTranslation(&Object[5].local[5], 9.25f, -6.5f, 0.0f);
	Object[5].mtrl[5] = TVSTAND_MTRL;

	D3DXCreateBox(Device, 0.5f, 9.0f, 5.5f, &Object[5].mesh[6], 0);
	D3DXMatrixTranslation(&Object[5].local[6], -9.25f, -6.5f, 0.0f);
	Object[5].mtrl[6] = TVSTAND_MTRL;

	//define xbox 360 video game box
	D3DXCreateBox(Device, 6.0f, 1.5f, 4.0f, &Object[5].mesh[7], 0);
	D3DXMatrixTranslation(&Object[5].local[7], 0.0f, -5.5f, 0.0f);
	Object[5].mtrl[7] = WHITE_MTRL;

	//define power brick
	D3DXCreateBox(Device, 2.0f, 1.25f, 4.0f, &Object[5].mesh[8], 0);
	D3DXMatrixTranslation(&Object[5].local[8], 0.0f, -9.5f, 0.0f);
	Object[5].mtrl[8] = BLACK_MTRL;

	//ring of light
	D3DXCreateTorus(Device, 0.03f, 0.4f, 3, 10, &Object[5].mesh[9], 0);
	D3DXMatrixScaling(&temp, 1.0f, 1.0f, 0.1f);
	D3DXMatrixTranslation(&Object[5].local[9], 1.5f, -5.5, -2.0f);
	Object[5].local[9] = temp * Object[5].local[9];
	Object[5].mtrl[9] = XBOX_MTRL;

	D3DXCreateSphere(Device, 0.1f, 15, 15, &Object[5].mesh[10], 0);
	D3DXMatrixTranslation(&Object[5].local[10], -0.4f, -9.5f, -2.0f);
	Object[5].mtrl[10] = XBOX_MTRL;
	
	//cards
	Object[6].num_parts = 5;
	D3DXCreateBox(Device, 1.0f, 0.1f, 1.5f, &Object[6].mesh[0], 0);
	D3DXMatrixRotationY(&temp, D3DX_PI * 0.75f);
	D3DXMatrixTranslation(&Object[6].local[0], -5.0f, -0.5f, 5.0f);
	Object[6].local[0] = temp * Object[6].local[0];
	Object[6].mtrl[0] = RED_MTRL;

	D3DXCreateBox(Device, 1.0f, 0.1f, 1.5f, &Object[6].mesh[1], 0);
	D3DXMatrixRotationY(&temp, D3DX_PI * 0.65f);
	D3DXMatrixTranslation(&Object[6].local[1], -5.0f, -0.5f, 7.0f);
	Object[6].local[1] = temp * Object[6].local[1];
	Object[6].mtrl[1] = RED_MTRL;

	D3DXCreateBox(Device, 1.0f, 0.1f, 1.5f, &Object[6].mesh[2], 0);
	D3DXMatrixRotationY(&temp, D3DX_PI * 0.85f);
	D3DXMatrixTranslation(&Object[6].local[2], -5.5f, -0.5f, 3.5f);
	Object[6].local[2] = temp * Object[6].local[2];
	Object[6].mtrl[2] = RED_MTRL;

	D3DXCreateBox(Device, 1.5f, 0.1f, 1.0f, &Object[6].mesh[3], 0);
	D3DXMatrixRotationY(&temp, D3DX_PI * 0.25f);
	D3DXMatrixTranslation(&Object[6].local[3], -3.0f, -0.5f, 5.0f);
	Object[6].local[3] = temp * Object[6].local[3];
	Object[6].mtrl[3] = RED_MTRL;

	D3DXCreateBox(Device, 1.5f, 0.1f, 1.0f, &Object[6].mesh[4], 0);
	D3DXMatrixRotationY(&temp, D3DX_PI * 0.35f);
	D3DXMatrixTranslation(&Object[6].local[4], -7.0f, -0.5f, 3.0f);
	Object[6].local[4] = temp * Object[6].local[4];
	Object[6].mtrl[4] = RED_MTRL;

	//Game cases
	Object[7].num_parts = 4;
	D3DXCreateBox(Device, 2.5f, 0.25f, 3.75f, &Object[7].mesh[0], 0);
	D3DXMatrixTranslation(&Object[7].local[0], 0.0f, 0.0f, 0.0f);
	Object[7].mtrl[0] = GREEN_MTRL;

	D3DXCreateBox(Device, 2.5f, 0.28f, 3.72f, &Object[7].mesh[1], 0);
	D3DXMatrixTranslation(&Object[7].local[1], -0.07f, 0.0f, 0.0f);
	Object[7].mtrl[1] = BLUE_MTRL;

	//Game cases
	//Object[7].num_parts = 2;
	D3DXCreateBox(Device, 2.5f, 0.25f, 3.75f, &Object[7].mesh[2], 0);
	D3DXMatrixRotationY(&temp, D3DX_PI * 0.25f);
	D3DXMatrixTranslation(&Object[7].local[2], 1.25f, 0.25f, 0.0f);
	Object[7].local[2] = temp * Object[7].local[2];
	Object[7].mtrl[2] = GREEN_MTRL;

	D3DXCreateBox(Device, 2.5f, 0.28f, 3.70f, &Object[7].mesh[3], 0);
	D3DXMatrixTranslation(&Object[7].local[3], 1.18f, 0.25f, 0.05f);
	Object[7].local[3] = temp * Object[7].local[3];
	Object[7].mtrl[3] = RED_MTRL;


	//Room

	D3DXCreateMeshFVF(64, 45, D3DXMESH_MANAGED, D3DFVF_CUSTOM, Device, &Wall);
	Vertex* v;
	Wall->LockVertexBuffer(0, (void **)&v);
	v[0] = Vertex(-40.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[1] = Vertex(-30.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[2] = Vertex(-20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[3] = Vertex(-10.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[4] = Vertex(  0.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[5] = Vertex( 10.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[6] = Vertex( 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[7] = Vertex( 30.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[8] = Vertex( 40.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f);

	v[9] = Vertex(-40.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[10] = Vertex(-30.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[11] = Vertex(-20.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[12] = Vertex(-10.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[13] = Vertex(  0.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[14] = Vertex( 10.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[15] = Vertex( 20.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[16] = Vertex( 30.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[17] = Vertex( 40.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f);

	v[18] = Vertex(-40.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[19] = Vertex(-30.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[20] = Vertex(-20.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[21] = Vertex(-10.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[22] = Vertex(  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[23] = Vertex( 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[24] = Vertex( 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[25] = Vertex( 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[26] = Vertex( 40.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);

	v[27] = Vertex(-40.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[28] = Vertex(-30.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[29] = Vertex(-20.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[30] = Vertex(-10.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[31] = Vertex(  0.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[32] = Vertex( 10.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[33] = Vertex( 20.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[34] = Vertex( 30.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[35] = Vertex( 40.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	
	v[36] = Vertex(-40.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[37] = Vertex(-30.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[38] = Vertex(-20.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[39] = Vertex(-10.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[40] = Vertex(  0.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[41] = Vertex( 10.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[42] = Vertex( 20.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[43] = Vertex( 30.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	v[44] = Vertex( 40.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f);
	
	Wall->UnlockVertexBuffer();

	WORD* i = 0;
	Wall->LockIndexBuffer(0, (void **)&i);

	for (int j = 0; j < 32; j++)
	{
		int index = j * 6;
		int k = j / 8 + j;
		i[index] = 9 + k; i[index + 1] = 0 + k; i[index + 2] = 1 + k;
		i[index + 3] = 9 + k; i[index + 4] = 1 + k; i[index + 5] = 10 + k;
	}

	Wall->UnlockIndexBuffer();
	
	D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);
	D3DXCOLOR c(WHITE);
	spot = InitSpotLight(&pos,&dir, &c);

	Device->SetLight(0, &spot);

	D3DXVECTOR3 pos2(0.0f, 3.0f, 0.0f);
	Point = InitPointLight(&pos2, &c);

	Device->SetLight(1, &Point);
	
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);


	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
	
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.35f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	
	Device->SetRenderState(D3DRS_LIGHTING, false);
	return(1);
}

//kill everything, make sure there is no survivors
void GameMurder()
{
	if (Font)
	{
		Font->MURDER();
		Font = 0;
	}
	if (Wall)
	{
		Wall->MURDER();
		Wall = 0;
	}
	if (Box)
	{
		Box->MURDER();
		Box = 0;
	}
	if (Box2)
	{
		Box2->MURDER();
		Box2 = 0;
	}
	//kill objects
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			if (Object[i].mesh[j])
			{
				Object[i].mesh[j]->MURDER();
				Object[i].mesh[j] = 0;
			}
		}
	}
	
	//kill direct input
	input.killmouse();
	input.killKB();
	input.killDI();

	//kill device
	if (Device)
	{
		Device->MURDER();
		Device = 0;
	}
}