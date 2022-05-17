// 

#include <stdio.h>
#include <stdlib.h>
#include "d3dbase.h"
#include "input.h"

// Defines
//---------------------------------------------------------------
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define MURDER Release
#define ERROR(msg) MessageBox(0,msg,"Error",MB_OK)


// Function Prototypes
//------------------------------------------------------------------------------------------------------------------------
int  GameMain(float);

int  GameInit();

void GameMurder();

void CreateTexBox(LPDIRECT3DDEVICE9 device, float width, float height, float depth, LPD3DXMESH *mesh);

//------------------------------------------------------------------------------------------------------------------------


// Globals
//-----------------------------------------------------------------------------------------------------------------------------

LPDIRECT3DDEVICE9 Device = 0;

LPD3DXFONT Font = 0;

LPDIRECT3DVERTEXBUFFER9 vb = 0;
LPDIRECT3DINDEXBUFFER9 ib = 0;
LPDIRECT3DTEXTURE9 Tex[8] = {0};

LPD3DXMESH Box, Cylinder, Torus, TorusTemp, Sphere, SphereTemp, StarsTemp, Stars, Base, Nose;
D3DLIGHT9 Point;
D3DMATERIAL9 Mtrl;

Input input;

HINSTANCE hinstance_app;
HWND hwnd_app;

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

	if (!InitD3D(hinstance, SCREEN_WIDTH, SCREEN_HEIGHT, false, D3DDEVTYPE_HAL, &Device))
	{
		ERROR("InitD3D() - FAILED");
		return(0);
	}
	hinstance_app = hinstance;
	if (!input.initDI())
	{
		ERROR("Could not initialize Direct Input");
		return(0);
	}

	if (!input.initKB())
	{
		ERROR("could not initialize keyboard");
		return(0);
	}

	if (!input.initmouse())
	{
		ERROR("could not initialize mouse");
		return(0);
	}
	
	srand((unsigned int)GetTickCount());
	if (!GameInit())
	{
		ERROR("GameInit() - FAILED");
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
		static float rot_obj = 0.0f;
		static float rot_all = 0.0f;
		static float speed = 2.5f;

// INPUT & LOGIC ///////////////////////////////////////////////////////////////////

		//read mouse input -----------------
		input.readmouse();
		theta += mouse_state.lX * .0025f;
		phi -= mouse_state.lY * .0025f;
		
		//read keyboard input ---------------
		input.readKB();
		if (kb_state[DIK_LEFT])
		{
			rot_all -= speed * deltatime;
		}
		if (kb_state[DIK_RIGHT])
		{
			rot_all += speed * deltatime;
		}
		if (kb_state[DIK_UP])
		{
			rot_obj -= speed * deltatime;
		}
		if (kb_state[DIK_DOWN])
		{
			rot_obj += speed * deltatime;
			
		}
		if (kb_state[DIK_W])
		{
			speed += 0.15f * deltatime;
		}
		if (kb_state[DIK_S])
		{
			speed -= 0.15f * deltatime;
			
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
		
// LOGIC ///////////////////////////////////////////////////////////////////
		
		posX += velX;
		posZ += velZ;
		if (rot_all < 0.0f) rot_all += 2 * D3DX_PI;
		else if (rot_all > 2.0f * D3DX_PI) rot_all -= 2 * D3DX_PI;

		
		// set view matrix
		//-------------------------------------------------------------
	
		
		
		
		
		//------------------------------------------------------------
	
	
// RENDER ///////////////////////////////////////////////////////////////////////////////////////////////////		

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(125,125,125), 1.0f, 0);
		Device->BeginScene();
		//Sun
		Device->SetMaterial(&SUN_MTRL);
		D3DXMATRIX T, R, S, R2, R3, S2;
		Device->SetTexture(0,Tex[3]);
		D3DXMatrixRotationY(&R, rot_all);
		D3DXMatrixTranslation(&T, 0.0f, 1.5f, 0.0f);
		D3DXMatrixRotationX(&R3, 3.14/2);
		D3DXMatrixScaling(&S,1,1,.1);
		Device->SetTransform(D3DTS_WORLD, &(T * R));
		Sphere->DrawSubset(0);
		Device->SetTexture(0,Tex[6]);
		Device->SetTransform(D3DTS_WORLD, &(S* R3 *T * R));
		Stars->DrawSubset(0);
		//Green Spokes
	/*	Device->SetTexture(0,0);
		D3DXMatrixRotationY(&R, rot_all);
		D3DXMatrixTranslation(&T, 0.0f, 0.5f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &(T * R));
		Device->SetMaterial(&GREEN_MTRL);
		Cylinder->DrawSubset(0);

		D3DXMatrixRotationY(&R, rot_all + D3DX_PI * 0.5f);
		D3DXMatrixTranslation(&T, -1.0f, 0.5f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &(T * R));
		Device->SetMaterial(&GREEN_MTRL);
		Cylinder->DrawSubset(0);

		D3DXMatrixRotationY(&R, rot_all + D3DX_PI * 0.5f);
		D3DXMatrixTranslation(&T, 1.0f, 0.5f, 0.0f);
		Device->SetTransform(D3DTS_WORLD, &(T * R));
		Device->SetMaterial(&GREEN_MTRL);
		Cylinder->DrawSubset(0);
		*/
		//Spaceship Box
		Device->SetTexture(0, Tex[0]);
		D3DXMatrixRotationY(&R2, rot_obj);
		D3DXMatrixRotationY(&R, rot_all);
		D3DXMatrixTranslation(&T, 0.0f, -0.2f, -3.3f);
		
		Device->SetTransform(D3DTS_WORLD, &(R2 * T * R));
		Device->SetMaterial(&WHITE_MTRL);
		Box->DrawSubset(0);
		D3DXMatrixRotationX(&R2, 3.14/2);
		D3DXMatrixTranslation(&T, 0.0f, .7f, -3.3f);
		D3DXMatrixScaling(&S,1,1,.25);
		Device->SetTexture(0,0);
		Device->SetMaterial(&WHITE_MTRL);
		Device->SetTransform(D3DTS_WORLD, &( S *R2 * T * R));
		Cylinder->DrawSubset(0);


		//Dophin Box
		Device->SetTexture(0, Tex[1]);
		D3DXMatrixRotationY(&R2, rot_obj);
		D3DXMatrixRotationY(&R, rot_all);
		D3DXMatrixTranslation(&T, 0.0f, -0.2f, 3.3f);
		
		Device->SetTransform(D3DTS_WORLD, &(R2 * T * R));
		Device->SetMaterial(&WHITE_MTRL);
		Box->DrawSubset(0);
		D3DXMatrixRotationX(&R2, 3.14/2);
		D3DXMatrixTranslation(&T, 0.0f, .7f, 3.3f);
		D3DXMatrixScaling(&S,1,1,.25);
		Device->SetTexture(0,0);
		Device->SetMaterial(&WHITE_MTRL);
		Device->SetTransform(D3DTS_WORLD, &( S *R2 * T * R));
		Cylinder->DrawSubset(0);

		//Uranus
		Device->SetTexture(0, Tex[5]);
		D3DXMatrixRotationY(&R2, rot_obj);
		D3DXMatrixRotationY(&R, rot_all);
		D3DXMatrixRotationX(&R3, 3.14/4);
		D3DXMatrixTranslation(&T, 3.3f, -0.5f, 1.0f);
		D3DXMatrixScaling(&S,.75,.75,.2);
		Device->SetTransform(D3DTS_WORLD, &(S *R3 * R2 * T * R));
		Torus->DrawSubset(0);
		Device->SetTransform(D3DTS_WORLD, &(R2 * T * R));
		Device->SetMaterial(&WHITE_MTRL);
		Sphere->DrawSubset(0);
		D3DXMatrixRotationX(&R2, 3.14/2);
		D3DXMatrixTranslation(&T, 3.3f, .7f, 1.0f);
		D3DXMatrixScaling(&S,1,1,.25);
		Device->SetTexture(0,0);
		Device->SetMaterial(&WHITE_MTRL);
		Device->SetTransform(D3DTS_WORLD, &( S *R2 * T * R));
		Cylinder->DrawSubset(0);

		//Shuttle
		Device->SetTexture(0, 0);
		D3DXMatrixRotationY(&R, rot_all);
		D3DXMatrixTranslation(&T, 2.3f, -0.2f, -3.0f);
		Device->SetTransform(D3DTS_WORLD, &(T * R));
		Device->SetMaterial(&GREEN_MTRL);
		Base->DrawSubset(0);
		D3DXMatrixRotationX(&R2, 3.14/2);
		D3DXMatrixTranslation(&T, 2.3f, .7f, -3.0f);
		D3DXMatrixScaling(&S,1,1,.25);
		Device->SetTexture(0,0);
		Device->SetMaterial(&WHITE_MTRL);
		Device->SetTransform(D3DTS_WORLD, &( S *R2 * T * R));
		Cylinder->DrawSubset(0);
		D3DXMatrixTranslation(&T, 2.3f, -0.2f, -2.2f);
		Device->SetTransform(D3DTS_WORLD, &( T * R));
		Device->SetMaterial(&RED_MTRL);
		Nose->DrawSubset(0);
		D3DXMatrixTranslation(&T, 2.3f, -0.2f, -4.0f);
		Device->SetTransform(D3DTS_WORLD, &( T * R));
		Device->SetMaterial(&RED_MTRL);
		Nose->DrawSubset(0);
		//Astronaut  
		D3DXMatrixRotationY(&R2, rot_obj);
		D3DXMatrixRotationY(&R, rot_all);
		D3DXMatrixTranslation(&T, -2.3f, -0.2f, -2.0f);
		Device->SetTransform(D3DTS_WORLD, &(R2 * T * R));
		Device->SetMaterial(&WHITE_MTRL);
		Device->SetTexture(0,Tex[7]);
		Box->DrawSubset(0);
		D3DXMatrixRotationX(&R2, 3.14/2);
		D3DXMatrixTranslation(&T, -2.3f, .7f, -2.0f);
		D3DXMatrixScaling(&S,1,1,.25);
		Device->SetTexture(0,0);
		Device->SetMaterial(&WHITE_MTRL);
		Device->SetTransform(D3DTS_WORLD, &( S *R2 * T * R));
		Cylinder->DrawSubset(0);

		//Saturn
		Device->SetTexture(0,Tex[4]);
		D3DXMatrixRotationY(&R2, rot_obj);
		D3DXMatrixRotationY(&R, rot_all);
		D3DXMatrixRotationX(&R3,  3.14/2);

		D3DXMatrixTranslation(&T, -3.3f, -0.5f, 1.0f);
		D3DXMatrixScaling(&S,1,1,.3);
		Device->SetTransform(D3DTS_WORLD, &(S *R3 *R2 * T * R));
		Torus->DrawSubset(0);
		Device->SetTransform(D3DTS_WORLD, &(R2 * T * R));
		Device->SetMaterial(&WHITE_MTRL);
		Sphere->DrawSubset(0);
		D3DXMatrixRotationX(&R2, 3.14/2);
		D3DXMatrixTranslation(&T, -3.3f, .7f, 1.0f);
		D3DXMatrixScaling(&S,1,1,.25);
		Device->SetTexture(0,0);
		Device->SetMaterial(&WHITE_MTRL);
		Device->SetTransform(D3DTS_WORLD, &( S *R2 * T * R));
		Cylinder->DrawSubset(0);

		//DrawBox();

		D3DXMatrixRotationY(&R2, rot_obj);
		D3DXMatrixRotationY(&R, rot_all);
		D3DXMatrixTranslation(&T, 0.0f, -0.5f, 2.0f);
		D3DXMatrixScaling(&S, 0.3f, 0.3f, 0.3f);
		Device->SetTransform(D3DTS_WORLD, &(S * R2 * T * R));
		Device->SetMaterial(&WHITE_MTRL);
		//DrawBox();

		Device->EndScene();
		
		Device->Present(0, 0, 0, 0);
		
	}
	return(1);
}

void CreateTexBox(LPDIRECT3DDEVICE9 device, float width, float height, float depth, LPD3DXMESH *mesh)
{
	D3DXCreateMesh(12, 24, 0, 0, device, mesh);
	(*mesh)->CloneMeshFVF(0, D3DFVF_CUSTOM, device, mesh);
	Vertex *v;
	(*mesh)->LockVertexBuffer(0,(void**)&v);
	//front face
	v[0] = Vertex(-width,-height,-depth, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-width, height,-depth, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f);
	v[2] = Vertex( width, height,-depth, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f);
	v[3] = Vertex( width,-height,-depth, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f);

	//right face
	v[4] = Vertex( width,-height,-depth, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[5] = Vertex( width, height,-depth, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[6] = Vertex( width, height, depth, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[7] = Vertex( width,-height, depth, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//back face
	v[8]  = Vertex( width,-height, depth, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[9]  = Vertex( width, height, depth, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[10] = Vertex(-width, height, depth, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-width,-height, depth, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	//left face
	v[12] = Vertex(-width,-height, depth,-1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[13] = Vertex(-width, height, depth,-1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[14] = Vertex(-width, height,-depth,-1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[15] = Vertex(-width,-height,-depth,-1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	//top face
	v[16] = Vertex(-width, height,-depth, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-width, height, depth, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex( width, height, depth, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex( width, height,-depth, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	//bottom face
	v[20] = Vertex(-width,-height, depth, 0.0f,-1.0f, 0.0f, 0.0f, 1.0f);
	v[21] = Vertex(-width,-height,-depth, 0.0f,-1.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex( width,-height,-depth, 0.0f,-1.0f, 0.0f, 1.0f, 0.0f);
	v[23] = Vertex( width,-height, depth, 0.0f,-1.0f, 0.0f, 1.0f, 1.0f);
	(*mesh)->UnlockVertexBuffer();

	WORD *i;
	(*mesh)->LockIndexBuffer(0,(void**)&i);
	for (int j = 0, k = 0; j < 32; j += 6, k += 4)
	{
		i[j + 0] = k; i[j + 1] = k + 1; i[j + 2] = k + 2; //first tri of face
		i[j + 3] = k; i[j + 4] = k + 2; i[j + 5] = k + 3; //second tri of face
	}
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
	
	D3DXCreateBox(Device, 0.3f, 0.3f, 0.3f, &Box, 0);
	CreateTexBox(Device, 0.5f, 0.5f, 0.5f, &Box);
	//D3DXCreateBox(Device, 0.3f, 0.3f, 0.75f, &Box2, 0);
	D3DXCreateCylinder(Device,.25,.25,1.5,100,100,&Base,0);
	D3DXCreateCylinder(Device,.01,.25,.5,100,100,&Nose,0);
	D3DXCreateSphere(Device,1,20,20,&SphereTemp,0);
	SphereTemp->CloneMeshFVF(0,D3DFVF_CUSTOM,Device,&Sphere);

	LPDIRECT3DVERTEXBUFFER9 tempVertexBuffer;

	Sphere->GetVertexBuffer(&tempVertexBuffer);
	
		
	
	Vertex *Vertices = NULL;
	tempVertexBuffer->Lock(0,0,(void**)&Vertices,0);
	int numVerts = Sphere->GetNumVertices();
	
	for(int i = 0; i<numVerts;i++)
	{
	Vertices[i].v = Vertices[i].normal.y / 2 + 0.5f;
	Vertices[i].u = Vertices[i].normal.x / 2 + 0.5f;
	
	}

	tempVertexBuffer->Unlock();
	tempVertexBuffer->Release();

	

	D3DXCreateTorus(Device,.09,1.5,20,20,&TorusTemp,0);
	TorusTemp->CloneMeshFVF(0,D3DFVF_CUSTOM,Device,&Torus);
	Torus->GetVertexBuffer(&tempVertexBuffer);
	
		
	
	tempVertexBuffer->Lock(0,0,(void**)&Vertices,0);
	numVerts = Torus->GetNumVertices();
	
	for(int i = 0; i<numVerts;i++)
	{
	Vertices[i].v = Vertices[i].pos.y / 2 + 0.5f;
	Vertices[i].u = Vertices[i].pos.x / 2 + 0.5f;
	
	}

	tempVertexBuffer->Unlock();
	tempVertexBuffer->Release();

	
	D3DXCreateTorus(Device,2,4,20,20,&StarsTemp,0);
	StarsTemp->CloneMeshFVF(0,D3DFVF_CUSTOM,Device,&Stars);
	Stars->GetVertexBuffer(&tempVertexBuffer);
	
		
	
	tempVertexBuffer->Lock(0,0,(void**)&Vertices,0);
	numVerts = Stars->GetNumVertices();
	
	for(int i = 0; i<numVerts;i++)
	{
	Vertices[i].v = Vertices[i].pos.y / 2 + 0.5f;
	Vertices[i].u = Vertices[i].pos.x / 2 + 0.5f;
	
	}

	tempVertexBuffer->Unlock();
	tempVertexBuffer->Release();


	
	


	D3DXCreateCylinder(Device, 0.05f, 0.05f, 8.0f, 10, 10, &Cylinder,0);

	D3DXMATRIX temp, temp2;

	D3DXCOLOR c(WHITE);
	D3DXVECTOR3 pos(0.0f,0.0f, 1.0f);
	
	Point = InitDirectionalLight(&pos,&c);
	Device->SetLight(0, &Point);
	Device->LightEnable(0, true);
	
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);

	D3DXCreateTextureFromFile(Device, "spaceship.bmp", &Tex[0]);
	D3DXCreateTextureFromFile(Device, "Alien.bmp", &Tex[1]);
	D3DXCreateTextureFromFile(Device, "SpaceDolphins.bmp", &Tex[2]);
	D3DXCreateTextureFromFile(Device, "sunmap.bmp", &Tex[3]);
	D3DXCreateTextureFromFile(Device, "Saturn.bmp", &Tex[4]);
	D3DXCreateTextureFromFile(Device, "Earth.bmp", &Tex[5]);
	D3DXCreateTextureFromFile(Device, "stars.bmp", &Tex[6]);
	D3DXCreateTextureFromFile(Device, "astronaut.bmp", &Tex[7]);

	
	
	
	Device->SetTexture(0, Tex[0]);
	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	Device->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	D3DXVECTOR3 position(0.0f, 0.0f, -10.0f);
	
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.5f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	
	Device->SetRenderState(D3DRS_LIGHTING, true);
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
	
	if (Box)
	{
		Box->MURDER();
		Box = 0;
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