// Collin Munson & Justin Knox

#include <stdio.h>
#include <stdlib.h>
#include "d3dbase.h"

// Defines
//---------------------------------------------------------------
#define KEYDOWN(vk) (GetAsyncKeyState(vk) & 0x8000) ? 1 : 0
#define KEYUP(vk) (GetAsyncKeyState(vk) & 0x8000) ? 0 : 1

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_Q 0x51 
#define VK_E 0x45 
#define VK_W 0x57 
#define VK_S 0x53 
#define VK_A 0x41 
#define VK_D 0x44 
#define VK_R 0x52 
#define VK_T 0x54
#define VK_H 0x48
#define VK_G 0x47
#define VK_C 0x43
#define VK_F 0x46

#define MURDER Release

#define MAX_OBJECTS 5

#define MAX_COLORS 16

enum{TEAPOT, CYLINDER, TORUS, SPHERE, DOLLAR, ALL};
enum{ROTATION, TRANSLATION, SCALING};


// Function Prototypes
//------------------------------------------------------------------------------------------------------------------------
int  GameMain(float);

int  GameInit();

void GameMurder();
//------------------------------------------------------------------------------------------------------------------------


// Structures
//---------------------------------------------------------------------------------------------------------------------------
struct
{
	float rotX, rotY, rotZ;
	D3DXMATRIX rotation;
	float posX, posY, posZ;
	float scaleX, scaleY, scaleZ;
	int cIndex, cIndex2;
	bool cChange;
	LPD3DXMESH mesh;
}Object[MAX_OBJECTS];


//--------------------------------------------------------------------------------------------------------------------------

// Globals
//-----------------------------------------------------------------------------------------------------------------------------

LPDIRECT3DDEVICE9 Device = 0;

LPD3DXFONT Font = 0;

LPDIRECT3DVERTEXBUFFER9 Lines = 0;


const D3DXVECTOR3 InitPos[] = { D3DXVECTOR3(5.0f, 5.0f, 0.0f), 
                                D3DXVECTOR3(5.0f, 0.0f, 0.0f), 
								D3DXVECTOR3(0.0f, 5.0f, 5.0f), 
								D3DXVECTOR3(-5.0f, 0.0f, 0.0f), 
								D3DXVECTOR3(0.0f, 0.0f, 0.0f) };



const D3DCOLOR Color[] = { D3DCOLOR_XRGB(255,  0,  0), D3DCOLOR_XRGB(  0,255,  0), 
                           D3DCOLOR_XRGB(  0,  0,255), D3DCOLOR_XRGB(255,255,  0),
                           D3DCOLOR_XRGB(255,  0,255), D3DCOLOR_XRGB(  0,255,255), 
						   D3DCOLOR_XRGB(255,127,  0), D3DCOLOR_XRGB(255,  0,127),
					       D3DCOLOR_XRGB(127,255,  0), D3DCOLOR_XRGB(  0,255,127), 
						   D3DCOLOR_XRGB(127,  0,255), D3DCOLOR_XRGB(  0,127,255),
					       D3DCOLOR_XRGB(127,127,127), D3DCOLOR_XRGB(255,127,127), 
						   D3DCOLOR_XRGB(127,255,127), D3DCOLOR_XRGB(127,127,255) };

const char *ObjStr[] = {"TEAPOT", "CYLINDER", "TORUS", "SPHERE", "DOLLAR", "ALL"};

const char *TypeStr[] = {"ROTATION", "TRANSLATION", "SCALING"};

int CurObj = ALL;

int TransType = ROTATION;

char Buffer[80];
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
	MessageBox(0, "Consult readme.txt for controls", "INSTRUCTIONS", 0);

	if (!InitD3D(hinstance, SCREEN_WIDTH, SCREEN_HEIGHT, false, D3DDEVTYPE_HAL, &Device))
	{
		MessageBox(0,"InitD3D() - FAILED", 0, 0);
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

		static float phi = 0.0f;
		static float theta = 0.0f;
		static float zoom = -10.0f;
		
		static float targetX = 0.0f;
		static float targetY = 0.0f;
		static float targetZ = 0.0f;
		
		static bool showAxisLines = false;

		for (int i = 0; i < MAX_OBJECTS; i++)
		{
			Object[i].rotX = Object[i].rotY = Object[i].rotZ = 0.0f;
			Object[i].cChange = false;
		}


// INPUT & LOGIC ///////////////////////////////////////////////////////////////////

		if (KEYDOWN(VK_UP))
		{
			phi -= deltatime * 2.0f;
		}
		if (KEYDOWN(VK_DOWN))
		{
			phi += deltatime * 2.0f;
		}
		if (KEYDOWN(VK_LEFT))
		{
			theta += deltatime * 2.0f;
		}
		if (KEYDOWN(VK_RIGHT))
		{
			theta -= deltatime * 2.0f;
		}
		if (KEYDOWN(VK_ADD))
		{
			zoom += deltatime * 3.0f;
		}
		if (KEYDOWN(VK_SUBTRACT))
		{
			zoom -= deltatime * 3.0f;
		}
		if (KEYDOWN(VK_1))
		{
			CurObj = TEAPOT;
		}
		if (KEYDOWN(VK_2))
		{
			CurObj = CYLINDER;
		}
		if (KEYDOWN(VK_3))
		{
			CurObj = TORUS;
		}
		if (KEYDOWN(VK_4))
		{
			CurObj = SPHERE;
		}
		if (KEYDOWN(VK_5))
		{
			CurObj = DOLLAR;
		}
		if (KEYDOWN(VK_6))
		{
			CurObj = ALL;
		}
		if (KEYDOWN(VK_TAB))
		{
			targetX = targetY = targetZ = 0.0f;
			phi = theta = 0.0f;
			zoom = -10.0f;
		}
		if (KEYDOWN(VK_SHIFT))
		{
			switch(TransType)
			{
				case ROTATION:
				{
					if (CurObj != ALL)
					{
						//Object[CurObj].rotX = Object[CurObj].rotY = Object[CurObj].rotZ = 0.0f;
						D3DXMatrixIdentity(&Object[CurObj].rotation);
					}
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							D3DXMatrixIdentity(&Object[i].rotation);
								//Object[i].rotX = Object[i].rotY = Object[i].rotZ = 0.0f;
					}
				}break;
				case TRANSLATION:
				{
					if (CurObj != ALL)
					{
						Object[CurObj].posX = InitPos[CurObj].x;
						Object[CurObj].posY = InitPos[CurObj].y;
						Object[CurObj].posZ = InitPos[CurObj].z;
					}
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
						{
							Object[i].posX = InitPos[i].x;
							Object[i].posY = InitPos[i].y;
							Object[i].posZ = InitPos[i].z;
						}
					}
	
				}break;
				case SCALING:
				{
					if (CurObj != ALL)
						Object[CurObj].scaleX = Object[CurObj].scaleY = Object[CurObj].scaleZ = 1.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
						{
							Object[i].scaleX = Object[i].scaleY = Object[i].scaleZ = 1.0f;
						}
					}
				}break;
			}
			
		}
		if(KEYDOWN(VK_Q))
		{
			switch(TransType)
			{
				case ROTATION:
				{
					if (CurObj != ALL)
						Object[CurObj].rotZ += deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
								Object[i].rotZ += deltatime * 2.0f;
					}
				}break;
				case TRANSLATION:
				{
					if (CurObj != ALL)
						Object[CurObj].posY -= deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].posY -= deltatime * 2.0f;
					}
				}break;
				case SCALING:
				{
					if (CurObj != ALL)
						Object[CurObj].scaleY -= deltatime * 1.1f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].scaleY -= deltatime * 1.1f;
					}
				}break;
			}
		}
		
		if(KEYDOWN(VK_E))
		{
			switch(TransType)
			{
				case ROTATION:
				{
					if (CurObj != ALL)
						Object[CurObj].rotZ -= deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
								Object[i].rotZ -= deltatime * 2.0f;
					}
				}break;
				case TRANSLATION:
				{
					if (CurObj != ALL)
						Object[CurObj].posY += deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].posY += deltatime * 2.0f;
					}
				}break;
				case SCALING:
				{
					if (CurObj != ALL)
						Object[CurObj].scaleY += deltatime * 1.1f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].scaleY += deltatime * 1.1f;
					}
				}break;
			}
		}

		if(KEYDOWN(VK_W))
		{
			switch(TransType)
			{
				case ROTATION:
				{
					if (CurObj != ALL)
						Object[CurObj].rotX += deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].rotX += deltatime * 2.0f;
					}
				}break;
				case TRANSLATION:
				{
					if (CurObj != ALL)
					{
						if (CurObj != ALL)
						Object[CurObj].posZ += deltatime * 2.0f;
						else
						{
							for (int i = 0; i < MAX_OBJECTS; i++)
								Object[i].posZ += deltatime * 2.0f;
						}
					}
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].posZ += deltatime * 2.0f;
					}
				}break;
				case SCALING:
				{
					if (CurObj != ALL)
						Object[CurObj].scaleZ += deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].scaleZ += deltatime * 1.1f;
					}
				}break;
			}
		}

		if(KEYDOWN(VK_S))
		{
			switch(TransType)
			{
				case ROTATION:
				{
					if (CurObj != ALL)
						Object[CurObj].rotX -= deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].rotX -= deltatime * 2.0f;
					}
				}break;
				case TRANSLATION:
				{
					if (CurObj != ALL)
						Object[CurObj].posZ -= deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].posZ -= deltatime * 2.0f;
					}
				}break;
				case SCALING:
				{
					if (CurObj != ALL)
						Object[CurObj].scaleZ -= deltatime * 1.1f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].scaleZ -= deltatime * 1.1f;
					}
				}break;
			}
		}

		if (KEYDOWN(VK_A))
		{
			switch(TransType)
			{
				case ROTATION:
				{
					if (CurObj != ALL)
						Object[CurObj].rotY += deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].rotY += deltatime * 2.0f;
					}
				}break;
				case TRANSLATION:
				{
					if (CurObj != ALL)
						Object[CurObj].posX -= deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].posX -=  deltatime * 2.0f;
					}
				}break;
				case SCALING:
				{
					if (CurObj != ALL)
						Object[CurObj].scaleX -= deltatime * 1.1f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].scaleX -= deltatime * 1.1f;
					}
				}break;
			}
		}

		if (KEYDOWN(VK_D))
		{
			switch(TransType)
			{
				case ROTATION:
				{
					if (CurObj != ALL)
						Object[CurObj].rotY -= deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].rotY -= deltatime * 2.0f;
					}
				}break;
				case TRANSLATION:
				{
					if (CurObj != ALL)
						Object[CurObj].posX += deltatime * 2.0f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].posX += deltatime * 2.0f;
					}
				}break;
				case SCALING:
				{
					if (CurObj != ALL)
						Object[CurObj].scaleX += deltatime * 1.1f;
					else
					{
						for (int i = 0; i < MAX_OBJECTS; i++)
							Object[i].scaleX += deltatime * 1.1f;
					}
				}break;
			}
		}

		if (KEYDOWN(VK_R))
		{
			TransType = ROTATION;
		}
		if (KEYDOWN(VK_T))
		{
			TransType = TRANSLATION;
		}

		if (KEYDOWN(VK_F))
		{
			TransType = SCALING;
		}
		
		static int count1 = 0, count2 = 0;
		if ((KEYDOWN(VK_G)) && GetTickCount() - count1 >= 200)
		{
			count1 = GetTickCount();
			if (CurObj != ALL)
			{
				Object[CurObj].cIndex++;
				Object[CurObj].cChange = true;
			}
			else
			{
				for (int i = 0; i < MAX_OBJECTS; i++)
				{
					Object[i].cIndex++;
					Object[i].cChange = true;
				}
			}
			
		}

		if ((KEYDOWN(VK_H)) && GetTickCount() - count2 >= 200)
		{
			count2 = GetTickCount();
			if (CurObj != ALL)
			{
				Object[CurObj].cIndex2++;
				Object[CurObj].cChange = true;
			}
			else
			{
				for (int i = 0; i < MAX_OBJECTS; i++)
				{
					Object[i].cIndex2++;
					Object[i].cChange = true;
				}
			}
			
		}
		if (KEYDOWN(VK_F1)) showAxisLines = true;
		if (KEYDOWN(VK_F2)) showAxisLines = false;
		
// LOGIC ///////////////////////////////////////////////////////////////////

		if (CurObj != ALL) 
		{
			targetX = Object[CurObj].posX;
			targetY = Object[CurObj].posY;
			targetZ = Object[CurObj].posZ;
		}
		for (int i = 0; i < MAX_OBJECTS; i++)
		{
			if (Object[i].scaleY < 0.1f) Object[i].scaleY = 0.1f;
			if (Object[i].scaleX < 0.1f) Object[i].scaleX = 0.1f;
			if (Object[i].scaleZ < 0.1f) Object[i].scaleZ = 0.1f;

			if (Object[i].cIndex >= MAX_COLORS) Object[i].cIndex = 0;
			if (Object[i].cIndex2 >= MAX_COLORS) Object[i].cIndex2 = 0;
		}

		if (phi > 1.57f) phi = 1.57f;
		else if (phi < -1.57f) phi = -1.57f;
		if (theta > 6.28f) theta = 0.0f;
		else if (theta < 0.0f) theta = 6.28f;
		if (zoom > -1.0f) zoom = -1.0f;

		// set view matrix
		//-------------------------------------------------------------
	
		float cameraX = (cosf(phi) * sinf(theta) * zoom) + targetX;
		float cameraY = (sinf(phi)               * zoom) + targetY; 
		float cameraZ = (cosf(phi) * cosf(theta) * zoom) + targetZ; 

		D3DXVECTOR3 position(cameraX, cameraY, cameraZ);
		
		D3DXVECTOR3 target(targetX, targetY, targetZ);
		
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		Device->SetTransform(D3DTS_VIEW, &V);
		//------------------------------------------------------------
	
		// change color per vertex
		//------------------------------------------------------------------
		for (int i = 0; i < MAX_OBJECTS; i++)
		{
			if (Object[i].cChange)
			{
				ColorVertex* v;
				Object[i].mesh->LockVertexBuffer(0, (void **)&v);
				for (int j = 0; j < Object[i].mesh->GetNumVertices(); j++)
				{	
					// smooth shading from front to back on DOLLAR & CYLINDER
					if ((i == DOLLAR || i == CYLINDER))
					{
						if (v[j].z < 0.0f)
							v[j].color = Color[Object[i].cIndex];
						else
							v[j].color = Color[Object[i].cIndex2];
					}
					// everything else random between the color and black
					else 
					{
						if (rand()%2)
							v[j].color = Color[Object[i].cIndex];
						else v[j].color = Color[Object[i].cIndex2];
					}
				}
				Object[i].mesh->UnlockVertexBuffer();
			}
		}
		//------------------------------------------------------------------
	
// RENDER ///////////////////////////////////////////////////////////////////////////////////////////////////		

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
		Device->BeginScene();

		// set transformation & draw objects
		//---------------------------------------------------------------------
		D3DXMATRIX W;
		//D3DXMatrixIdentity(&W);
		D3DXMatrixScaling(&W, 50.0f, 50.0f, 50.0f);
		Device->SetTransform(D3DTS_WORLD, &(W));
		if (showAxisLines)
		{
			Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
			Device->SetFVF(D3DFVF_COLOR);
			Device->SetStreamSource(0, Lines, 0, sizeof(ColorVertex));
			Device->DrawPrimitive(D3DPT_LINELIST, 0, 3);
		}
		for (int i = 0; i < MAX_OBJECTS; i++)
		{
			D3DXMATRIX R, T, S;
			D3DXMatrixScaling(&S, Object[i].scaleX, Object[i].scaleY, Object[i].scaleZ);
			D3DXMatrixRotationYawPitchRoll(&R, Object[i].rotY, Object[i].rotX, Object[i].rotZ);
			D3DXMatrixTranslation(&T, Object[i].posX, Object[i].posY,  Object[i].posZ);	
			Object[i].rotation = R * Object[i].rotation;
			W = S * Object[i].rotation * T;
			
			Device->SetTransform(D3DTS_WORLD, &W);
			if (showAxisLines)
			{
				Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
				Device->SetFVF(D3DFVF_COLOR);
				Device->SetStreamSource(0, Lines, 0, sizeof(ColorVertex));
				Device->DrawPrimitive(D3DPT_LINELIST, 0, 3);
			}
			Device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
			Object[i].mesh->DrawSubset(0);
		}
		//----------------------------------------------------------------------

		// display coordinates, transformations, current object
		//----------------------------------------------------------------------
		RECT rect;
		rect.top = 0;
		rect.bottom = 50;
		rect.left = 0;
		rect.right = 450;
		sprintf(Buffer,"Camera Coordinates\nx = %.2f, y = %.2f, z = %.2f", cameraX, cameraY, cameraZ);
		Font->DrawText(NULL,Buffer,-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(255,255,255));

		rect.top = 0;
		rect.bottom = 50;
		rect.left = 450;
		rect.right = 800;
		sprintf(Buffer,"Target/Object Coordinates\nx = %.2f, y = %.2f, z = %.2f", targetX, targetY, targetZ);
		Font->DrawText(NULL,Buffer,-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(255,255,255));

		rect.top = SCREEN_HEIGHT - 24;
		rect.bottom = SCREEN_HEIGHT;
		rect.left = 0;
		rect.right = 450;
		sprintf(Buffer,"Transform Type->[%s]",TypeStr[TransType]);
		Font->DrawText(NULL,Buffer,-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(255,255,255));

		rect.top = SCREEN_HEIGHT - 24;
		rect.bottom = SCREEN_HEIGHT;
		rect.left = 450;
		rect.right = SCREEN_WIDTH;
		sprintf(Buffer,"Current Object->[%s]",ObjStr[CurObj]);
		Font->DrawText(NULL,Buffer,-1,&rect,DT_TOP|DT_LEFT,D3DCOLOR_XRGB(255,255,255));
		//-------------------------------------------------------------------------

		Device->EndScene();
		
		Device->Present(0, 0, 0, 0);
		
	}
	return(1);
}

int GameInit()
{
	//setup font for outputting data
	//------------------------------------------------------------
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
	//------------------------------------------------------------

	//Create mesh for Teapot, Cylinder, Torus, Sphere, and Dollar Sign
	//-----------------------------------------------------------------
	D3DXCreateTeapot(Device,&Object[TEAPOT].mesh,0);

	D3DXCreateCylinder(Device, 1.0f, 1.0f, 3.0f, 12, 1, &Object[CYLINDER].mesh, 0);

	D3DXCreateTorus(Device, 0.7f, 1.5f, 10, 10, &Object[TORUS].mesh, 0);

	D3DXCreateSphere(Device, 2.0f, 15, 15, &Object[SPHERE].mesh, 0);

	HDC hdc = CreateCompatibleDC(0);
	LOGFONT lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfHeight = 25;
	lf.lfWidth = 12;
	lf.lfWeight = 500;
	lf.lfItalic = false;
	lf.lfUnderline = false;
	lf.lfStrikeOut = false;
	lf.lfCharSet = DEFAULT_CHARSET;
	strcpy(lf.lfFaceName, "Times New Roman");
	HFONT hFont;
	HFONT hFontOld;
	hFont = CreateFontIndirect(&lf);
	hFontOld = (HFONT)SelectObject(hdc, hFont);
	D3DXCreateText(Device, hdc,"$",0.001f,0.4f,&Object[DOLLAR].mesh,0,0);
	SelectObject(hdc,hFontOld);
	DeleteObject(hFont);
	DeleteDC(hdc);
	//----------------------------------------------------------------------
	

	//Change FVF for object mesh
	for (int i = 0; i < MAX_OBJECTS; i++)
		Object[i].mesh->CloneMeshFVF(D3DXMESH_WRITEONLY | D3DXMESH_MANAGED, D3DFVF_COLOR, Device, &Object[i].mesh);

	ColorVertex* v;
	Device->CreateVertexBuffer(6 * sizeof(ColorVertex), D3DUSAGE_WRITEONLY, D3DFVF_COLOR, D3DPOOL_MANAGED, &Lines, 0);
	Lines->Lock(0, 0, (void **)&v, 0);
	v[0] = ColorVertex(0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 0));
	v[1] = ColorVertex(3.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(255, 0, 255));
	v[2] = ColorVertex(0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0));
	v[3] = ColorVertex(0.0f, 3.0f, 0.0f, D3DCOLOR_XRGB(0, 255, 0));
	v[4] = ColorVertex(0.0f, 0.0f, 0.0f, D3DCOLOR_XRGB(0, 0, 255));
	v[5] = ColorVertex(0.0f, 0.0f, 3.0f, D3DCOLOR_XRGB(0, 0, 255));
	Lines->Unlock();

	//Initialize colors
	for (int i = 0; i < MAX_OBJECTS; i++)
	{	
		Object[i].mesh->LockVertexBuffer(0, (void **)&v);
		Object[i].cIndex = rand()%16;
		Object[i].cIndex2 = rand()%16;
		D3DCOLOR color1 = Color[Object[i].cIndex];
		D3DCOLOR color2 = Color[Object[i].cIndex2];
		for (int j = 0; j < Object[i].mesh->GetNumVertices(); j++)
		{	
			// smooth shading from front to back on DOLLAR & CYLINDER
			if ((i == DOLLAR || i == CYLINDER))
			{
				if (i == DOLLAR)
				{
					v[j].x -= 0.225f;
					v[j].y -= 0.29f;
					v[j].z += 0.2f;

					v[j].x *= 5.0f;
					v[j].y *= 5.0f;
					v[j].z *= 2.0f;
				}
				if (v[j].z < 0.0f)
					v[j].color = color1;
				else v[j].color = color2;
			}
			// everything else random between the color and black
			else 
			{
				if (rand()%2)
					v[j].color = color1;
				else v[j].color = color2;
			}
		}
		Object[i].mesh->UnlockVertexBuffer();
	}

	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		Object[i].posX =  InitPos[i].x;
		Object[i].posY =  InitPos[i].y;
		Object[i].posZ =  InitPos[i].z;

		Object[i].scaleX = 1.0f;
		Object[i].scaleY = 1.0f;
		Object[i].scaleZ = 1.0f;

		D3DXMatrixIdentity(&Object[i].rotation);
	}
	
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	
	Device->SetRenderState(D3DRS_LIGHTING, false);
	return(1);
}

void GameMurder()
{
	for (int i = 0; i < 5; i++)
	{
		if (Object[i].mesh)
		{
			Object[i].mesh->MURDER();
			Object[i].mesh = 0;
		}
	}
	if (Font) 
	{
		Font->MURDER();
		Font = 0;
	}
	if (Lines)
	{
		Lines->MURDER();
		Lines = 0;
	}
	if (Device)
	{
		Device->MURDER();
		Device = 0;
	}
}