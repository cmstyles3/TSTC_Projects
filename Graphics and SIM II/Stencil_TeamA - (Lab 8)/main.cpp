// Collin Munson & Jarred Nealey

#include <stdio.h>
#include <stdlib.h>
#include "d3dbase.h"
#include "input.h"

// Defines
//---------------------------------------------------------------
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MURDER  Release
#define DESTROY Release

#define MAX_TEXTURES 32

enum{TABLE, CHAIR, TEAPOT, BALL, BOOKSHELF, TVSTAND, CARDS, GAMECASE };

struct Object_type
{
	float x, y, z;
	float pitch, yaw, roll;
	int num_parts;
	D3DXMATRIX local[16];
	D3DXMATRIX reflect[16], shadow[16];
	D3DMATERIAL9 mtrl[16];
	LPD3DXMESH mesh[16];

	void SetPosition(float X, float Y, float Z)
	{ x = X, y = Y, z = Z; }
}Object[16];

// Function Prototypes
//------------------------------------------------------------------------------------------------------------------------
int  GameMain(float);

int  GameInit();

void GameMurder();

void DrawObject(int index, float x, float y, float z, float yaw, float pitch, float roll);


//
void CreateRoom(float width, float height, float depth);

void DrawRoom();

void RenderMirror();
void RenderMirror2();
void RenderMirror3();

void RenderShadow();
//------------------------------------------------------------------------------------------------------------------------


// Globals
//-----------------------------------------------------------------------------------------------------------------------------

LPDIRECT3DDEVICE9 Device = 0;

LPDIRECT3DTEXTURE9 Tex[MAX_TEXTURES];

LPD3DXFONT Font = 0;

LPD3DXMESH Room, Teapot, Mirror, Box;
LPD3DXSPRITE Crosshair;

D3DLIGHT9 Point;

Input input;

HINSTANCE hinstance_app;
HWND hwnd_app;

char Buffer[80];

//custom materials for objects
const D3DMATERIAL9 FLOOR_MTRL = InitMtrl(MAGENTA, MAGENTA, RED, BLACK, 2.0f);
const D3DMATERIAL9 WALL_MTRL = InitMtrl(RED, YELLOW, PINK, BLACK, 2.0f);
const D3DMATERIAL9 BOOKSHELF_MTRL = InitMtrl(GREEN, YELLOW, YELLOW, BLACK, 2.0f);
const D3DMATERIAL9 CHAIR_MTRL = InitMtrl(BLUE, CYAN, CYAN, BLACK, 2.0f);
const D3DMATERIAL9 TVSTAND_MTRL = InitMtrl(BLUE, GREEN, GREEN, BLACK, 2.0f);

float teapotX = 0.0f, teapotY = 0.0f, teapotZ = 0.0f;
float teapotYaw = 0.0f, teapotPitch = 0.0f;

float mirror1_x = 0.0f, mirror1_y = 20.0f, mirror1_z = 39.0f, mirror1_xv = 16.0f;
//float mirror2_x = 0.0f, mirror2_y = 13.0f, mirror2_z = 39.0f;

float posX = 69.0f, posY = 22.0f, posZ = 0.0f;
float dirX = 0.0f, dirY = 0.0f, dirZ = 0.0f;
float theta = D3DX_PI * -0.5f, phi = 0.0f;
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
		posY = 22.0f;
		float velX = 0.0f, velZ = 0.0f;
		bool show_face = false;
		static float yaw = 0.0f, pitch = 0.0f;
		static float velocity = 0.0f;
// INPUT & LOGIC ///////////////////////////////////////////////////////////////////
	
		//read mouse input -----------------
		input.readmouse();
		theta += mouse_state.lX * .0025f;
		phi -= mouse_state.lY * .0025f;
		input.readKB();
		if (mouse_state.rgbButtons[0] )
		{
			if (kb_state[DIK_T])
			{
				if (Object[0].y >= 80.0f)
					Object[0].y = 80.0f;
				else
					Object[0].y += 2.0f * deltatime;
				PlaySound("notpanic.wav",hinstance_app,SND_FILENAME|SND_ASYNC|SND_NOSTOP);
				velocity = 2.0f;
			}
		}
		else
		{
			velocity += 0.25f;
			Object[0].y -= velocity * deltatime;
			if (Object[0].y <= 0.0f) Object[0].y = 0.0f;
		}
		if (mouse_state.rgbButtons[1] )
		{
			if (kb_state[DIK_T])
			{
				if (Object[0].y <= 0.0f)
					Object[0].y = 0.0f;
				else
					Object[0].y -= 2.0f * deltatime;
				PlaySound("expelled.wav",hinstance_app,SND_FILENAME|SND_ASYNC|SND_NOSTOP);
			}
		}
		if (mouse_state.rgbButtons[0] )
		{
			if (kb_state[DIK_R])
			{ 
				if (Object[3].y >= 20.0f)
					Object[3].y = 20.0f;
				else
					Object[3].y += 2.0f * deltatime;
					PlaySound("notpanic.wav",hinstance_app,SND_FILENAME|SND_ASYNC|SND_NOSTOP);
			}
		}
		if (mouse_state.rgbButtons[1] )
		{
			if (kb_state[DIK_R])
			{
				if (Object[3].y <= 12.0f)
					Object[3].y = 12.0f;
				else
					Object[3].y -= 2.0f * deltatime;
				PlaySound("expelled.wav",hinstance_app,SND_FILENAME|SND_ASYNC|SND_NOSTOP);
			}
		}
		if (mouse_state.rgbButtons[0] )
		{
			if (kb_state[DIK_Y])
			{
				if (Object[4].y >= 20.0f)
					Object[4].y = 20.0f;
				else
					Object[4].y += 2.0f * deltatime;
				PlaySound("notpanic.wav",hinstance_app,SND_FILENAME|SND_ASYNC|SND_NOSTOP);
			}
		}
		if (mouse_state.rgbButtons[1] )
		{
			if (kb_state[DIK_Y])
			{
				if (Object[4].y <= 12.0f)
					Object[4].y = 12.0f;
				else
					Object[4].y -= 2.0f * deltatime;
				PlaySound("expelled.wav",hinstance_app,SND_FILENAME|SND_ASYNC|SND_NOSTOP);
			}
		}
		//read keyboard input ---------------
		
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
		
		if (kb_state[DIK_C])
		{
			posY -= 11.0f;
		}
		if (kb_state[DIK_J])
		{
			teapotX -= 20.0f * deltatime;
		}
		if (kb_state[DIK_L])
		{
			teapotX += 20.0f * deltatime;
		}
		if (kb_state[DIK_I])
		{
			teapotZ += 20.0f * deltatime;
		}
		if (kb_state[DIK_K])
		{
			teapotZ -= 20.0f * deltatime;
		}
		if (kb_state[DIK_U])
		{
			teapotY += 20.0f * deltatime;
		}
		if (kb_state[DIK_O])
		{
			teapotY -= 20.0f * deltatime;
		}
		
// LOGIC ///////////////////////////////////////////////////////////////////
		
		posX += velX;
		posZ += velZ;
		
		if (theta > 6.28f)
			theta -= 6.28f;
		else if (theta < 0.0f)
			theta += 6.28f;

		if (phi > 1.57f)
			phi = 1.57f;
		else if (phi < -1.57f)
			phi = -1.57f;

		mirror1_x += mirror1_xv * deltatime;
		if (mirror1_x > 40.0f)
		{
			mirror1_xv = -16.0f;
			mirror1_x = 39.5f;
		}
		else if (mirror1_x < -40.0f)
		{
			mirror1_xv = 16.0f;
			mirror1_x = -39.5f;
		}
		
		// set view matrix
		//-------------------------------------------------------------
		if (!show_face)
		{
			dirX = (cosf(phi) * sinf(theta));
			dirY = (sinf(phi)              );
			dirZ = (cosf(phi) * cosf(theta));
		}

		D3DXVECTOR3 position(posX, posY, posZ);
		
		D3DXVECTOR3 target(posX + dirX, posY + dirY, posZ + dirZ);
		
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);
		
		
		//------------------------------------------------------------
	
	
// RENDER ///////////////////////////////////////////////////////////////////////////////////////////////////		

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0xff00001A, 1.0f, 0L);
		Device->BeginScene();
		
		//End Room --------------------------------------------

		D3DXMATRIX T, R;
	
		D3DXMatrixIdentity(&T);
		Device->SetTransform(D3DTS_WORLD, &T);
		Device->SetMaterial(&WHITE_MTRL);
		DrawRoom();
		D3DXMatrixTranslation(&T, mirror1_x, mirror1_y, 39.0f);
		Device->SetTransform(D3DTS_WORLD, &T);
		if (show_face)
			Device->SetTexture(0, Tex[4]);
		else Device->SetTexture(0, 0);
		Device->SetMaterial(&WHITE_MTRL);
		Mirror->DrawSubset(0);
		
		D3DXMatrixRotationY(&R, D3DX_PI);
		Device->SetTransform(D3DTS_WORLD, &(T * R));
		Mirror->DrawSubset(0);
	
		RenderShadow();

		RenderMirror();
		RenderMirror2();
		RenderMirror3();
		
		
		//DRAW OBJECTS
		
		DrawObject(0, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		DrawObject(1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		Object[2].SetPosition(2.0f*dirX+posX, 2.0f*dirY+posY, 2.0f*dirZ+posZ);
		DrawObject(2, 0.0f, 0.0f, 0.0f, theta, -phi, 0.0f);
		DrawObject(3, 0.0f, 0.0f, 0.0f, D3DX_PI * -0.5f, 0.0f, 0.0f);
		DrawObject(4, 0.0f, 0.0f, 0.0f, D3DX_PI * -0.25f, 0.0f, 0.0f);

		D3DXMATRIX inv;
		D3DXMatrixInverse(&inv, 0, &V);
		inv._41 = posX + dirX * 10.0f;
		inv._42 = posY + dirY * 10.0f;
		inv._43 = posZ + dirZ * 10.0f;

			// Multiply this by the billboard matrix
		

		// Now factor in the objects world position

		

		// Set the final matrix as the world matrix
		Device->SetTransform(D3DTS_WORLD, &inv );


		/*D3DXMatrixTranslation(&T, 0.0f, 10.0f, 0.0f);
		//D3DXMatrixRotationYawPitchRoll(&R, D3DX_PI 1.5f, 0.0f, 0.0f);
		//Device->SetTransform(D3DTS_WORLD, &(T));*/
		Device->SetTexture(0, Tex[4]);
		Box->DrawSubset(0);


		Device->EndScene();
		
		
		Device->Present(0, 0, 0, 0);
		
	}
	return(1);
}

void DrawObject(int index, float x, float y, float z, float yaw, float pitch, float roll)
{
	D3DXMATRIX T, R, S;
	D3DXMatrixTranslation(&T, Object[index].x, Object[index].y, Object[index].z);
	Object[index].pitch = pitch;
	Object[index].yaw = yaw;
	Object[index].roll = roll;
	D3DXMatrixRotationYawPitchRoll(&R, yaw, pitch, roll);
	
	for (int i = 0; i < Object[index].num_parts; i++)
	{
		Device->SetTexture(0,0);
		Device->SetMaterial(&Object[index].mtrl[i]);
		Device->SetTransform(D3DTS_WORLD, &(Object[index].local[i] * R * T));
		Object[index].mesh[i]->DrawSubset(0);
	}
}

void InitMagicWand(int index)
{
	Object_type* currObj = &Object[index];
	currObj->SetPosition(0.0f, 0.0f, 0.0f);
	currObj->num_parts = 1;

	D3DXMATRIX temp, temp2;
	D3DXCreateCylinder(Device, 0.10f, 0.10f, 3.0f, 8, 1, &currObj->mesh[0], 0);
	currObj->mtrl[0] = WHITE_MTRL;
	D3DXMatrixRotationYawPitchRoll(&temp, D3DX_PI * -0.15f,D3DX_PI * -0.15f,0.0f);
	D3DXMatrixTranslation(&temp2, 0.0f, 0.0f, 1.5f);
	currObj->local[0] = temp * temp2;
}

void InitTable(int index)
{
	Object_type* currObj = &Object[index];
	//currObj->tex_index = -1;
	currObj->x = 0.0f, currObj->y = 0.0f, currObj->z = 0.0f;
	currObj->num_parts = 5;

	D3DXMATRIX temp, temp2, temp3;
	//Table Top
	D3DXCreateCylinder(Device, 10.0f, 10.0f, 1.0f, 20, 4, &currObj->mesh[0], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp2, 0.0f, 10.0f, 0.0f);
	currObj->local[0] = temp * temp2;
	currObj->mtrl[0] = BLUE_MTRL;

	//Table Legs
	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &currObj->mesh[1], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp2, 5.0f, 5.0f, 5.0f);
	currObj->local[1] = temp * temp2;
	currObj->mtrl[1] = BLUE_MTRL;

	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &currObj->mesh[2], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp2, -5.0f, 5.0f, 5.0f);
	currObj->local[2] = temp * temp2;
	currObj->mtrl[2] = BLUE_MTRL;

	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &currObj->mesh[3], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp2, -5.0f, 5.0f, -5.0f);
	currObj->local[3] = temp * temp2;
	currObj->mtrl[3] = BLUE_MTRL;

	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &currObj->mesh[4], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp2, 5.0f, 5.0f, -5.0f);
	currObj->local[4] = temp * temp2;
	currObj->mtrl[4] = BLUE_MTRL;
}

void InitDoor(int index)
{
	Object_type* currObj = &Object[index];
	//currObj->tex_index = -1;
	currObj->x = 75.0f, currObj->y = 17.0f, currObj->z = 0.0f;
	currObj->num_parts = 3;

	D3DXMATRIX temp, temp2, temp3;

	D3DXMatrixRotationY(&temp2, D3DX_PI * -0.5f);
	D3DXCreateBox(Device, 18.0f, 34.0f, 0.5f, &currObj->mesh[0], 0);
	//D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	//D3DXMatrixTranslation(&temp2, 0.0f, 10.0f, 0.0f);
	currObj->local[0] = temp2;
	//D3DXMatrixIdentity(&currObj->local[0]);
	currObj->mtrl[0] = GREEN_MTRL;

	//Table Legs
	D3DXCreateCylinder(Device, 0.35f, 0.35f, 1.3f, 8, 1, &currObj->mesh[1], 0);
	//D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp, 6.0f, -2.0f, 0.5f);
	currObj->local[1] = temp * temp2;
	currObj->mtrl[1] = BLUE_MTRL;

	D3DXCreateSphere(Device, 0.75f, 20, 20, &currObj->mesh[2], 0);
	//D3DXMatrixRotationX(&temp, D3DX_PI * .5f);
	D3DXMatrixTranslation(&temp, 6.0f, -2.0f, 1.5f);
	currObj->local[2] = temp * temp2;
	currObj->mtrl[2] = BLUE_MTRL;

}

void InitTVStand(int index)
{
	Object_type* currObj = &Object[index];
	D3DXMATRIX temp, temp2;
	currObj->SetPosition(-69.0f,11.0f,0.0f);
	//TV & STAND
	//tv box
	currObj->num_parts = 11;
	D3DXCreateBox(Device, 16.0f, 9.0f, 4.5f, &currObj->mesh[0], 0);
	D3DXMatrixTranslation(&currObj->local[0], 0.0f, 2.25f, 0.0f);
	currObj->mtrl[0] = SILVER_MTRL;

	//tv screen
	D3DXCreateBox(Device, 12.8f, 7.2f, 0.1f, &currObj->mesh[1], 0);
	D3DXMatrixTranslation(&currObj->local[1], 0.0f, 2.25f, -2.25f);
	currObj->mtrl[1] = BLACK_MTRL;

	//tv stand
	D3DXCreateBox(Device, 18.0f, 0.5f, 5.0f, &currObj->mesh[2], 0);
	D3DXMatrixTranslation(&currObj->local[2], 0.0f, -2.5f, 0.0f);
	currObj->mtrl[2] = TVSTAND_MTRL;

	D3DXCreateBox(Device, 18.0f, 0.5f, 5.0f, &currObj->mesh[3], 0);
	D3DXMatrixTranslation(&currObj->local[3], 0.0f, -6.5f, 0.0f);
	currObj->mtrl[3] = TVSTAND_MTRL;

	D3DXCreateBox(Device, 18.0f, 0.5f, 5.0f, &currObj->mesh[4], 0);
	D3DXMatrixTranslation(&currObj->local[4], 0.0f, -10.5f, 0.0f);
	currObj->mtrl[4] = TVSTAND_MTRL;
	

	D3DXCreateBox(Device, 0.5f, 9.0f, 5.5f, &currObj->mesh[5], 0);
	D3DXMatrixTranslation(&currObj->local[5], 9.25f, -6.5f, 0.0f);
	currObj->mtrl[5] = TVSTAND_MTRL;

	D3DXCreateBox(Device, 0.5f, 9.0f, 5.5f, &currObj->mesh[6], 0);
	D3DXMatrixTranslation(&currObj->local[6], -9.25f, -6.5f, 0.0f);
	currObj->mtrl[6] = TVSTAND_MTRL;

	//define xbox 360 video game box
	D3DXCreateBox(Device, 6.0f, 1.5f, 4.0f, &currObj->mesh[7], 0);
	D3DXMatrixTranslation(&currObj->local[7], 0.0f, -5.5f, 0.0f);
	currObj->mtrl[7] = WHITE_MTRL;

	//define power brick
	D3DXCreateBox(Device, 2.0f, 1.25f, 4.0f, &currObj->mesh[8], 0);
	D3DXMatrixTranslation(&currObj->local[8], 0.0f, -9.5f, 0.0f);
	currObj->mtrl[8] = BLACK_MTRL;

	//ring of light
	D3DXCreateTorus(Device, 0.03f, 0.4f, 3, 10, &currObj->mesh[9], 0);
	D3DXMatrixScaling(&temp, 1.0f, 1.0f, 0.1f);
	D3DXMatrixTranslation(&currObj->local[9], 1.5f, -5.5, -2.0f);
	currObj->local[9] = temp * currObj->local[9];
	currObj->mtrl[9] = XBOX_MTRL;

	D3DXCreateSphere(Device, 0.1f, 15, 15, &currObj->mesh[10], 0);
	D3DXMatrixTranslation(&currObj->local[10], -0.4f, -9.5f, -2.0f);
	currObj->mtrl[10] = XBOX_MTRL;
}
void InitBookshelf(int index)
{
	Object_type* currObj = &Object[index];
	D3DXMATRIX temp, temp2;
	currObj->SetPosition(-65.0f,12.0f,30.0f);

	currObj->num_parts = 8;
	D3DXCreateBox(Device, 15.0f, 22.0f, 0.5f, &currObj->mesh[0], 0);
	D3DXMatrixIdentity(&currObj->local[0]);
	currObj->mtrl[0] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 15.0f, 0.5f, 5.0f, &currObj->mesh[1], 0);
	D3DXMatrixTranslation(&currObj->local[1], 0.0f, 10.75f, -2.75f);
	currObj->mtrl[1] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 15.0f, 0.5f, 5.0f, &currObj->mesh[2], 0);
	D3DXMatrixTranslation(&currObj->local[2], 0.0f, -10.75f, -2.75f);
	currObj->mtrl[2] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 0.5f, 22.0f, 5.5f, &currObj->mesh[3], 0);
	D3DXMatrixTranslation(&currObj->local[3], 7.5f, 0.0f, -2.5f);
	currObj->mtrl[3] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 0.5f, 22.0f, 5.5f, &currObj->mesh[4], 0);
	D3DXMatrixTranslation(&currObj->local[4], -7.5f, 0.0f, -2.5f);
	currObj->mtrl[4] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 14.5f, 0.5f, 5.0f, &currObj->mesh[5], 0);
	D3DXMatrixTranslation(&currObj->local[5], 0.0f, 3.58f, -2.75f);
	currObj->mtrl[5] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 14.5f, 0.5f, 5.0f, &currObj->mesh[6], 0);
	D3DXMatrixTranslation(&currObj->local[6], 0.0f, -3.58f, -2.75f);
	currObj->mtrl[6] = BOOKSHELF_MTRL;

	D3DXCreateBox(Device, 1.0f, 5.0f, 4.0f, &currObj->mesh[7], 0);
	D3DXMatrixRotationZ(&temp, D3DX_PI * 0.15f);
	D3DXMatrixTranslation(&currObj->local[7], -5.6f, -1.0f, -2.75f);
	currObj->local[7] = temp * currObj->local[7];
	currObj->mtrl[7] = RED_MTRL;
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
	
	CreateRoom(150.0f, 40.0f, 80.0f);
	D3DXCreateTeapot(Device, &Teapot, 0);
	CreateTexPlane(Device, 5.0f, 9.0f, &Box);
	//D3DXCreateBox(Device,10.0f, 10.0f, 1.0f, &Box, 0);
	CreateTexPlane(Device, 32.0f, 15.0f, &Mirror);

	InitTable(0);
	InitDoor(1);
	InitMagicWand(2);
	InitTVStand(3);
	InitBookshelf(4);
	D3DXCreateSprite(Device, &Crosshair);

	D3DXCreateTextureFromFile(Device, "Textures\\wall.bmp", &Tex[0]);
	D3DXCreateTextureFromFile(Device, "Textures\\ceiling.bmp", &Tex[1]);
	D3DXCreateTextureFromFile(Device, "Textures\\floor.bmp", &Tex[2]);
	D3DXCreateTextureFromFile(Device, "Textures\\crosshair.dds", &Tex[3]);
	D3DXCreateTextureFromFile(Device, "Textures\\Mirror_face1.bmp", &Tex[4]);

	Device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
	Device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);

	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);
	D3DXCOLOR c(WHITE);
	
	D3DXVECTOR3 pos2(0.0f, -1.0f, 0.0f);
	Point = InitDirectionalLight(&pos2, &c);

	Device->SetLight(0, &Point);
	Device->LightEnable(0, true);
	
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);


	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.35f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	
	Device->SetRenderState(D3DRS_LIGHTING, true);
	return(1);
}

//destroy on site and kill everything, make sure there are no survivors
void GameMurder()
{
	//font murder
	if (Font)
	{
		Font->MURDER();
		Font = 0;
	}

	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		if (Tex[i])
		{
			Tex[i]->MURDER();
			Tex[i] = 0;
		}
	}
	
	if (Teapot)
	{
		Teapot->DESTROY();
		Teapot = 0;
	}
	//destroy room
	if (Room)
	{
		Room->DESTROY();
		Room = 0;
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

//creates room mesh with custom dimensions
//width and depth is centered around the origin
//height is built from the origin up
void CreateRoom(float width, float height, float depth)
{
	D3DXCreateMeshFVF(12, 24, D3DXMESH_MANAGED, D3DFVF_CUSTOM, Device, &Room);
	Vertex* v;
	Room->LockVertexBuffer(0, (void**)&v);
	//front wall
	v[0] = Vertex(-width/2,     0.0f, depth/2, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-width/2,   height, depth/2, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex( width/2,   height, depth/2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = Vertex( width/2,     0.0f, depth/2, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	//left wall
	v[4] = Vertex(-width/2,     0.0f, -depth/2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[5] = Vertex(-width/2,   height, -depth/2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[6] = Vertex(-width/2,   height,  depth/2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[7] = Vertex(-width/2,     0.0f,  depth/2, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//back wall
	v[8]  = Vertex(width/2,     0.0f, -depth/2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[9]  = Vertex(width/2,   height, -depth/2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[10] = Vertex(-width/2,  height, -depth/2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-width/2,    0.0f, -depth/2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	//right wall
	v[12] = Vertex(width/2,     0.0f,  depth/2, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[13] = Vertex(width/2,   height,  depth/2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[14] = Vertex(width/2,   height, -depth/2, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[15] = Vertex(width/2,     0.0f, -depth/2, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//ceiling
	v[16] = Vertex(-width/2,   height,  depth/2, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-width/2,   height, -depth/2, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex( width/2,   height, -depth/2, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex( width/2,   height,  depth/2, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);

	//floor
	v[20] = Vertex(-width/2,   0.0f, -depth/2, 0.0f, 1.0f, 0.0f, 0.0f, 8.0f);
	v[21] = Vertex(-width/2,   0.0f,  depth/2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex( width/2,   0.0f,  depth/2, 0.0f, 1.0f, 0.0f, 8.0f, 0.0f);
	v[23] = Vertex( width/2,   0.0f, -depth/2, 0.0f, 1.0f, 0.0f, 8.0f, 8.0f);
	Room->UnlockVertexBuffer();

	WORD* i;
	Room->LockIndexBuffer(0, (void**)&i);
	for (int j = 0, k = 0; j < 24; j += 4, k += 6)
	{
		i[k]   = j; i[k+1] = j+1; i[k+2] = j+2;
		i[k+3] = j; i[k+4] = j+2; i[k+5] = j+3;
	}
	Room->UnlockIndexBuffer();

	DWORD* a;
	Room->LockAttributeBuffer(0, &a);
	//left wall;
	for (int i = 0; i < 8; i++)
		a[i] = 0;

	for (int i = 8; i < 10; i++)
		a[i] = 1;

	for (int i = 10; i < 12; i++)
		a[i] = 2;

	Room->UnlockAttributeBuffer();
	
}

void DrawRoom()
{
	for (int i = 0; i < 3; i++)
	{
		if (Room)
		{
			//if (i == 2) Device->SetMaterial(&CHAIR_MTRL);
			Device->SetTexture(0, Tex[i]);
			Room->DrawSubset(i);
		}
	}
}

void RenderMirror()
{
	//
	// Draw Mirror quad to stencil buffer ONLY.  In this way
	// only the stencil bits that correspond to the mirror will
	// be on.  Therefore, the reflected teapot can only be rendered
	// where the stencil bits are turned on, and thus on the mirror 
	// only.
	//

    Device->SetRenderState(D3DRS_STENCILENABLE,    true);
    Device->SetRenderState(D3DRS_STENCILFUNC,      D3DCMP_ALWAYS);
    Device->SetRenderState(D3DRS_STENCILREF,       0x1);
    Device->SetRenderState(D3DRS_STENCILMASK,      0xffffffff);
    Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
    Device->SetRenderState(D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP);
    Device->SetRenderState(D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP);
    Device->SetRenderState(D3DRS_STENCILPASS,      D3DSTENCILOP_REPLACE);

	// disable writes to the depth and back buffers
    Device->SetRenderState(D3DRS_ZWRITEENABLE, false);
    Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
    Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    // draw the mirror to the stencil buffer
	
	
	Device->SetTexture(0, Tex[2]);
	D3DXMATRIX I;
	D3DXMATRIX W, T, R, R2, R3;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);
	//floor
	Room->DrawSubset(2);

	/*D3DXMatrixTranslation(&I, mirror1_x, mirror1_y, 39.0f);
	Device->SetTexture(0, 0);
	Device->SetTransform(D3DTS_WORLD, &I);
	Mirror->DrawSubset(0);

	D3DXMatrixRotationY(&R, D3DX_PI);
	Device->SetTexture(0, 0);
	Device->SetTransform(D3DTS_WORLD, &(I * R));
	Mirror->DrawSubset(0);*/


	// re-enable depth writes
	Device->SetRenderState( D3DRS_ZWRITEENABLE, true );

	// only draw reflected teapot to the pixels where the mirror
	// was drawn to.
	Device->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_EQUAL);
    Device->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP);

	// position reflection
	
	D3DXPLANE plane(0.0f, 1.0f, 0.0f, 0.0f); // xy plane
	D3DXMatrixReflect(&R, &plane);

	D3DXMatrixRotationYawPitchRoll(&R2, teapotYaw, teapotPitch, 0.0f);
	for (int i = 0; i < 16; i++)
	{
		D3DXMatrixTranslation(&T, Object[i].x,  Object[i].y, Object[i].z); 
		for (int j = 0; j < Object[i].num_parts; j++)
		{
			D3DXMatrixRotationYawPitchRoll(&R2, Object[i].yaw, Object[i].pitch, Object[i].roll);
			D3DXMatrixTranslation(&T, Object[i].x,  Object[i].y, Object[i].z); 
			Object[i].reflect[j] = Object[i].local[j] * R2 * T * R;
		}
	}
	
	// clear depth buffer and blend the reflected teapot with the mirror
	Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_DESTCOLOR);
    Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	// Finally, draw the reflected teapot
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&RED_MTRL);
	Device->SetTexture(0, 0);

	// reverse cull mode
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < Object[i].num_parts; j++)
		{
			Device->SetTransform(D3DTS_WORLD, &Object[i].reflect[j]);
			Device->SetMaterial(&Object[i].mtrl[j]);
			Object[i].mesh[j]->DrawSubset(0);
		}
	}
	/*Device->SetTransform(D3DTS_WORLD, &W);
	Teapot->DrawSubset(0);*/
	D3DXMatrixRotationY(&R2, D3DX_PI);
	Device->SetMaterial(&WHITE_MTRL);
	Device->SetTransform(D3DTS_WORLD, &(R));
	DrawRoom();

	D3DXMatrixTranslation(&T, posX, posY - 5.0f, posZ);
	D3DXMatrixRotationYawPitchRoll(&R2, theta, -phi, 0.0f);
	Device->SetTransform(D3DTS_WORLD, &(R2 * T * R));
	Box->DrawSubset(0);
	
	// Restore render states.
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState( D3DRS_STENCILENABLE, false);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void RenderMirror2()
{
	//
	// Draw Mirror quad to stencil buffer ONLY.  In this way
	// only the stencil bits that correspond to the mirror will
	// be on.  Therefore, the reflected teapot can only be rendered
	// where the stencil bits are turned on, and thus on the mirror 
	// only.
	//

    Device->SetRenderState(D3DRS_STENCILENABLE,    true);
    Device->SetRenderState(D3DRS_STENCILFUNC,      D3DCMP_ALWAYS);
    Device->SetRenderState(D3DRS_STENCILREF,       0x1);
    Device->SetRenderState(D3DRS_STENCILMASK,      0xffffffff);
    Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
    Device->SetRenderState(D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP);
    Device->SetRenderState(D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP);
    Device->SetRenderState(D3DRS_STENCILPASS,      D3DSTENCILOP_REPLACE);

	// disable writes to the depth and back buffers
    Device->SetRenderState(D3DRS_ZWRITEENABLE, false);
//    /Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
    Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

    // draw the mirror to the stencil buffer
	
	
	//Device->SetTexture(0, Tex[2]);
	D3DXMATRIX I;
	D3DXMATRIX W, T, R, R2, R3;
	//D3DXMatrixIdentity(&I);
	//Device->SetTransform(D3DTS_WORLD, &I);
	////floor
	//Room->DrawSubset(2);

	D3DXMatrixTranslation(&I, mirror1_x, mirror1_y, 39.0f);
	Device->SetTexture(0, Tex[4]);
	Device->SetTransform(D3DTS_WORLD, &I);
	
	Mirror->DrawSubset(0);
/*
	D3DXMatrixRotationY(&R, D3DX_PI);
	Device->SetTexture(0, 0);
	Device->SetTransform(D3DTS_WORLD, &(I * R));
	Mirror->DrawSubset(0);*/


	// re-enable depth writes
	Device->SetRenderState( D3DRS_ZWRITEENABLE, true );

	// only draw reflected teapot to the pixels where the mirror
	// was drawn to.
	Device->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_EQUAL);
    Device->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP);

	// position reflection
	
	D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	D3DXMatrixReflect(&R, &plane);
	D3DXMatrixTranslation(&I, 0.0f, 0.0f, -80.0f);
	
	for (int i = 0; i < 16; i++)
	{
		D3DXMatrixRotationYawPitchRoll(&R2, Object[i].yaw, Object[i].pitch, Object[i].roll);
		D3DXMatrixTranslation(&T, Object[i].x,  Object[i].y, Object[i].z); 
		for (int j = 0; j < Object[i].num_parts; j++)
		{
			Object[i].reflect[j] = Object[i].local[j] * R2 * I * T * R;
		}
	}
	
	// clear depth buffer and blend the reflected teapot with the mirror
	Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
    Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR);

	// Finally, draw the reflected teapot
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&RED_MTRL);
	Device->SetTexture(0, 0);

	// reverse cull mode
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < Object[i].num_parts; j++)
		{
			Device->SetTransform(D3DTS_WORLD, &Object[i].reflect[j]);
			Device->SetMaterial(&Object[i].mtrl[j]);
			Object[i].mesh[j]->DrawSubset(0);
		}
	}
	Device->SetMaterial(&WHITE_MTRL);
	//D3DXMatrixIdentity(&I);

	D3DXMatrixRotationY(&R2, D3DX_PI);
	Device->SetTransform(D3DTS_WORLD, &(I * R));
	DrawRoom();

	D3DXMatrixTranslation(&T, posX, posY - 5.0f, posZ);
	D3DXMatrixRotationYawPitchRoll(&R2, theta + D3DX_PI, 0.0f, 0.0f);
	Device->SetTransform(D3DTS_WORLD, &(R2 * T * I * R));
	Device->SetTexture(0, Tex[4]);
	Box->DrawSubset(0);
	/*Device->SetTransform(D3DTS_WORLD, &W);
	Teapot->DrawSubset(0);*/
	
	
	
	// Restore render states.
	//Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState( D3DRS_STENCILENABLE, false);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void RenderMirror3()
{
	//
	// Draw Mirror quad to stencil buffer ONLY.  In this way
	// only the stencil bits that correspond to the mirror will
	// be on.  Therefore, the reflected teapot can only be rendered
	// where the stencil bits are turned on, and thus on the mirror 
	// only.
	//

    Device->SetRenderState(D3DRS_STENCILENABLE,    true);
    Device->SetRenderState(D3DRS_STENCILFUNC,      D3DCMP_ALWAYS);
    Device->SetRenderState(D3DRS_STENCILREF,       0x1);
    Device->SetRenderState(D3DRS_STENCILMASK,      0xffffffff);
    Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
    Device->SetRenderState(D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP);
    Device->SetRenderState(D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP);
    Device->SetRenderState(D3DRS_STENCILPASS,      D3DSTENCILOP_REPLACE);

	// disable writes to the depth and back buffers
    Device->SetRenderState(D3DRS_ZWRITEENABLE, false);
   // Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
   /* Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
    Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);*/

    // draw the mirror to the stencil buffer
	
	
	//Device->SetTexture(0, Tex[2]);
	D3DXMATRIX I;
	D3DXMATRIX W, T, R, R2, R3;
	//D3DXMatrixIdentity(&I);
	//Device->SetTransform(D3DTS_WORLD, &I);
	////floor
	//Room->DrawSubset(2);

	D3DXMatrixTranslation(&I, mirror1_x, mirror1_y, 39.0f);
	/*Device->SetTexture(0, Tex[4]);

	Device->SetTransform(D3DTS_WORLD, &I);
	Mirror->DrawSubset(0);*/

	D3DXMatrixRotationY(&R, D3DX_PI);
	Device->SetTexture(0, 0);
	Device->SetTransform(D3DTS_WORLD, &(I * R));
	Mirror->DrawSubset(0);


	// re-enable depth writes
	Device->SetRenderState( D3DRS_ZWRITEENABLE, true );

	// only draw reflected teapot to the pixels where the mirror
	// was drawn to.
	Device->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_EQUAL);
    Device->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP);

	// position reflection
	
	D3DXPLANE plane(0.0f, 0.0f, 1.0f, 0.0f); // xy plane
	D3DXMatrixReflect(&R, &plane);
	D3DXMatrixTranslation(&I, 0.0f, 0.0f, 80.0f);
	
	for (int i = 0; i < 16; i++)
	{
		D3DXMatrixTranslation(&T, Object[i].x,  Object[i].y, Object[i].z); 
		for (int j = 0; j < Object[i].num_parts; j++)
		{
			D3DXMatrixRotationYawPitchRoll(&R2, Object[i].yaw, Object[i].pitch, Object[i].roll);
			D3DXMatrixTranslation(&T, Object[i].x,  Object[i].y, Object[i].z); 
			Object[i].reflect[j] = Object[i].local[j] * R2 * I * T * R;
		}
	}
	
	// clear depth buffer and blend the reflected teapot with the mirror
	Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	//Device->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ONE);
    //Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVDESTCOLOR);
	

	// Finally, draw the reflected teapot
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&RED_MTRL);
	Device->SetTexture(0, 0);

	// reverse cull mode
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < Object[i].num_parts; j++)
		{
			Device->SetTransform(D3DTS_WORLD, &Object[i].reflect[j]);
			Device->SetMaterial(&Object[i].mtrl[j]);
			Object[i].mesh[j]->DrawSubset(0);
		}
	}
	Device->SetMaterial(&WHITE_MTRL);
	//D3DXMatrixIdentity(&I);

	D3DXMatrixRotationY(&R2, D3DX_PI);
	Device->SetTransform(D3DTS_WORLD, &(I * R));
	DrawRoom();
	
	/*Device->SetTransform(D3DTS_WORLD, &W);
	Teapot->DrawSubset(0);*/
	D3DXMatrixTranslation(&T, posX, posY - 5.0f, posZ);
	D3DXMatrixRotationYawPitchRoll(&R2, theta + D3DX_PI, 0.0f, 0.0f);
	Device->SetTransform(D3DTS_WORLD, &(R2 * T * I * R));
	Device->SetTexture(0, Tex[4]);
	Box->DrawSubset(0);
	
	
	// Restore render states.
	//Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState( D3DRS_STENCILENABLE, false);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void RenderShadow()
{
	Device->SetRenderState(D3DRS_STENCILENABLE,    true);
    Device->SetRenderState(D3DRS_STENCILFUNC,      D3DCMP_EQUAL);
    Device->SetRenderState(D3DRS_STENCILREF,       0x0);
    Device->SetRenderState(D3DRS_STENCILMASK,      0xffffffff);
    Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
    Device->SetRenderState(D3DRS_STENCILZFAIL,     D3DSTENCILOP_KEEP);
    Device->SetRenderState(D3DRS_STENCILFAIL,      D3DSTENCILOP_KEEP);
    Device->SetRenderState(D3DRS_STENCILPASS,      D3DSTENCILOP_INCR); // increment to 1

	// position shadow
	D3DXVECTOR4 lightDirection(0.0f, -1.0f, 0.0f, 0.0f);
	D3DXPLANE groundPlane(0.0f, -1.0f, 0.0f, 0.0f);

	D3DXMATRIX S, R;
	D3DXMatrixShadow(
		&S,
		&lightDirection,
		&groundPlane);

	D3DXMATRIX T;

	for (int i = 0; i < 16; i++)
	{
		D3DXMatrixTranslation(&T, Object[i].x,  Object[i].y, Object[i].z); 
		for (int j = 0; j < Object[i].num_parts; j++)
		{
			D3DXMatrixRotationYawPitchRoll(&R, Object[i].yaw, Object[i].pitch, Object[i].roll);
			D3DXMatrixTranslation(&T, Object[i].x,  Object[i].y, Object[i].z); 
			Object[i].shadow[j] = Object[i].local[j] * R * T * S;
		}
	}
	/*D3DXMatrixTranslation(
		&T,
		TeapotPosition.x,
		TeapotPosition.y,
		TeapotPosition.z);*/

	//D3DXMATRIX W = T * S;
	//Device->SetTransform(D3DTS_WORLD, &W);

	// alpha blend the shadow
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	D3DMATERIAL9 mtrl = InitMtrl(BLACK, BLACK, BLACK, BLACK, 0.0f);
	mtrl.Diffuse.a = 0.5f; // 50% transparency.

	// Disable depth buffer so that z-fighting doesn't occur when we
	// render the shadow on top of the floor.
	Device->SetRenderState(D3DRS_ZENABLE, false);
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < Object[i].num_parts; j++)
		{
			Device->SetTransform(D3DTS_WORLD, &Object[i].shadow[j]);
			Device->SetTexture(0, 0);
			Device->SetMaterial(&mtrl);
			Object[i].mesh[j]->DrawSubset(0);
		}
	}
	
	
	

	Device->SetRenderState(D3DRS_ZENABLE, true);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_STENCILENABLE,    false);
}