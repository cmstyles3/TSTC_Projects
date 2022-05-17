/*
LAB07 - COLLISION DETECTION - UNIFORM GRID
PROGRAMMED BY: Collin Munson
DESCRIPTION: Implements AABB, Bounding spheres, and OBB
consult readme.txt for controls

*/

#include "d3dapp.h" 
#include "geometry.h"
#include "xFileLoad.h"
#include "xFileSave.h" 
#include "collision.h"
#include <queue>


// {4C3EBA00-217A-447d-B406-B8828A685524}
DEFINE_GUID(HIGHSCORES_GUID, 
0x4c3eba00, 0x217a, 0x447d, 0xb4, 0x6, 0xb8, 0x82, 0x8a, 0x68, 0x55, 0x24);


// Size of each cell for uniform grid
#define CELL_SIZE 30.0f

#define GRID_ROWS 17
#define GRID_COLS 21

struct Object3D
{
	D3DXVECTOR3 right; // x-axis
	D3DXVECTOR3 up;    // y-axis
	D3DXVECTOR3 look;  // z-axis

	void Reset()
	{
		right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		up    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		look  = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}

	Object3D()
	{
		Reset();
	}
	
	void Roll(float angle)
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &look, angle);

		D3DXVec3TransformCoord(&right, &right, &T);
		D3DXVec3TransformCoord(&up, &up, &T);
	}
	void SetWorldMatrix(float x, float y, float z)
	{
		D3DXMATRIX world, rot, trans;
		D3DXVec3Normalize(&look, &look);

		D3DXVec3Cross(&up, &look, &right);
		D3DXVec3Normalize(&up, &up);

		D3DXVec3Cross(&right, &up, &look);
		D3DXVec3Normalize(&right, &right);

		D3DXMatrixIdentity(&rot);
		rot(0, 0) = right.x;	rot(0, 1) = up.x;  rot(0, 2) = look.x;  rot(0, 3) = 0.0f;

		rot(1, 0) = right.y;  rot(1, 1) = up.y;  rot(1, 2) = look.y;  rot(1, 3) = 0.0f;

		rot(2, 0) = right.z; 	rot(2, 1) = up.z;  rot(2, 2) = look.z;  rot(2, 3) = 0.0f;

		D3DXMatrixTranslation(&trans, x, y, z);
		D3DXMatrixInverse(&rot, 0, &rot);

		world = rot * trans;
		g_device->SetTransform(D3DTS_WORLD, &world);
	}
};

// GAME STATES
enum 
{
	DISPLAY_START, 
	INIT_LEVEL1, PLAY_LEVEL1, 
	INIT_LEVEL2, PLAY_LEVEL2, 
	INIT_LEVEL3, PLAY_LEVEL3, 
	INIT_RESULTS, DISPLAY_RESULTS,
	START_LEVEL1, START_LEVEL2, START_LEVEL3
};

#define NUM_OBJECTS 5

// used to flag objects in cells
struct Cell
{
	std::queue<int> id;
};

// COLORS USED FOR OBJECTS
const DWORD Color[] = 
{ 
	D3DCOLOR_XRGB(255, 0, 0), 
	D3DCOLOR_XRGB(0, 255, 0), 
	D3DCOLOR_XRGB(0, 0, 255),
	D3DCOLOR_XRGB(255, 255, 0),
	D3DCOLOR_XRGB(0, 255, 255)
};


struct Object
{
	D3DXVECTOR3 position;
	float speed;
	bool hit;
};

class MAIN : public D3DApp
{
	public:
		MAIN(Setup *setup);
		~MAIN();
		

		// Framework override functions
		void Update(float dt);
		void Draw(float dt);
		void OnLostDevice();
		void OnResetDevice();

		void SetD3DOptions();

		void LoadHighscores();
		void SaveHighscores();

		void InitLevel1();
		void UpdateLevel1(float dt);
		void DrawLevel1();

		void InitLevel2();
		void UpdateLevel2(float dt);
		void DrawLevel2();

		void InitLevel3();
		void UpdateLevel3(float dt);
		void DrawLevel3();

		void ResetGrid();

	private:
		LPD3DXSPRITE m_sprite;
		LPD3DXFONT m_font;
		LPDIRECT3DTEXTURE9 m_startBkgTex, 
			               m_level1BkgTex, 
						   m_level2BkgTex, 
						   m_level3BkgTex,
						   m_resultBkgTex,
						   m_pauseTex;

//		Object m_objects[6];
		LPD3DXMESH m_mesh;

		D3DXVECTOR3 m_position[2];
		//D3DXVECTOR3 m_velocity[2];
		float m_speed;

		float m_angle;
		
		LPD3DXMESH m_sphereMesh;
	

		LPD3DXMESH m_level1Meshs[2];
		LPD3DXMESH m_level1Boxes[2];
		AABB m_aabb[2];

		LPD3DXMESH m_level2Meshs[2];
		LPD3DXMESH m_level2Spheres[2];
		Sphere m_sphere[2];

		LPD3DXMESH m_level3Meshs[2];
		LPD3DXMESH m_level3Boxes[2];
		OBB m_obb[2];
		Object3D m_object3D[2];
		
		float m_level1Timer;
		float m_level2Timer;
		float m_level3Timer;
		bool m_paused;
		float m_startTimer;

		int m_currState;

		DWORD m_highscores[5];
		DWORD m_totalScore;
		bool m_newHighscore;
		DWORD m_rank;

		bool m_showBoundingVolume;

		int m_collisionCount;

		LPD3DXLINE m_gridLines;
		D3DXVECTOR3 m_cellLines[3];

		D3DXMATRIX proj, view;

		Cell m_grid[GRID_ROWS][GRID_COLS];
		Object m_objects[NUM_OBJECTS];


};


int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int)
{
	Setup setup;
	ZeroMemory(&setup, sizeof(Setup));
	strcpy(setup.appName, "LAB05 - POLYGON WARS");
	setup.width = 800;
	setup.height = 600;
	setup.instance = instance;
	setup.windowed = false;

	MAIN testApp(&setup);
	g_app = &testApp;

	return(g_app->run());
}

MAIN::MAIN(Setup *setup) : D3DApp(setup)
{
	m_currState = DISPLAY_START;

	LoadHighscores();

	D3DXFONT_DESC fontDesc;
	fontDesc.Height = 28;
	fontDesc.Width = 15;
	fontDesc.Weight = FW_BOLD;
	fontDesc.MipLevels = 0;
	fontDesc.Italic = false;
	fontDesc.CharSet = DEFAULT_CHARSET;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality = DEFAULT_QUALITY;
	fontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy(fontDesc.FaceName, "Times New Roman");
	D3DXCreateFontIndirect(g_device, &fontDesc, &m_font);


	SetD3DOptions();


	D3DXCreateSprite(g_device, &m_sprite);

	D3DXCreateLine(g_device, &m_gridLines);
	m_cellLines[0] = D3DXVECTOR3(0.0f, CELL_SIZE, 0.0f);
	m_cellLines[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_cellLines[2] = D3DXVECTOR3(CELL_SIZE, 0.0f, 0.0f);
	
	

	// LOAD TEXTURES ------------------------------------------------------------------
	D3DXCreateTextureFromFile(g_device, "level1.bmp", &m_level1BkgTex);
	D3DXCreateTextureFromFile(g_device, "level2.bmp", &m_level2BkgTex);
	D3DXCreateTextureFromFile(g_device, "level3.bmp", &m_level3BkgTex);
	D3DXCreateTextureFromFile(g_device, "pause.dds", &m_pauseTex);
	D3DXCreateTextureFromFile(g_device, "start.bmp", &m_startBkgTex);
	D3DXCreateTextureFromFile(g_device, "results.bmp", &m_resultBkgTex);


	// CREATE MESHES for LEVEL 1 -------------------------------------------------------

	Vertex *vertices;
	WORD *indices;

	// mesh 1
	D3DXCreateMeshFVF(16, 18, D3DXMESH_MANAGED, D3DFVF_VERTEX, g_device, &m_level1Meshs[0]);

	// Set Vertex Buffer
	m_level1Meshs[0]->LockVertexBuffer(0, (void**)&vertices);
	memcpy(vertices, vertices0, sizeof(vertices0));
	m_level1Meshs[0]->UnlockVertexBuffer();
	MeshScale(m_level1Meshs[0], 3.8);
	m_level1Meshs[0]->LockVertexBuffer(0, (void**)&vertices);
	D3DXComputeBoundingBox(&vertices[0].pos, m_level1Meshs[0]->GetNumVertices(), m_level1Meshs[0]->GetNumBytesPerVertex(), &m_aabb[0].min, &m_aabb[0].max);
	m_level1Meshs[0]->UnlockVertexBuffer();

	// Set Index Buffer
	m_level1Meshs[0]->LockIndexBuffer(0, (void**)&indices);
	memcpy(indices, indices0, sizeof(indices0));
	m_level1Meshs[0]->UnlockIndexBuffer();

	D3DXCreateBox(g_device, (m_aabb[0].max.x - m_aabb[0].min.x), 
		                    (m_aabb[0].max.y - m_aabb[0].min.y), 
		                    (m_aabb[0].max.z - m_aabb[0].min.z), 
							&m_level1Boxes[0], 0);

	m_obb[0].e[0] = m_aabb[0].max.x;
	m_obb[0].e[1] = m_aabb[0].max.y;
	m_obb[0].e[2] = m_aabb[0].max.z;


	// mesh 2
	D3DXCreateMeshFVF(12, 14, D3DXMESH_MANAGED, D3DFVF_VERTEX, g_device, &m_level1Meshs[1]);
	m_level1Meshs[1]->LockVertexBuffer(0, (void**)&vertices);
	memcpy(vertices, vertices1, sizeof(vertices1));
	m_level1Meshs[1]->UnlockVertexBuffer();
	MeshScale(m_level1Meshs[1], 4.0f);
	m_level1Meshs[1]->LockVertexBuffer(0, (void**)&vertices);
	D3DXComputeBoundingBox(&vertices[0].pos, m_level1Meshs[1]->GetNumVertices(), m_level1Meshs[1]->GetNumBytesPerVertex(), &m_aabb[1].min, &m_aabb[1].max);
	m_level1Meshs[1]->UnlockVertexBuffer();

	
	m_level1Meshs[1]->LockIndexBuffer(0, (void**)&indices);
	memcpy(indices, indices1, sizeof(indices1));
	m_level1Meshs[1]->UnlockIndexBuffer();

	D3DXCreateBox(g_device, (m_aabb[1].max.x - m_aabb[1].min.x), 
		                    (m_aabb[1].max.y - m_aabb[1].min.y), 
		                    (m_aabb[1].max.z - m_aabb[1].min.z), 
							&m_level1Boxes[1], 0);
	
	m_obb[1].e[0] = m_aabb[1].max.x;
	m_obb[1].e[1] = m_aabb[1].max.y;
	m_obb[1].e[2] = m_aabb[1].max.z;



	// CREATE MESHES FOR LEVEL 2 ----------------------------------------------------------------
	// mesh 1
	D3DXCreateMeshFVF(16, 17, D3DXMESH_MANAGED, D3DFVF_VERTEX, g_device, &m_level2Meshs[0]);
	m_level2Meshs[0]->LockVertexBuffer(0, (void**)&vertices);
	memcpy(vertices, vertices2, sizeof(vertices2));
	m_level2Meshs[0]->UnlockVertexBuffer();
	MeshScale(m_level2Meshs[0], 13.0f);
	m_level2Meshs[0]->LockVertexBuffer(0, (void**)&vertices);
	D3DXComputeBoundingSphere(&vertices[0].pos, m_level2Meshs[0]->GetNumVertices(), m_level2Meshs[0]->GetNumBytesPerVertex(), &m_sphere[0].center, &m_sphere[0].radius);
	m_level2Meshs[0]->UnlockVertexBuffer();

	m_level2Meshs[0]->LockIndexBuffer(0, (void**)&indices);
	memcpy(indices, indices2, sizeof(indices2));
	m_level2Meshs[0]->UnlockIndexBuffer();

	D3DXCreateSphere(g_device, m_sphere[0].radius, 10, 10, &m_level2Spheres[0], 0);

	// mesh 2
	D3DXCreateMeshFVF(14, 16, D3DXMESH_MANAGED, D3DFVF_VERTEX, g_device, &m_level2Meshs[1]);
	m_level2Meshs[1]->LockVertexBuffer(0, (void**)&vertices);
	memcpy(vertices, vertices3, sizeof(vertices3));
	m_level2Meshs[1]->UnlockVertexBuffer();
	MeshScale(m_level2Meshs[1], 4.4f);
	m_level2Meshs[1]->LockVertexBuffer(0, (void**)&vertices);
	D3DXComputeBoundingSphere(&vertices[0].pos, m_level2Meshs[1]->GetNumVertices(), m_level2Meshs[1]->GetNumBytesPerVertex(), &m_sphere[1].center, &m_sphere[1].radius);
	m_level2Meshs[1]->UnlockVertexBuffer();

	m_level2Meshs[1]->LockIndexBuffer(0, (void**)&indices);
	memcpy(indices, indices3, sizeof(indices3));
	m_level2Meshs[1]->UnlockIndexBuffer();

	D3DXCreateSphere(g_device, m_sphere[1].radius, 10, 10, &m_level2Spheres[1], 0);

	for (int i = 0; i < 2; i++)
	{
		DWORD* adjacencyInfo = new DWORD[m_level1Meshs[i]->GetNumFaces()*3];
		m_level1Meshs[i]->GenerateAdjacency(0.001f, adjacencyInfo);
		m_level1Meshs[i]->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, adjacencyInfo, 0, 0, 0);
		delete [] adjacencyInfo;
	}


	// ------------------------------------------------------------------------------------


	m_position[0].x = m_position[0].y = m_position[0].z = 0.0f;
	m_angle = 0.0f;

	m_paused = false;
	
	m_object3D[0].Reset();
	m_object3D[1].Reset();
	ShowCursor(false);
}

MAIN::~MAIN()
{
	// Release LEVEL 1 MESHES
	for (int i = 0; i < 2; i++)
	{
		SAFE_RELEASE(m_level1Meshs[i]);
		SAFE_RELEASE(m_level1Boxes[i]);
	}

	// Release LEVEL 2 MESHES
	for (int i = 0; i < 1; i++)
	{
		SAFE_RELEASE(m_level2Meshs[i]);
		SAFE_RELEASE(m_level2Spheres[i]);
	}

	// Release Textures
	SAFE_RELEASE(m_startBkgTex);
	SAFE_RELEASE(m_pauseTex);
	SAFE_RELEASE(m_level1BkgTex);
	SAFE_RELEASE(m_level2BkgTex);
	SAFE_RELEASE(m_level3BkgTex);

	SAFE_RELEASE(m_font);
}


void MAIN::SetD3DOptions()
{
	//SETUP PROJECTION AND VIEW MATRIX
	
	D3DXMatrixOrthoLH(&proj, 400, 300, 1.0, 60.0);
	g_device->SetTransform(D3DTS_PROJECTION, &proj);
	
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(0.0f, 0.0f, -60.0f), 
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	g_device->SetTransform(D3DTS_VIEW, &view);

	// TURN OFF LIGHTING
	g_device->SetRenderState(D3DRS_LIGHTING, false);
}


void MAIN::OnLostDevice()
{

	m_font->OnLostDevice();
	m_sprite->OnLostDevice();
	m_gridLines->OnLostDevice();
}

void MAIN::OnResetDevice()
{
	m_font->OnResetDevice();
	m_sprite->OnResetDevice();
	m_gridLines->OnResetDevice();

	SetD3DOptions();
}

void MAIN::Update(float dt)
{
	/*
	XBOX 360 GAMEPAD
	A - 0
	B - 1
	X - 2
	Y - 3
	LB - 4
	RB - 5
	BACK - 6
	START - 7
	LTHUMB - 8
	RTHUMB - 9
	LEFT TRIGGER - +Z-AXIS
	RIGHT TRIGGER - -Z-AXIS
	*/
	float timer;
	switch(m_currState)
	{
		case DISPLAY_START:
			if ((GetInput()->KeyDown(DIK_ESCAPE)   && !GetInput()->PrevKeyDown(DIK_ESCAPE)) ||
				(GetInput()->JoystickButtonDown(6) && !GetInput()->PrevJoystickButtonDown(6)))
				PostQuitMessage(0);

			if (GetInput()->KeyDown(DIK_SPACE) ||
				GetInput()->JoystickButtonDown(7))
				m_currState = INIT_LEVEL1;

			if (GetInput()->KeyDown(DIK_R) || 
				GetInput()->JoystickButtonDown(3))
			{
				memset(m_highscores, 0, sizeof(DWORD) * 5);
				SaveHighscores();
			}
			break;
		case INIT_LEVEL1:
			InitLevel1();
			m_currState = START_LEVEL1;
			m_startTimer = 4.0f;
			break;
		case START_LEVEL1:
			m_startTimer -= dt;
			if (m_startTimer <= 1) m_currState = PLAY_LEVEL1;
			break;
		case PLAY_LEVEL1:
			UpdateLevel1(dt);
			break;
		case INIT_LEVEL2:
			InitLevel2();
			m_currState = START_LEVEL2;
			m_startTimer = 4.0f;
			break;
		case START_LEVEL2:
			m_startTimer -= dt;
			if (m_startTimer <= 1) m_currState = PLAY_LEVEL2;
			break;
		case PLAY_LEVEL2:
			UpdateLevel2(dt);
			break;
		case INIT_LEVEL3:
			InitLevel3();
			m_currState = START_LEVEL3;
			m_startTimer = 4.0f;
			break;
		case START_LEVEL3:
			m_startTimer -= dt;
			if (m_startTimer <= 1) m_currState = PLAY_LEVEL3;
			break;
		case PLAY_LEVEL3:
			UpdateLevel3(dt);
			break;
		case INIT_RESULTS:
			{
				m_newHighscore = false;
				m_totalScore = 10000 - (int)m_level1Timer - (int)m_level2Timer - (int)m_level3Timer;
				int i = 0;
				for (i = 0; i < 5; i++)
				{
					if (m_totalScore > m_highscores[i])
					{	
						m_newHighscore = true;
						break;
					}
				}
				if (m_newHighscore)
				{
					for (int j = 4; j > i; j--)
						m_highscores[j] = m_highscores[j-1];
				
					m_highscores[i] = m_totalScore;
					m_rank = i;
					SaveHighscores();
				}
				
				m_currState = DISPLAY_RESULTS;
			}
			break;
		case DISPLAY_RESULTS:
			if ((GetInput()->KeyDown(DIK_ESCAPE)   && !GetInput()->PrevKeyDown(DIK_ESCAPE)) || 
				(GetInput()->JoystickButtonDown(6) && !GetInput()->PrevJoystickButtonDown(6)))
				m_currState = DISPLAY_START;
			break;
	};
	
}

void MAIN::Draw(float dt)
{
	g_device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
	g_device->BeginScene();

	switch(m_currState)
	{
		case DISPLAY_START:
			{
				m_sprite->Begin(D3DXSPRITE_ALPHABLEND);
				RECT rect = {0, 0, 800, 600 };
				m_sprite->Draw(m_startBkgTex, &rect, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0.0), D3DCOLOR_XRGB(255, 255, 255));
				char buffer[100];
				sprintf(buffer, "1. %i\n2. %i\n3. %i\n4. %i\n5. %i", m_highscores[0], m_highscores[1], m_highscores[2], m_highscores[3], m_highscores[4]);
				rect.left = 340;
				rect.top = 270;
				m_font->DrawTextA(m_sprite, buffer, -1, &rect, DT_LEFT, 0xffeeeeee);
				m_sprite->End();
			}
			break;
		case INIT_LEVEL1:
			break;
		case START_LEVEL1:
		case PLAY_LEVEL1:
			DrawLevel1();
			break;
		case INIT_LEVEL2:
			break;
		case START_LEVEL2:
		case PLAY_LEVEL2:
			DrawLevel2();
			break;
		case INIT_LEVEL3:
			break;
		case START_LEVEL3:
		case PLAY_LEVEL3:
			DrawLevel3();
			break;
		case INIT_RESULTS:
			
			break;
		case DISPLAY_RESULTS:
			m_sprite->Begin(D3DXSPRITE_ALPHABLEND);
			RECT rect = {0, 0, 800, 600};
			m_sprite->Draw(m_resultBkgTex, &rect, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0), D3DCOLOR_XRGB(255, 255, 255));
			char buffer[130];
			sprintf(buffer, "Total Score - %i\n\
							Level 1 time - %i seconds\n\
							Level 2 time - %i seconds\n\
							Level 3 time - %i seconds\n", m_totalScore,
							(int)m_level1Timer, (int)m_level2Timer, (int)m_level3Timer);

			m_font->DrawTextA(m_sprite, buffer, -1, &rect, DT_LEFT, 0xffffffff);

			if (m_newHighscore)
			{
				sprintf(buffer, "Congratulations!\nYou set a new highscore\nYou're ranked at #%i on the list", m_rank + 1);
				rect.top = 300;
				m_font->DrawTextA(m_sprite, buffer, -1, &rect, DT_LEFT, 0xffffffff);
			}
			else
			{
				
				rect.top = 300;
				m_font->DrawTextA(m_sprite, "Whats the deal?\nCan't you do any better than that", -1, &rect, DT_LEFT, 0xffffffff);
			}
			rect.top = 500;
			m_font->DrawTextA(m_sprite, "Press Escape to go back to menu", -1, &rect, DT_LEFT, 0xffffffff);
			m_sprite->End();
			break;
	};

	
	g_device->EndScene();
	g_device->Present(0,0,0,0);
}

void MAIN::LoadHighscores()
{
	CXFileLoad xFileLoad;
	xFileLoad.LoadFromFile("highscores.x");
	char *szTemplate = "xof 0302txt 0032 \
						template HIGH_SCORES \
						{\
							<4C3EBA00-217A-447d-B406-B8828A685524>\
							array DWORD highscores[5];\
						}";
	xFileLoad.RegisterTemplates(szTemplate, sizeof(szTemplate));

	SIZE_T count;
	count = xFileLoad.GetChildCount();

	for (int counter = 0; counter < count; counter++)
	{
		CDataObject *object = xFileLoad.GetChild(counter);
		object->GetData();
		if (object->GetType() == HIGHSCORES_GUID)
		{
			struct HIGHSCORES
			{
				DWORD highscores[5];
			}*highscorelist;
			
			SIZE_T size = sizeof(HIGHSCORES);
			object->Lock(&size, (const void**)&highscorelist);
			memcpy(m_highscores, highscorelist, sizeof(HIGHSCORES));
			object->Unlock();
		}
		delete object;
	}
}

void MAIN::SaveHighscores()
{
	CXFileSave xFileSave;
	xFileSave.CreateFile("highscores.x");
	char *szTemplate = "xof 0302txt 0032\
					   template HIGH_SCORES \
						{\
							<4C3EBA00-217A-447d-B406-B8828A685524>\
							array DWORD highscores[5];\
						}";
	xFileSave.RegisterTemplates(szTemplate, strlen(szTemplate));

	struct HIGHSCORES
	{
		DWORD highscores[5];
	}highscorelist;
	for (int i = 0; i < 5; i++)
	{
		highscorelist.highscores[i] = m_highscores[i];
	}
	xFileSave.CreateSaveDataObject(HIGHSCORES_GUID, "HighScores", 0, sizeof(HIGHSCORES), &highscorelist);
	xFileSave.Save();
}

void MAIN::InitLevel1()
{
	m_level1Timer = 0;
	m_collisionCount = 0;
	m_position[0].x = m_position[0].y = 0;
	m_position[1].x = 260;
	m_position[1].y = 0.0f;
	m_position[1].z = 0;
	ZeroMemory(&m_objects, sizeof(Object) * NUM_OBJECTS);
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		m_objects[i].speed = 6.0f;
		m_objects[i].position = D3DXVECTOR3(rand()%400 - 200, rand()%300 - 150,0);
	}
	m_speed = 2.0f;
	m_paused = false;
	m_showBoundingVolume = false;
}

void MAIN::UpdateLevel1(float dt)
{


	if ((GetInput()->KeyDown(DIK_ESCAPE)   && !GetInput()->PrevKeyDown(DIK_ESCAPE)) ||
		(GetInput()->JoystickButtonDown(6) && !GetInput()->PrevJoystickButtonDown(6)))
		m_currState = DISPLAY_START;
	
	if ((GetInput()->KeyDown(DIK_P)        && !GetInput()->PrevKeyDown(DIK_P)) ||
		(GetInput()->JoystickButtonDown(7) && !GetInput()->PrevJoystickButtonDown(7)))
		m_paused = !m_paused;

	if ((GetInput()->KeyDown(DIK_S) && !GetInput()->PrevKeyDown(DIK_S)) ||
		(GetInput()->JoystickButtonDown(2) && !GetInput()->PrevJoystickButtonDown(2)))
		m_showBoundingVolume = !m_showBoundingVolume;

	if (!m_paused)
	{
		//m_angle = 0.0f;
		m_position[0].x += GetInput()->JoystickDX() * 15.0f * dt;
		m_position[0].y -= GetInput()->JoystickDY() * 15.0f * dt;

		m_position[0].x += GetInput()->MouseDX() * 0.25f;
		m_position[0].y -= GetInput()->MouseDY() * 0.25f;
		if (GetInput()->KeyDown(DIK_LEFT))
		{
			m_position[0].x -= 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_RIGHT))
		{
			m_position[0].x += 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_UP))
		{
			m_position[0].y += 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_DOWN))
		{
			m_position[0].y -= 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_R))
		{
			m_angle = 1.0f * dt;
			
		}
		
		
		m_level1Timer += dt;
		if (m_position[0].x < -260) m_position[0].x =  260;
		if (m_position[0].x >  260) m_position[0].x = -260;
		if (m_position[0].y < -210) m_position[0].y =  210;
		if (m_position[0].y >  210) m_position[0].y = -210;

		//FLOATING OBJECT
		for (int i = 0; i < NUM_OBJECTS; i++)
		{
			m_objects[i].position.x += m_objects[i].speed * dt;

			if (m_objects[i].position.x > 200) 
			{
				m_objects[i].position.x = -200;
				m_objects[i].position.y = rand()%300 - 150;
			}
		}
	
		ResetGrid();
		
		// FIND COMPUTER CONTROLLED OBJECT CELLS
		for (int i = 0; i < NUM_OBJECTS; i++)
		{
			int cellX = (m_objects[i].position.x+(float)GRID_COLS/2.0f*CELL_SIZE)/(CELL_SIZE);
			int cellY = (m_objects[i].position.y+(float)GRID_ROWS/2.0f*CELL_SIZE)/(CELL_SIZE);

			m_grid[cellY][cellX].id.push(i);
		}


		// FIND PLAYER CONTROLLED OBJECT CELL
		int playerCellX = (m_position[0].x+(float)GRID_COLS/2.0f*CELL_SIZE)/(CELL_SIZE);
		int playerCellY = (m_position[0].y+(float)GRID_ROWS/2.0f*CELL_SIZE)/(CELL_SIZE);
		
		// UPDATE player AABB position
		m_aabb[0].c = m_position[0];

		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				int cellY = playerCellY + y;
				int cellX = playerCellX + x;

				while(!m_grid[cellY][cellX].id.empty())
				{
					int index = m_grid[cellY][cellX].id.front();

					// UPDATE AABB position
					m_aabb[1].c = m_objects[index].position;
					
					if (TestAABBAABB(m_aabb[0], m_aabb[1]))
					{
						m_collisionCount++;
						m_objects[index].hit = true;

						bool allObjectsHit = true;
						for (int i = 0; i < NUM_OBJECTS; i++)
						{
							if (m_objects[i].hit == false)
							{
								allObjectsHit = false;
								break;
							}
						}

						if (m_collisionCount >= 10 && allObjectsHit)
						{
							
							if (allObjectsHit)
								m_currState = INIT_LEVEL2;
						}
						else
						{
							m_objects[index].speed *= 2;
							m_objects[index].position.x = 260;
						}
					}

					m_grid[cellY][cellX].id.pop();
				}
			}
		}
					
	}
	
}

void MAIN::DrawLevel1()
{
	
	
	D3DXMATRIX I, R;

	//DRAW PLAYER MESH
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, m_position[0].x, m_position[0].y, m_position[0].z);
	g_device->SetTransform(D3DTS_WORLD, &T);
	m_level1Meshs[0]->DrawSubset(0);
	if (m_showBoundingVolume)
	{
		g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_level1Boxes[0]->DrawSubset(0);
		g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}


	//DRAW MESHES
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		D3DXMatrixTranslation(&T, m_objects[i].position.x, m_objects[i].position.y, m_objects[i].position.z);
		g_device->SetTransform(D3DTS_WORLD, &T);
		Vertex *v;
		m_level1Meshs[1]->DrawSubset(0);
		if (m_showBoundingVolume)
		{
			g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			m_level1Boxes[1]->DrawSubset(0);
			g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
	}

	// DRAW GRID LINES
	m_gridLines->Begin();
	for (int y = GRID_ROWS - 1; y >= 0; y--)
	{
		for (int x = 0; x <= GRID_COLS - 1; x++)
		{
			D3DXMatrixTransformation2D(&T, 0,0,0,0,0,&D3DXVECTOR2(x * CELL_SIZE-((float)GRID_COLS/2.0f * CELL_SIZE), y * CELL_SIZE-((float)GRID_ROWS/2.0f * CELL_SIZE)));
			m_gridLines->DrawTransform(m_cellLines, 3, &(T * view * proj), 0xffffffff);
		}
	}
	m_gridLines->End();

	m_sprite->Begin(D3DXSPRITE_ALPHABLEND);
	RECT rect = {0, 0, 800, 600 };

	// DRAW BACKGROUND
	m_sprite->Draw(m_level1BkgTex, &rect, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,1.0), D3DCOLOR_XRGB(255, 255, 255));
	char buffer[80];
	// DISPLAY TIMER AND COLLISION COUNT
	sprintf(buffer, "Timer[%.0f]\nCollision Count[%i]", m_level1Timer, m_collisionCount);
	m_font->DrawTextA(m_sprite, buffer, -1, &rect, DT_LEFT, 0xffffffff); 

	if (m_currState == START_LEVEL1)
	{
		rect.left = 260;
		rect.top = 240;
		
		sprintf(buffer, "Level 1 Begins in %i", (int)m_startTimer);
		m_font->DrawTextA(m_sprite, buffer, -1, &rect, DT_LEFT, 0xffff0000); 
	}

	if (m_paused) m_sprite->Draw(m_pauseTex, &rect, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0.0), D3DCOLOR_XRGB(255, 255, 255));
	m_sprite->End();
	
	/*int cell_x = (m_position[0].x+(float)GRID_COLS/2.0f*CELL_SIZE)/(CELL_SIZE);
	int cell_y = (m_position[0].y+(float)GRID_ROWS/2.0f*CELL_SIZE)/(CELL_SIZE);
	sprintf(buffer, "cell_x = %i, cell_y = %i", cell_x, cell_y);
	rect.left = 300, rect.top = 550;
	m_font->DrawText(NULL, buffer, -1, &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0,255,0));*/

}

void MAIN::InitLevel2()
{
	m_level2Timer = 0;
	m_collisionCount = 0;
	m_position[0].x = m_position[0].y = 0;
	m_position[1].x = 0;
	m_position[1].y = -200.0f;
	m_position[1].z = 0;
	m_speed = 2.0f;
	m_paused = false;
	m_showBoundingVolume = false;
	m_object3D[0].Reset();
	m_object3D[1].Reset();

	ZeroMemory(&m_objects, sizeof(Object) * NUM_OBJECTS);
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		m_objects[i].speed = 6.0f;
		m_objects[i].position = D3DXVECTOR3(rand()%400 - 200, rand()%300 - 150,0);
	}
}

void MAIN::UpdateLevel2(float dt)
{
	if ((GetInput()->KeyDown(DIK_ESCAPE)   && !GetInput()->PrevKeyDown(DIK_ESCAPE)) ||
		(GetInput()->JoystickButtonDown(6) && !GetInput()->PrevJoystickButtonDown(6)))
		m_currState = DISPLAY_START;
	
	if ((GetInput()->KeyDown(DIK_P)        && !GetInput()->PrevKeyDown(DIK_P)) ||
		(GetInput()->JoystickButtonDown(7) && !GetInput()->PrevJoystickButtonDown(7)))
		m_paused = !m_paused;

	if ((GetInput()->KeyDown(DIK_S) && !GetInput()->PrevKeyDown(DIK_S)) ||
		(GetInput()->JoystickButtonDown(2) && !GetInput()->PrevJoystickButtonDown(2)))
		m_showBoundingVolume = !m_showBoundingVolume;

	if (!m_paused)
	{
		m_position[0].x += GetInput()->JoystickDX() * 0.025f;
		m_position[0].y -= GetInput()->JoystickDY() * 0.025f;
		
		m_position[0].x += GetInput()->MouseDX() * 0.25f;
		m_position[0].y -= GetInput()->MouseDY() * 0.25f;
		if (GetInput()->KeyDown(DIK_LEFT))
		{
			m_position[0].x -= 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_RIGHT))
		{
			m_position[0].x += 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_UP))
		{
			m_position[0].y += 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_DOWN))
		{
			m_position[0].y -= 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_R))
		{
			m_angle += 5.0f * dt;
		}

		m_level2Timer += dt;
		if (m_position[0].x < -260) m_position[0].x =  260;
		if (m_position[0].x >  260) m_position[0].x = -260;
		if (m_position[0].y < -210) m_position[0].y =  210;
		if (m_position[0].y >  210) m_position[0].y = -210;

		//FLOATING OBJECT
		for (int i = 0; i <  NUM_OBJECTS; i++)
		{
			m_objects[i].position.y -= m_objects[i].speed * dt;

			if (m_objects[i].position.y < -150)
			{
				m_objects[i].position.y = 150;
				m_objects[i].position.x = rand()%400 - 200;
			}
		}


		ResetGrid();
		
		// FIND COMPUTER CONTROLLED OBJECT CELLS
		for (int i = 0; i < NUM_OBJECTS; i++)
		{
			int cellX = (m_objects[i].position.x+(float)GRID_COLS/2.0f*CELL_SIZE)/(CELL_SIZE);
			int cellY = (m_objects[i].position.y+(float)GRID_ROWS/2.0f*CELL_SIZE)/(CELL_SIZE);

			m_grid[cellY][cellX].id.push(i);
		}


		// FIND PLAYER CONTROLLED OBJECT CELL
		int playerCellX = (m_position[0].x+(float)GRID_COLS/2.0f*CELL_SIZE)/(CELL_SIZE);
		int playerCellY = (m_position[0].y+(float)GRID_ROWS/2.0f*CELL_SIZE)/(CELL_SIZE);
		
		// UPDATE player AABB position
		m_sphere[0].center = m_position[0];

		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				int cellY = playerCellY + y;
				int cellX = playerCellX + x;

				while(!m_grid[cellY][cellX].id.empty())
				{
					int index = m_grid[cellY][cellX].id.front();

					// UPDATE BOUNDING SPHERE position
					m_sphere[1].center = m_objects[index].position;
					if (TestSphereSphere(m_sphere[0], m_sphere[1]))
					{
						m_collisionCount++;
						m_objects[index].hit = true;

						bool allObjectsHit = true;
						for (int i = 0; i < NUM_OBJECTS; i++)
						{
							if (m_objects[i].hit == false)
							{
								allObjectsHit = false;
								break;
							}
						}

						if (m_collisionCount >= 10 && allObjectsHit)
						{
							m_currState = INIT_LEVEL3;
						}
						else
						{
							m_objects[index].speed *= 2;
							m_objects[index].position.y = -150;
						}
					}
					m_grid[cellY][cellX].id.pop();
				}
			}
		}
	}
}

void MAIN::DrawLevel2()
{
	
	
	D3DXMATRIX I, R;

	//DRAW PLAYER MESH
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, m_position[0].x, m_position[0].y, m_position[0].z);
	g_device->SetTransform(D3DTS_WORLD, &T);
	m_level2Meshs[0]->DrawSubset(0);
	if (m_showBoundingVolume)
	{
		g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_level2Spheres[0]->DrawSubset(0);
		g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	//DRAW MESHES
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		D3DXMatrixTranslation(&T, m_objects[i].position.x, m_objects[i].position.y, m_objects[i].position.z);
		g_device->SetTransform(D3DTS_WORLD, &T);
		m_level2Meshs[1]->DrawSubset(0);
		if (m_showBoundingVolume)
		{
			g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			m_level2Spheres[1]->DrawSubset(0);
			g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
	}

	// DRAW GRID LINES
	m_gridLines->Begin();
	for (int y = GRID_ROWS - 1; y >= 0; y--)
	{
		for (int x = 0; x <= GRID_COLS - 1; x++)
		{
			D3DXMatrixTransformation2D(&T, 0,0,0,0,0,&D3DXVECTOR2(x * CELL_SIZE-((float)GRID_COLS/2.0f * CELL_SIZE), y * CELL_SIZE-((float)GRID_ROWS/2.0f * CELL_SIZE)));
			m_gridLines->DrawTransform(m_cellLines, 3, &(T * view * proj), 0xffffffff);
		}
	}
	m_gridLines->End();

	m_sprite->Begin(D3DXSPRITE_ALPHABLEND);
	RECT rect = {0, 0, 800, 600 };

	// DRAW BACKGROUND
	m_sprite->Draw(m_level2BkgTex, &rect, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,1.0), D3DCOLOR_XRGB(255, 255, 255));
	char buffer[80];
	// DISPLAY TIMER AND COLLISION COUNT
	sprintf(buffer, "Timer[%.0f]\nCollision Count[%i]", m_level2Timer, m_collisionCount);
	m_font->DrawTextA(m_sprite, buffer, -1, &rect, DT_LEFT, 0xffffffff); 


	if (m_currState == START_LEVEL2)
	{
		rect.left = 260;
		rect.top = 240;
		
		sprintf(buffer, "Level 2 Begins in %i", (int)m_startTimer);
		m_font->DrawTextA(m_sprite, buffer, -1, &rect, DT_LEFT, 0xffff0000); 
	}

	if (m_paused) m_sprite->Draw(m_pauseTex, &rect, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0.0), D3DCOLOR_XRGB(255, 255, 255));
	m_sprite->End();

}

void MAIN::InitLevel3()
{
	m_level3Timer = 0;
	m_collisionCount = 0;
	m_position[0].x = m_position[0].y = 0;
	m_position[1].x = 0;
	m_position[1].y = -200.0f;
	m_position[1].z = 0;
	m_speed = 2.0f;
	m_paused = false;
	m_showBoundingVolume = false;

	m_object3D[0].Reset();
	m_object3D[1].Reset();

	ZeroMemory(&m_objects, sizeof(Object) * NUM_OBJECTS);
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		m_objects[i].speed = 6.0f;
		m_objects[i].position = D3DXVECTOR3(rand()%400 - 200, rand()%300 - 150,0);
	}
}

void MAIN::UpdateLevel3(float dt)
{
	if ((GetInput()->KeyDown(DIK_ESCAPE)   && !GetInput()->PrevKeyDown(DIK_ESCAPE)) ||
		(GetInput()->JoystickButtonDown(6) && !GetInput()->PrevJoystickButtonDown(6)))
		m_currState = DISPLAY_START;
	
	if ((GetInput()->KeyDown(DIK_P)        && !GetInput()->PrevKeyDown(DIK_P)) ||
		(GetInput()->JoystickButtonDown(7) && !GetInput()->PrevJoystickButtonDown(7)))
		m_paused = !m_paused;

	if ((GetInput()->KeyDown(DIK_S) && !GetInput()->PrevKeyDown(DIK_S)) || 
		(GetInput()->JoystickButtonDown(2) && !GetInput()->PrevJoystickButtonDown(2)))
		m_showBoundingVolume = !m_showBoundingVolume;

	if (!m_paused)
	{
		m_position[0].x += GetInput()->JoystickDX() * 0.025f;
		m_position[0].y -= GetInput()->JoystickDY() * 0.025f;
		
		m_position[0].x += GetInput()->MouseDX() * 0.25f;
		m_position[0].y -= GetInput()->MouseDY() * 0.25f;
		if (GetInput()->KeyDown(DIK_LEFT))
		{
			m_position[0].x -= 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_RIGHT))
		{
			m_position[0].x += 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_UP))
		{
			m_position[0].y += 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_DOWN))
		{
			m_position[0].y -= 400.0f * dt;
			
		}
		if (GetInput()->KeyDown(DIK_R))
		{
			m_angle += 5.0f * dt;
		}

		m_object3D[0].Roll(1.0f * dt);
		m_object3D[1].Roll(-1.0f * dt);

		m_level3Timer += dt;
		if (m_position[0].x < -260) m_position[0].x =  260;
		if (m_position[0].x >  260) m_position[0].x = -260;
		if (m_position[0].y < -210) m_position[0].y =  210;
		if (m_position[0].y >  210) m_position[0].y = -210;


		//FLOATING OBJECT
		for (int i = 0; i < NUM_OBJECTS; i++)
		{
			m_objects[i].position.y -= m_objects[i].speed * dt;

			if (m_objects[i].position.y < -150)
			{
				m_objects[i].position.y = 150;
				m_objects[i].position.x = rand()%400 - 200;
			}
		}

		ResetGrid();
		
		// FIND COMPUTER CONTROLLED OBJECT CELLS
		for (int i = 0; i < NUM_OBJECTS; i++)
		{
			int cellX = (m_objects[i].position.x+(float)GRID_COLS/2.0f*CELL_SIZE)/(CELL_SIZE);
			int cellY = (m_objects[i].position.y+(float)GRID_ROWS/2.0f*CELL_SIZE)/(CELL_SIZE);

			m_grid[cellY][cellX].id.push(i);
		}


		// FIND PLAYER CONTROLLED OBJECT CELL
		int playerCellX = (m_position[0].x+(float)GRID_COLS/2.0f*CELL_SIZE)/(CELL_SIZE);
		int playerCellY = (m_position[0].y+(float)GRID_ROWS/2.0f*CELL_SIZE)/(CELL_SIZE);
		
		// UPDATE player AABB position
		m_obb[0].c = m_position[0];
		m_obb[0].u[0] = m_object3D[0].right;
		m_obb[0].u[1] = m_object3D[0].up;
		m_obb[0].u[2] = m_object3D[0].look;

		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				int cellY = playerCellY + y;
				int cellX = playerCellX + x;

				while(!m_grid[cellY][cellX].id.empty())
				{
					int index = m_grid[cellY][cellX].id.front();

					// UPDATE OBB position and orientation
					m_obb[1].c = m_objects[index].position;
					m_obb[1].u[0] = m_object3D[1].right;
					m_obb[1].u[1] = m_object3D[1].up;
					m_obb[1].u[2] = m_object3D[1].look;

					if (TestOBBOBB(m_obb[0], m_obb[1]))
					{
						m_collisionCount++;
						m_objects[index].hit = true;

						bool allObjectsHit = true;
						for (int i = 0; i < NUM_OBJECTS; i++)
						{
							if (m_objects[i].hit == false)
							{
								allObjectsHit = false;
								break;
							}
						}

						if (m_collisionCount >= 10 && allObjectsHit)
						{
							m_currState = INIT_RESULTS;
						}
						else
						{
							m_objects[index].speed *= 2;
							m_objects[index].position.y = -150;
						}
					}
					m_grid[cellY][cellX].id.pop();
				}
			}
		}
	}
}

void MAIN::DrawLevel3()
{
	
	D3DXMATRIX I, R;

	//DRAW PLAYER MESH
	D3DXMATRIX T;
	m_object3D[0].SetWorldMatrix(m_position[0].x, m_position[0].y, m_position[0].z);
	m_level1Meshs[0]->DrawSubset(0);
	if (m_showBoundingVolume)
	{
		g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		m_level1Boxes[0]->DrawSubset(0);
		g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	//DRAW MESHES
	for (int i = 0; i < 5; i++)
	{
		m_object3D[1].SetWorldMatrix(m_objects[i].position.x, m_objects[i].position.y, m_objects[i].position.z);
		m_level1Meshs[1]->DrawSubset(0);
		if (m_showBoundingVolume)
		{
			g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
			m_level1Boxes[1]->DrawSubset(0);
			g_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		}
	}

	
	// DRAW GRID LINES
	m_gridLines->Begin();
	for (int y = GRID_ROWS - 1; y >= 0; y--)
	{
		for (int x = 0; x <= GRID_COLS - 1; x++)
		{
			D3DXMatrixTransformation2D(&T, 0,0,0,0,0,&D3DXVECTOR2(x * CELL_SIZE-((float)GRID_COLS/2.0f * CELL_SIZE), y * CELL_SIZE-((float)GRID_ROWS/2.0f * CELL_SIZE)));
			m_gridLines->DrawTransform(m_cellLines, 3, &(T * view * proj), 0xffffffff);
		}
	}
	m_gridLines->End();

	m_sprite->Begin(D3DXSPRITE_ALPHABLEND);
	RECT rect = {0, 0, 800, 600 };

	// DRAW BACKGROUND
	m_sprite->Draw(m_level3BkgTex, &rect, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,1.0), D3DCOLOR_XRGB(255, 255, 255));
	char buffer[80];
	// DISPLAY TIMER AND COLLISION COUNT
	sprintf(buffer, "Timer[%.0f]\nCollision Count[%i]", m_level3Timer, m_collisionCount);
	m_font->DrawTextA(m_sprite, buffer, -1, &rect, DT_NOCLIP, 0xffffffff); 

	if (m_currState == START_LEVEL3)
	{
		rect.left = 260;
		rect.top = 240;
		
		sprintf(buffer, "Level 3 Begins in %i", (int)m_startTimer);
		m_font->DrawTextA(m_sprite, buffer, -1, &rect, DT_NOCLIP | DT_LEFT, 0xffff0000); 
	}


	if (m_paused) m_sprite->Draw(m_pauseTex, &rect, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,0.0), D3DCOLOR_XRGB(255, 255, 255));
	m_sprite->End();
}

void MAIN::ResetGrid()
{
	for (int y = 0; y < GRID_ROWS; y++)
		for (int x = 0;  x < GRID_COLS; x++)
		{
			while(!m_grid[y][x].id.empty())
			{
				m_grid[y][x].id.pop();
			}
		}
}