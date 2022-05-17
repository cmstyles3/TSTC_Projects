// Collin Munson

#include <stdio.h>
#include <stdlib.h>
#include "d3dbase.h"
#include "input.h"

// Defines
//---------------------------------------------------------------
#define SCREEN_WIDTH 1680
#define SCREEN_HEIGHT 1050

#define MURDER Release

#define MAX_TEXTURES 32

#define SPEED 250.0f

enum{TABLE, CHAIR, TEAPOT, BALL, BOOKSHELF, TVSTAND, CARDS, GAMECASE };

struct
{
	int num_parts;
	int tex[16];
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

void DrawHallwaySeg(float z);

void CreateTexBox(LPDIRECT3DDEVICE9 device, float width, float height, float depth, LPD3DXMESH *mesh);
void CreateTexPlane(LPDIRECT3DDEVICE9 device, float width, float height, LPD3DXMESH *mesh);
//------------------------------------------------------------------------------------------------------------------------


// Globals
//-----------------------------------------------------------------------------------------------------------------------------

LPDIRECT3DDEVICE9 Device = 0;

LPD3DXFONT Font = 0;

LPDIRECT3DTEXTURE9 Tex[MAX_TEXTURES] = {0};

LPD3DXMESH teapot, Spiderweb, Picture, Wall, GhostHead, GhostBody;
D3DLIGHT9 Spot, Point;
D3DMATERIAL9 Mtrl;

Input input;


D3DXMATRIX ObjMatrices[20];

HINSTANCE hinstance_app;
HWND hwnd_app;

float ghost1Z = 0.0f, ghost2Z = 0.0f, ghost3Z = 0.0f;

char Buffer[80];

//custom materials for objects
const D3DMATERIAL9 FLOOR_MTRL = InitMtrl(MAGENTA, MAGENTA, RED, BLACK, 2.0f);
const D3DMATERIAL9 WALL_MTRL = InitMtrl(RED, YELLOW, PINK, BLACK, 2.0f);
const D3DMATERIAL9 BOOKSHELF_MTRL = InitMtrl(RED, YELLOW, YELLOW, BLACK, 2.0f);
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
	PlaySound("soundtrack.wav", 0, SND_ASYNC | SND_LOOP);
	EnterMsgLoop(GameMain);

	GameMurder();

	return(0);
}

int GameMain(float deltatime)
{
	if (Device)
	{

		static float theta = 0.0f, phi = 0.0f;
		static float posX = 0.0f, posZ = 0.0f;
		static float dirX = 0.0f, dirY = 0.0f, dirZ = 0.0f;
		float posY = 4.0f;
		float velX = 0.0f, velZ = 0.0f;
		static bool lightOn = true;
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
			velX += (sinf(theta) * SPEED) * deltatime;
			velZ += (cosf(theta) * SPEED) * deltatime;
		}
		if (kb_state[DIK_S])
		{
			velX += -(sinf(theta) * SPEED) * deltatime;
			velZ += -(cosf(theta) * SPEED) * deltatime;
		}
		if (kb_state[DIK_A])
		{
			velX += -(cosf(theta) * SPEED) * deltatime;
			velZ +=  (sinf(theta) * SPEED) * deltatime;
		}
		if (kb_state[DIK_D])
		{
			velX +=  (cosf(theta) * SPEED) * deltatime;
			velZ += -(sinf(theta) * SPEED) * deltatime;
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
		
		//advance the movement of camera position by velocities obtained from user input
		posX += velX;
		posZ += velZ;

		ghost1Z += 420.0f * deltatime;
		if (ghost1Z > 1600.0f)
			ghost1Z -= 3200.0f;
		ghost2Z -= 480.0f * deltatime;
		if (ghost2Z < -1600.0f)
			ghost2Z += 3200.0f;
		ghost3Z += 300.0f * deltatime;
		if (ghost3Z > 1600.0f)
			ghost3Z -= 3200.0f;

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
	
		//test to see if z should be teleported, 
		//important part of the infinite hall trick
		if (posZ > 1000.0f)
			posZ -= 2000.0f;
		else if (posZ < -1000.0f) 
			posZ += 2000.0f;

		//restrict boundaries for x position of camera
		if (posX > 34.0f) posX = 34.0f;
		else if (posX < -34.0f) posX = -34.0f;



		// set view matrix
		//-------------------------------------------------------------
	
		dirX = (cosf(phi) * sinf(theta));
		dirY = (sinf(phi)              );
		dirZ = (cosf(phi) * cosf(theta));

		Spot.Direction.x = dirX;
		Spot.Direction.y = dirY;
		Spot.Direction.z = dirZ;
		
		Spot.Position.x = posX;
		Spot.Position.y = posY;
		Spot.Position.z = posZ;

		Point.Position.x = posX;
		Point.Position.y = posY;
		Point.Position.z = posZ + dirZ * 40.0f;

		D3DXVECTOR3 position(posX, posY, posZ);
		
		D3DXVECTOR3 target(posX + dirX, posY + dirY, posZ + dirZ);
		
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

		
		
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);
		
		Device->SetLight(0, &Spot);
		Device->SetLight(1, &Point);
		
		//------------------------------------------------------------
	
	
// RENDER ///////////////////////////////////////////////////////////////////////////////////////////////////		

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
		Device->BeginScene();
		
		//Draw 40 segments of Hallway down the z-axis and centered on the z-axis
		for (int seg = 0; seg <= 40; seg++)
			DrawHallwaySeg(float((seg * 80) - (1600)));

		D3DXMATRIX S,S2;
		D3DXMATRIX R, T,T2;
		D3DXMatrixIdentity(&T);
		
		
		DrawObject(CHAIR, 0.0f, -18.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		DrawObject(TVSTAND, -0.0f, -9.0f, 1050.0f, 0.0f, 0.0f, 0.0f);
		DrawObject(TVSTAND, -0.0f, -9.0f, -1050.0f, D3DX_PI, 0.0f, 0.0f);
		DrawObject(BOOKSHELF, 38.0f, -9.0f, 10.0f, D3DX_PI * 0.5f, 0.0f, 0.0f);
		
		//Device->SetMaterial(&SILVER_MTRL);
		
		D3DXMatrixRotationYawPitchRoll(&R, 0.5f * D3DX_PI, 0.0f, 0.05f * D3DX_PI);
		D3DXMatrixTranslation(&T, 39.9f, 0.0f, 100.0f);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Device->SetTexture(0, Tex[6]);
		Picture->DrawSubset(0);	
		
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

		
		D3DXMatrixTranslation(&T, 39.8f, 0.0f, 100.0f);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Device->SetTexture(0, Tex[7]);
		Picture->DrawSubset(0);

		D3DXMatrixRotationYawPitchRoll(&R, -0.5f * D3DX_PI, 0.0f, 0.0f);
		D3DXMatrixScaling(&S, 3.0f, 3.0f, 1.0f);
		D3DXMatrixTranslation(&T, -39.9f, 0.0f, 107.0f);
		Device->SetTransform(D3DTS_WORLD, &(S * R * T));
		Device->SetTexture(0, Tex[8]);
		Picture->DrawSubset(0);

		D3DXMatrixRotationYawPitchRoll(&R, -0.5f * D3DX_PI, 0.0f, 0.0f);
		D3DXMatrixScaling(&S, 1.0f, 4.0f, 1.0f);
		D3DXMatrixTranslation(&T, -39.7f, 0.0f, 80.0f);
		Device->SetTransform(D3DTS_WORLD, &(S * R * T));
		Device->SetTexture(0, Tex[7]);
		Picture->DrawSubset(0);

		D3DXMatrixRotationYawPitchRoll(&R, -0.5f * D3DX_PI, 0.0f, 0.5f * D3DX_PI);
		D3DXMatrixScaling(&S, 3.0f, 6.0f, 1.0f);
		D3DXMatrixTranslation(&T, -39.7f, 0.0f, 30.0f);
		Device->SetTransform(D3DTS_WORLD, &(S * R * T));
		Device->SetTexture(0, Tex[8]);
		Picture->DrawSubset(0);

		D3DXMatrixIdentity(&T);
		D3DXMatrixRotationYawPitchRoll(&R, -0.5f * D3DX_PI, -0.25f * D3DX_PI, 0.0f);
		D3DXMatrixTranslation(&T, -33.0f, 13.0f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Device->SetTexture(0, Tex[5]);
		Spiderweb->DrawSubset(0);
		
		
		D3DXMatrixTranslation(&T, 0.0f, 0.0f, ghost1Z);
		Device->SetTransform(D3DTS_WORLD, &T);
		Device->SetTexture(0, Tex[3]);
		GhostHead->DrawSubset(0);
		D3DXMatrixRotationX(&R, D3DX_PI * 0.5f);
		D3DXMatrixTranslation(&T, 0.0f, -6.5f, ghost1Z);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Device->SetTexture(0, Tex[4]);
		GhostBody->DrawSubset(0);

		D3DXMatrixTranslation(&T, 15.0f, 0.0f, ghost2Z);
		Device->SetTransform(D3DTS_WORLD, &T);
		Device->SetTexture(0, Tex[3]);
		GhostHead->DrawSubset(0);
		D3DXMatrixRotationX(&R, D3DX_PI * 0.5f);
		D3DXMatrixTranslation(&T, 15.0f, -6.5f, ghost2Z);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Device->SetTexture(0, Tex[4]);
		GhostBody->DrawSubset(0);

		D3DXMatrixTranslation(&T, -15.0f, 0.0f, ghost3Z);
		Device->SetTransform(D3DTS_WORLD, &T);
		Device->SetTexture(0, Tex[3]);
		GhostHead->DrawSubset(0);
		D3DXMatrixRotationX(&R, D3DX_PI * 0.5f);
		D3DXMatrixTranslation(&T, -15.0f, -6.5f, ghost3Z);
		Device->SetTransform(D3DTS_WORLD, &(R * T));
		Device->SetTexture(0, Tex[4]);
		GhostBody->DrawSubset(0);

		
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		
		
		//teapot->DrawSubset(0);
		
		//Draw Table
		

		////Position and Draw chairs
		////4 around table
		
		//DrawObject(CHAIR, 10.0f, -18.0f, -20.0f, D3DX_PI * -0.5f, 0.0f, 0.0f);
		//DrawObject(CHAIR, 30.0f, -18.0f, -20.0f, D3DX_PI * 0.5f, 0.0f, 0.0f);
		//DrawObject(CHAIR, 20.0f, -18.0f, -30.0f, D3DX_PI, 0.0f, 0.0f);
		//
		////1 in front of TV
		//DrawObject(CHAIR, -15.0f, -18.0f, 15.0f, D3DX_PI * 0.75f, 0.0f, 0.0f);


		////Position and Draw a couple balls
		//Object[BALL].mtrl[0] = GREEN_MTRL;
		//DrawObject(BALL, -30.0f, -18.0f, -25.0f, 0.0f, 0.0f, 0.0f);
		//Object[BALL].mtrl[0] = BLUE_MTRL;
		//DrawObject(BALL, -35.0f, -18.0f, -28.0f, 0.0f, 0.0f, 0.0f);

		//DrawObject(TEAPOT, 20.0f, -8.6f, -20.0f, D3DX_PI * -.25f, 0.0f, 0.0f);

		

		////Position and Draw TV & STAND which also includes an xbox 360
		//DrawObject(TVSTAND, -30.0f, -9.0f, 30.0f, D3DX_PI * -.25f, 0.0f, 0.0f);

		////DrawObject(TVSTAND, posX+dirX*20.0f, posY+dirY*20.0f, posZ+dirZ*20.0f, 0.0f, 0.0f, 0.0f);

		////Position and Draw two a group of gamecases
		//DrawObject(GAMECASE, -30.0f, -13.5f, 30.0f, 0.0f, 0.0f, 0.0f);
		//DrawObject(GAMECASE, -35.0f, -19.5f, 18.0f, 0.0f, 0.0f, 0.0f);

		////Position and Draw 4 groups of cards
		//DrawObject(CARDS, 20.0f, -8.9f, -20.0f, D3DX_PI * -.25f, 0.0f, 0.0f );
		//DrawObject(CARDS, 20.0f, -8.9f, -20.0f, D3DX_PI * .25f, 0.0f, 0.0f );
		//DrawObject(CARDS, 20.0f, -8.9f, -20.0f, D3DX_PI * .75f, 0.0f, 0.0f );
		//DrawObject(CARDS, 20.0f, -8.9f, -20.0f, D3DX_PI * -.75f, 0.0f, 0.0f );

		D3DXMATRIX T3,R2;
		

		
		
		
		//End Room --------------------------------------------

		RECT rect;
		rect.top = rand()%10+80;
		rect.bottom = rect.top + 80;
		rect.left = rand()%10 + 150;
		rect.right = rect.left + 600;
		
		if (posX >= -2.0f && posX <= 2.0f && posZ >= 33.0f && posZ <= 39.0f &&
				dirZ >= 0.5f && dirZ <= 1.0f && dirY >= -0.75f && dirY <= 0.25f)
		{	
			if (lightOn)
				Font->DrawText(NULL,"Press Space Bar to Flip Light Switch Off",-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(0,130,255));
			else Font->DrawText(NULL,"Press Space Bar to Flip Light Switch On",-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(0,130,255));
		}
		if (posZ >= 950.0f && posZ <= 1000.0f || posZ <= -950.0f && posZ >= -1000.0f)
		{
			Font->DrawText(NULL,"NO TV AND XBOX 360 FOR YOU",-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(0,130,255));
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
		Device->SetTexture(0, Tex[Object[index].tex[i]]);
		Device->SetTransform(D3DTS_WORLD, &(Object[index].local[i] * R * T));
		Object[index].mesh[i]->DrawSubset(0);
	}
}

void DrawHallwaySeg(float z)
{
	D3DXMATRIX T, T2, R, R2;
	//Draw Walls
	Device->SetTexture(0, Tex[0]);
	Device->SetMaterial(&WHITE_MTRL);
	D3DXMatrixTranslation(&T2, 0.0f, 0.0f, z);
	D3DXMatrixTranslation(&T, 0.0f, 0.0f, 40.0f);
	
	D3DXMatrixRotationY(&R, D3DX_PI * .50f);
	Device->SetTransform(D3DTS_WORLD, &(T * R * T2));
	Wall->DrawSubset(0);
	
	D3DXMatrixRotationY(&R, D3DX_PI * 1.50f);
	Device->SetTransform(D3DTS_WORLD, &(T * R * T2));
	Wall->DrawSubset(0);

	//Draw Floor
	Device->SetTexture(0, Tex[1]);
	Device->SetMaterial(&WHITE_MTRL);
	D3DXMatrixTranslation(&T, -20.0f, -20.0f, 0.0f);
	D3DXMatrixRotationX(&R, 0.5f * D3DX_PI);
	D3DXMatrixRotationY(&R2, 0.5f * D3DX_PI);
	Device->SetTransform(D3DTS_WORLD, &(R * R2 * T * T2));
	Wall->DrawSubset(0);

	Device->SetTexture(0, Tex[1]);
	Device->SetMaterial(&WHITE_MTRL);
	D3DXMatrixTranslation(&T, 20.0f, -20.0f, 0.0f);
	D3DXMatrixRotationX(&R, 0.5f * D3DX_PI);
	D3DXMatrixRotationY(&R2, 0.5f * D3DX_PI);
	Device->SetTransform(D3DTS_WORLD, &(R * R2 * T * T2));
	Wall->DrawSubset(0);

	
	//Draw Ceiling
	Device->SetTexture(0, Tex[2]);
	Device->SetMaterial(&WHITE_MTRL);
	D3DXMatrixTranslation(&T, -20.0f, 20.0f, 00.0f);
	D3DXMatrixRotationX(&R, D3DX_PI * 1.50f);
	Device->SetTransform(D3DTS_WORLD, &(R * R2 * T * T2));
	Wall->DrawSubset(0);

	//Draw Ceiling
	Device->SetTexture(0, Tex[2]);
	Device->SetMaterial(&WHITE_MTRL);
	D3DXMatrixTranslation(&T, 20.0f, 20.0f, 00.0f);
	D3DXMatrixRotationX(&R, D3DX_PI * 1.50f);
	Device->SetTransform(D3DTS_WORLD, &(R * R2 * T * T2));
	Wall->DrawSubset(0);
}

void CreateTexBox(LPDIRECT3DDEVICE9 device, float width, float height, float depth, LPD3DXMESH *mesh)
{
	D3DXCreateMeshFVF(12, 24, 0, D3DFVF_CUSTOM, device, mesh);
	
	Vertex *v;
	(*mesh)->LockVertexBuffer(0,(void**)&v);
	//front face
	v[0] = Vertex(-width/2,-height/2,-depth/2, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-width/2, height/2,-depth/2, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f);
	v[2] = Vertex( width/2, height/2,-depth/2, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f);
	v[3] = Vertex( width/2,-height/2,-depth/2, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f);

	//right face
	v[4] = Vertex( width/2,-height/2,-depth/2, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[5] = Vertex( width/2, height/2,-depth/2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[6] = Vertex( width/2, height/2, depth/2, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[7] = Vertex( width/2,-height/2, depth/2, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//back face
	v[8]  = Vertex( width/2,-height/2, depth/2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[9]  = Vertex( width/2, height/2, depth/2, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[10] = Vertex(-width/2, height/2, depth/2, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-width/2,-height/2, depth/2, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	//left face
	v[12] = Vertex(-width/2,-height/2, depth/2,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[13] = Vertex(-width/2, height/2, depth/2,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[14] = Vertex(-width/2, height/2,-depth/2,-1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[15] = Vertex(-width/2,-height/2,-depth/2,-1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//top face
	v[16] = Vertex(-width/2, height/2,-depth/2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-width/2, height/2, depth/2, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex( width/2, height/2, depth/2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex( width/2, height/2,-depth/2, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	//bottom face
	v[20] = Vertex(-width/2,-height/2, depth/2, 0.0f,-1.0f, 0.0f, 0.0f, 1.0f);
	v[21] = Vertex(-width/2,-height/2,-depth/2, 0.0f,-1.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex( width/2,-height/2,-depth/2, 0.0f,-1.0f, 0.0f, 1.0f, 0.0f);
	v[23] = Vertex( width/2,-height/2, depth/2, 0.0f,-1.0f, 0.0f, 1.0f, 1.0f);
	(*mesh)->UnlockVertexBuffer();

	WORD *i;
	(*mesh)->LockIndexBuffer(0,(void**)&i);
	for (int j = 0, k = 0; j < 36; j += 6, k += 4)
	{
		i[j + 0] = k; i[j + 1] = k + 1; i[j + 2] = k + 2; //first tri of face
		i[j + 3] = k; i[j + 4] = k + 2; i[j + 5] = k + 3; //second tri of face
	}
	(*mesh)->UnlockIndexBuffer();
}

void CreateTexPlane(LPDIRECT3DDEVICE9 device, float width, float height, LPD3DXMESH *mesh)
{
	D3DXCreateMeshFVF(2, 4, 0, D3DFVF_CUSTOM, device, mesh);
	
	Vertex *v;
	(*mesh)->LockVertexBuffer(0,(void**)&v);
	//front face
	v[0] = Vertex(-width,-height, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-width, height, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f);
	v[2] = Vertex( width, height, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f);
	v[3] = Vertex( width,-height, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f);
	(*mesh)->UnlockVertexBuffer();

	WORD *i;
	(*mesh)->LockIndexBuffer(0,(void**)&i);
	
	i[0] = 0; i[1] = 1; i[2] = 2; //first tri of face
	i[3] = 0; i[4] = 2; i[5] = 3; //second tri of face
	
	(*mesh)->UnlockIndexBuffer();
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
	
	CreateTexPlane(Device, 10.0f, 10.0f, &Spiderweb);
	CreateTexPlane(Device, 8.0f, 6.0f, &Picture);
	//D3DXCreateBox(Device, 10.0f, 10.0f, 0.75f, &Spiderweb, 0);

	D3DXMATRIX temp, temp2;

	//Define Table -----------------------------------------------------------
	Object[0].num_parts = 5;

	//Table Top
	D3DXCreateCylinder(Device, 10.0f, 10.0f, 1.0f, 20, 4, &Object[0].mesh[0], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * 0.5f);
	D3DXMatrixTranslation(&temp2, 0.0f, 10.0f, 0.0f);
	Object[0].local[0] = temp * temp2;
	Object[0].mtrl[0] = BLUE_MTRL;

	//Table Legs
	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[1], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * 0.5f);
	D3DXMatrixTranslation(&temp2, 5.0f, 5.0f, 5.0f);
	Object[0].local[1] = temp * temp2;
	Object[0].mtrl[1] = BLUE_MTRL;

	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[2], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * 0.5f);
	D3DXMatrixTranslation(&temp2, -5.0f, 5.0f, 5.0f);
	Object[0].local[2] = temp * temp2;
	Object[0].mtrl[2] = BLUE_MTRL;

	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[3], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * 0.5f);
	D3DXMatrixTranslation(&temp2, -5.0f, 5.0f, -5.0f);
	Object[0].local[3] = temp * temp2;
	Object[0].mtrl[3] = BLUE_MTRL;

	D3DXCreateCylinder(Device, 1.0f, 0.5f, 10.0f, 8, 1, &Object[0].mesh[4], 0);
	D3DXMatrixRotationX(&temp, D3DX_PI * 0.5f);
	D3DXMatrixTranslation(&temp2, 5.0f, 5.0f, -5.0f);
	Object[0].local[4] = temp * temp2;
	Object[0].mtrl[4] = BLUE_MTRL;
	//End Table ----------------------------------------------------

	//Define Chair
	Object[1].num_parts = 6;
	Object[1].tex[0] = 1;
	//D3DXCreateBox(Device, 5.0f, 6.0f, 1.0f, &Object[1].mesh[0], 0);
	CreateTexBox(Device, 5.0f, 6.0f, 1.0f, &Object[1].mesh[0]);
	D3DXMatrixTranslation(&Object[1].local[0], 0.0f, 5.5f, 2.5f);
	Object[1].mtrl[0] = CHAIR_MTRL;

	//D3DXCreateBox(Device, 5.0f, 2.0f, 4.0f, &Object[1].mesh[1], 0);
	CreateTexBox(Device, 5.0f, 2.0f, 4.0f, &Object[1].mesh[1]);
	D3DXMatrixTranslation(&Object[1].local[1], 0.0f, 3.5f, 0.0f);
	Object[1].mtrl[1] = CHAIR_MTRL;

	//D3DXCreateBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[2], 0);
	CreateTexBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[2]);
	D3DXMatrixTranslation(&Object[1].local[2], 1.5f, 0.5f, 2.5f);
	Object[1].mtrl[2] = CHAIR_MTRL;

	CreateTexBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[3]);
	D3DXMatrixTranslation(&Object[1].local[3], -1.5f, 0.5f, 2.5f);
	Object[1].mtrl[3] = CHAIR_MTRL;

	CreateTexBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[4]);
	D3DXMatrixTranslation(&Object[1].local[4], -1.5f, 0.5f, -1.5f);
	Object[1].mtrl[4] = CHAIR_MTRL;

	CreateTexBox(Device, 1.0f, 5.0f, 1.0f, &Object[1].mesh[5]);
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
	CreateTexBox(Device, 15.0f, 22.0f, 0.5f, &Object[4].mesh[0]);
	D3DXMatrixIdentity(&Object[4].local[0]);
	Object[4].mtrl[0] = BOOKSHELF_MTRL;

	CreateTexBox(Device, 15.0f, 0.5f, 5.0f, &Object[4].mesh[1]);
	D3DXMatrixTranslation(&Object[4].local[1], 0.0f, 10.75f, -2.75f);
	Object[4].mtrl[1] = BOOKSHELF_MTRL;

	CreateTexBox(Device, 15.0f, 0.5f, 5.0f, &Object[4].mesh[2]);
	D3DXMatrixTranslation(&Object[4].local[2], 0.0f, -10.75f, -2.75f);
	Object[4].mtrl[2] = BOOKSHELF_MTRL;

	CreateTexBox(Device, 0.5f, 22.0f, 5.5f, &Object[4].mesh[3]);
	D3DXMatrixTranslation(&Object[4].local[3], 7.5f, 0.0f, -2.5f);
	Object[4].mtrl[3] = BOOKSHELF_MTRL;

	CreateTexBox(Device, 0.5f, 22.0f, 5.5f, &Object[4].mesh[4]);
	D3DXMatrixTranslation(&Object[4].local[4], -7.5f, 0.0f, -2.5f);
	Object[4].mtrl[4] = BOOKSHELF_MTRL;

	CreateTexBox(Device, 14.5f, 0.5f, 5.0f, &Object[4].mesh[5]);
	D3DXMatrixTranslation(&Object[4].local[5], 0.0f, 3.58f, -2.75f);
	Object[4].mtrl[5] = BOOKSHELF_MTRL;

	CreateTexBox(Device, 14.5f, 0.5f, 5.0f, &Object[4].mesh[6]);
	D3DXMatrixTranslation(&Object[4].local[6], 0.0f, -3.58f, -2.75f);
	Object[4].mtrl[6] = BOOKSHELF_MTRL;

	CreateTexBox(Device, 1.0f, 5.0f, 4.0f, &Object[4].mesh[7]);
	D3DXMatrixRotationZ(&temp, D3DX_PI * 0.15f);
	D3DXMatrixTranslation(&Object[4].local[7], -5.6f, -1.0f, -2.75f);
	Object[4].local[7] = temp * Object[4].local[7];
	Object[4].mtrl[7] = RED_MTRL;
	
	//TV & STAND
	//tv box
	Object[5].num_parts = 11;
	CreateTexBox(Device, 16.0f, 9.0f, 4.5f, &Object[5].mesh[0]);
	D3DXMatrixTranslation(&Object[5].local[0], 0.0f, 2.25f, 0.0f);
	Object[5].mtrl[0] = SILVER_MTRL;

	//tv screen
	CreateTexBox(Device, 12.8f, 7.2f, 0.1f, &Object[5].mesh[1]);
	D3DXMatrixTranslation(&Object[5].local[1], 0.0f, 2.25f, -2.25f);
	Object[5].mtrl[1] = BLACK_MTRL;

	//tv stand
	CreateTexBox(Device, 18.0f, 0.5f, 5.0f, &Object[5].mesh[2]);
	D3DXMatrixTranslation(&Object[5].local[2], 0.0f, -2.5f, 0.0f);
	Object[5].mtrl[2] = TVSTAND_MTRL;

	CreateTexBox(Device, 18.0f, 0.5f, 5.0f, &Object[5].mesh[3]);
	D3DXMatrixTranslation(&Object[5].local[3], 0.0f, -6.5f, 0.0f);
	Object[5].mtrl[3] = TVSTAND_MTRL;

	CreateTexBox(Device, 18.0f, 0.5f, 5.0f, &Object[5].mesh[4]);
	D3DXMatrixTranslation(&Object[5].local[4], 0.0f, -10.5f, 0.0f);
	Object[5].mtrl[4] = TVSTAND_MTRL;
	

	CreateTexBox(Device, 0.5f, 9.0f, 5.5f, &Object[5].mesh[5]);
	D3DXMatrixTranslation(&Object[5].local[5], 9.25f, -6.5f, 0.0f);
	Object[5].mtrl[5] = TVSTAND_MTRL;

	CreateTexBox(Device, 0.5f, 9.0f, 5.5f, &Object[5].mesh[6]);
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
	D3DXMatrixTranslation(&Object[5].local[9], 2.0f, -5.5, -2.0f);
	Object[5].local[9] = temp * Object[5].local[9];
	Object[5].mtrl[9] = GREEN_MTRL;

	D3DXCreateSphere(Device, 0.1f, 15, 15, &Object[5].mesh[10], 0);
	D3DXMatrixTranslation(&Object[5].local[10], 0.0f, -9.5f, -2.0f);
	Object[5].mtrl[10] = GREEN_MTRL;
	
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
	v[0] = Vertex(-40.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[1] = Vertex(-30.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.125f, 0.0f);
	v[2] = Vertex(-20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.25f, 0.0f);
	v[3] = Vertex(-10.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.375f, 0.0f);
	v[4] = Vertex(  0.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f);
	v[5] = Vertex( 10.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.625f, 0.0f);
	v[6] = Vertex( 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.75f, 0.0f);
	v[7] = Vertex( 30.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.875f, 0.0f);
	v[8] = Vertex( 40.0f, 20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[9] = Vertex(-40.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.25f);
	v[10] = Vertex(-30.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.125f, 0.25f);
	v[11] = Vertex(-20.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.25f, 0.25f);
	v[12] = Vertex(-10.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.375f, 0.25f);
	v[13] = Vertex(  0.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.25f);
	v[14] = Vertex( 10.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.625f, 0.25f);
	v[15] = Vertex( 20.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.75f, 0.25f);
	v[16] = Vertex( 30.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.875f, 0.25f);
	v[17] = Vertex( 40.0f, 10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.25f);

	v[18] = Vertex(-40.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.5f);
	v[19] = Vertex(-30.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.125f, 0.5f);
	v[20] = Vertex(-20.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.25f, 0.5f);
	v[21] = Vertex(-10.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.375f, 0.5f);
	v[22] = Vertex(  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.5f);
	v[23] = Vertex( 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.625f, 0.5f);
	v[24] = Vertex( 20.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.75f, 0.5f);
	v[25] = Vertex( 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.875f, 0.5f);
	v[26] = Vertex( 40.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.5f);

	v[27] = Vertex(-40.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.75f);
	v[28] = Vertex(-30.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.125f, 0.75f);
	v[29] = Vertex(-20.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.25f, 0.75f);
	v[30] = Vertex(-10.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.375f, 0.75f);
	v[31] = Vertex(  0.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.75f);
	v[32] = Vertex( 10.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.625f, 0.75f);
	v[33] = Vertex( 20.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.75f, 0.75f);
	v[34] = Vertex( 30.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.875f, 0.75f);
	v[35] = Vertex( 40.0f,-10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.75f);
	
	v[36] = Vertex(-40.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[37] = Vertex(-30.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.125f, 1.0f);
	v[38] = Vertex(-20.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.25f, 1.0f);
	v[39] = Vertex(-10.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.375f, 1.0f);
	v[40] = Vertex(  0.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 1.0f);
	v[41] = Vertex( 10.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.625f, 1.0f);
	v[42] = Vertex( 20.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.75f, 1.0f);
	v[43] = Vertex( 30.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.875, 1.0f);
	v[44] = Vertex( 40.0f,-20.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
	
	for (int i = 0; i < Wall->GetNumVertices(); i++)
	{
		v[i].u *= 2.0f;
	}
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
	
	D3DXCreateSphere(Device, 6.0f, 15, 15, &GhostHead, 0);
	GhostHead->CloneMeshFVF(0, D3DFVF_CUSTOM, Device, &GhostHead);

	GhostHead->LockVertexBuffer(0, (void**)&v);
	for( int i = 0; i < GhostHead->GetNumVertices(); i++)
	{
		v[i].u = (v[i].normal.x / 2) + 0.5f;
		v[i].v =-(v[i].normal.y / 2) + 0.5f;
	}
	GhostHead->UnlockVertexBuffer();

	D3DXCreateCylinder(Device, 6.0f, 4.0f, 9.5f, 12, 1, &GhostBody, 0);
	GhostBody->CloneMeshFVF(0, D3DFVF_CUSTOM, Device, &GhostBody);

	GhostBody->LockVertexBuffer(0, (void**)&v);
	for( int i = 0; i < GhostBody->GetNumVertices(); i++)
	{
		v[i].u = (v[i].normal.x / 2) + 0.5f;
		v[i].v =-(v[i].normal.y / 2) + 0.5f;
	}
	GhostBody->UnlockVertexBuffer();
		


	D3DXCreateTextureFromFile(Device, "Textures\\wall.bmp", &Tex[0]);
	D3DXCreateTextureFromFile(Device, "Textures\\floor.bmp", &Tex[1]);
	D3DXCreateTextureFromFile(Device, "Textures\\ceiling.bmp", &Tex[2]);
	D3DXCreateTextureFromFile(Device, "Textures\\Ghost1.dds", &Tex[3]);
	D3DXCreateTextureFromFile(Device, "Textures\\GhostBody1.dds", &Tex[4]);
	D3DXCreateTextureFromFile(Device, "Textures\\spiderweb.dds", &Tex[5]);
	D3DXCreateTextureFromFile(Device, "Textures\\pictureframe.bmp", &Tex[6]);
	D3DXCreateTextureFromFile(Device, "Textures\\bloodsplatter.dds", &Tex[7]);
	D3DXCreateTextureFromFile(Device, "Textures\\Rip.dds", &Tex[8]);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	Device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// use alpha channel in texture for alpha
	Device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	Device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// set blending factors so that alpha component determines transparency
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 dir(0.0f, 0.0f, 0.0f);
	D3DXCOLOR c(RED);
	Spot = InitSpotLight(&pos,&dir, &c);
	Device->SetLight(0, &Spot);

	Point = InitPointLight(&pos, &c);
	Device->SetLight(1, &Point);
	Device->LightEnable(1, true);
	
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);


	D3DXVECTOR3 position(0.0f, 0.0f, -5.0f);
	
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI*0.5f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	
	Device->SetRenderState(D3DRS_LIGHTING, true);
	
	return(1);
}

//kill everything, make sure there is no survivors
void GameMurder()
{
	if (teapot)
	{
		teapot->MURDER();
		teapot = 0;
	}
	if (Spiderweb)
	{
		Spiderweb->MURDER();
		Spiderweb = 0;
	}
	if (Picture)
	{
		Picture->MURDER();
		Picture = 0;
	}
	if (GhostHead)
	{
		GhostHead->MURDER();
		GhostHead = 0;
	}
	if (GhostBody)
	{
		GhostBody->MURDER();
		GhostBody = 0;
	}
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
	for (int i = 0; i < MAX_TEXTURES; i++)
	{
		if (Tex[i])
		{
			Tex[i]->Release();
			Tex[i] = 0;
		}
	}
	
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