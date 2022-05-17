//			//LOAD LAVA PLANET TERRAIN
	//g_Terrain[LAVA].Init(Device, 64, 65, 200, HEIGHT_SCALE);
	//g_Terrain[LAVA].LoadHeightmap("Heightmap\\LavaPlanet_heightmap.raw", 513);
	//
	//g_Terrain[LAVA].LoadTextures(
	//	"Textures\\game\\lava\\Rock04.bmp",
	//	"Textures\\game\\lava\\Rock05_Test.bmp",
	//	"Textures\\game\\lava\\Rock02.bmp",
	//	"Textures\\game\\lava\\lava02.bmp", 
	//	D3DCOLOR_XRGB(255,0,0));

	//for (int z = 0; z < 8; z++)
	//{
	//	for (int x = 0; x < 8; x++)
	//	{
	//		int index = (z * 8) + x;
	//		g_Terrain[LAVA].SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
	//		char path[256];
	//		sprintf(path, "Textures\\Terrain\\lava\\Patch%i\.bmp", index); 
	//		g_Terrain[LAVA].GenerateTexture(path, index, 512, 512);		
	//	}
	//}

	////LOAD ICE PLANET TERRAIN
	//g_Terrain[SNOW].Init(Device, 64, 65, 200, HEIGHT_SCALE);
	//g_Terrain[SNOW].LoadHeightmap("Heightmap\\IcePlanetA_heightmap.raw", 513);
	//
	//g_Terrain[SNOW].LoadTextures(
	//	"Textures\\game\\ice\\snow.bmp",
	//	"Textures\\game\\ice\\Ice3_TileImprove_512.bmp",
	//	"Textures\\game\\ice\\Ice2_TileImprove_512.bmp",
	//	"Textures\\game\\ice\\Ice4_TileImprove_512.bmp", 
	//	D3DCOLOR_XRGB(180,180,255));

	//for (int z = 0; z < 8; z++)
	//{
	//	for (int x = 0; x < 8; x++)
	//	{
	//		int index = (z * 8) + x;
	//		g_Terrain[SNOW].SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
	//		char path[256];
	//		sprintf(path, "Textures\\Terrain\\ice\\Patch%i\.bmp", index); 
	//		g_Terrain[SNOW].GenerateTexture(path, index, 512, 512);		
	//	}
	//}

////LOAD LAVA PLANET TERRAIN
	//g_Terrain[LAVA].Init(Device, 64, 65, 200, HEIGHT_SCALE);
	//g_Terrain[LAVA].LoadHeightmap("Heightmap\\LavaPlanet_heightmap.raw", 513);
	//
	//g_Terrain[LAVA].LoadTextures(
	//	"Textures\\game\\lava\\Rock04.bmp",
	//	"Textures\\game\\lava\\Rock05_Test.bmp",
	//	"Textures\\game\\lava\\Rock02.bmp",
	//	"Textures\\game\\lava\\lava02.bmp", 
	//	D3DCOLOR_XRGB(255,0,0));

	//for (int z = 0; z < 8; z++)
	//{
	//	for (int x = 0; x < 8; x++)
	//	{
	//		int index = (z * 8) + x;
	//		g_Terrain[LAVA].SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
	//		char path[256];
	//		sprintf(path, "Textures\\Terrain\\lava\\Patch%i\.bmp", index); 
	//		g_Terrain[LAVA].GenerateTexture(path, index, 512, 512);		
	//	}
	//}

	////LOAD ICE PLANET TERRAIN
	//g_Terrain[SNOW].Init(Device, 64, 65, 200, HEIGHT_SCALE);
	//g_Terrain[SNOW].LoadHeightmap("Heightmap\\IcePlanetA_heightmap.raw", 513);
	//
	//g_Terrain[SNOW].LoadTextures(
	//	"Textures\\game\\ice\\snow.bmp",
	//	"Textures\\game\\ice\\Ice3_TileImprove_512.bmp",
	//	"Textures\\game\\ice\\Ice2_TileImprove_512.bmp",
	//	"Textures\\game\\ice\\Ice4_TileImprove_512.bmp", 
	//	D3DCOLOR_XRGB(180,180,255));

	//for (int z = 0; z < 8; z++)
	//{
	//	for (int x = 0; x < 8; x++)
	//	{
	//		int index = (z * 8) + x;
	//		g_Terrain[SNOW].SetHeightmap(index, (65 * x) - x, (65 * z) - z, 513);
	//		char path[256];
	//		sprintf(path, "Textures\\Terrain\\ice\\Patch%i\.bmp", index); 
	//		g_Terrain[SNOW].GenerateTexture(path, index, 512, 512);		
	//	}
	//}

if (g_Input.IsKeyDown(DIK_C) && !g_Input.WasKeyDown(DIK_C))
				{
					if (cameraConn)
						cameraConn = false;
					else cameraConn = true;
				}


void InitWaterMesh()
{
	D3DXCreateTextureFromFile(Device, "Textures\\water.dds", &g_planetTex[EARTHY]);
	D3DXCreateTextureFromFile(Device, "Textures\\game\\lava\\lava01.dds", &g_planetTex[LAVA]);
	D3DXCreateTextureFromFile(Device, "Textures\\game\\lava\\lava02.dds", &g_planetTex[SNOW]);
	D3DXCreateTextureFromFile(Device, "Textures\\earthy.bmp", &g_terrainTex[EARTHY]);
	D3DXCreateTextureFromFile(Device, "Textures\\lava_rock.bmp", &g_terrainTex[LAVA]);
	D3DXCreateTextureFromFile(Device, "Textures\\snow.bmp", &g_terrainTex[SNOW]);
	D3DXCreateMeshFVF(2, 4, D3DXMESH_DYNAMIC, D3DFVF_XYZTEX1, Device, &Water);
	VertexTex1 *v;
	if ( FAILED(Water->LockVertexBuffer(0, (void**)&v)))
		PostQuitMessage(0);
	v[0] = VertexTex1(0.0f, HEIGHT_SCALE * 63.5, 0.0f,  0.0f, 0.0f);
	v[1] = VertexTex1(0.0f, HEIGHT_SCALE * 63.5, 80000.0f,  50.0f, 0.0f);
	v[2] = VertexTex1(80000.0f, HEIGHT_SCALE * 63.5, 80000.0f, 50.0f, 50.0f);
	v[3] = VertexTex1(80000.0f, HEIGHT_SCALE * 63.5, 0.0f,  0.0f, 50.0f);
	Water->UnlockVertexBuffer();

	WORD *i;
	Water->LockIndexBuffer(0, (void**)&i);
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;
	Water->UnlockIndexBuffer();

	D3DXCreateMeshFVF(2, 4, D3DXMESH_DYNAMIC, D3DFVF_XYZTEX1, Device, &Water2);
	if ( FAILED(Water2->LockVertexBuffer(0, (void**)&v)))
		PostQuitMessage(0);
	v[0] = VertexTex1(0.0f, HEIGHT_SCALE * 63.5, 0.0f,  0.0f, 0.0f);
	v[1] = VertexTex1(0.0f, HEIGHT_SCALE * 63.5, 80000.0f,  50.0f, 0.0f);
	v[2] = VertexTex1(80000.0f, HEIGHT_SCALE * 63.5, 80000.0f, 50.0f, 50.0f);
	v[3] = VertexTex1(80000.0f, HEIGHT_SCALE * 63.5, 0.0f,  0.0f, 50.0f);
	Water2->UnlockVertexBuffer();

	
	Water2->LockIndexBuffer(0, (void**)&i);
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;
	Water2->UnlockIndexBuffer();

	D3DXCreateMeshFVF(2, 4, D3DXMESH_DYNAMIC, D3DFVF_XYZTEX1, Device, &Water3);
	if ( FAILED(Water3->LockVertexBuffer(0, (void**)&v)))
		PostQuitMessage(0);
	v[0] = VertexTex1(0.0f, HEIGHT_SCALE * 63.5, 0.0f,  0.0f, 0.0f);
	v[1] = VertexTex1(0.0f, HEIGHT_SCALE * 63.5, 80000.0f,  50.0f, 0.0f);
	v[2] = VertexTex1(80000.0f, HEIGHT_SCALE * 63.5, 80000.0f, 50.0f, 50.0f);
	v[3] = VertexTex1(80000.0f, HEIGHT_SCALE * 63.5, 0.0f,  0.0f, 50.0f);
	Water3->UnlockVertexBuffer();

	
	Water3->LockIndexBuffer(0, (void**)&i);
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;
	Water3->UnlockIndexBuffer();

	//D3DXCreateMeshFVF(2, 4, 0, D3DFVF_CUSTOM, Device, &Water2);
	////Vertex *v;
	//if ( FAILED(Water2->LockVertexBuffer(0, (void**)&v)))
	//	PostQuitMessage(0);
	//v[0] = Vertex(0.0f, 40.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	//v[1] = Vertex(0.0f, 40.5f, 100000.0f, 0.0f, 1.0f, 0.0f, 250.0f, 0.0f);
	//v[2] = Vertex(100000.0f, 40.5f, 100000.0f, 0.0f, 1.0f, 0.0f, 250.0f, 250.0f);
	//v[3] = Vertex(100000.0f, 40.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 250.0f);
	//Water2->UnlockVertexBuffer();

	////WORD *i;
	//Water2->LockIndexBuffer(0, (void**)&i);
	//i[0] = 0; i[1] = 1; i[2] = 2;
	//i[3] = 0; i[4] = 2; i[5] = 3;
	//Water2->UnlockIndexBuffer();

	//D3DXCreateMeshFVF(2, 4, D3DXMESH_DYNAMIC, D3DFVF_CUSTOM, Device, &Water3);
	////Vertex *v;
	//if ( FAILED(Water3->LockVertexBuffer(0, (void**)&v)))
	//	PostQuitMessage(0);
	//v[0] = Vertex(0.0f, 41.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	//v[1] = Vertex(0.0f, 41.0f, 100000.0f, 0.0f, 1.0f, 0.0f, 250.0f, 0.0f);
	//v[2] = Vertex(100000.0f, 41.0f, 100000.0f, 0.0f, 1.0f, 0.0f, 250.0f, 250.0f);
	//v[3] = Vertex(100000.0f, 41.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 250.0f);
	//Water3->UnlockVertexBuffer();

	////WORD *i;
	//Water3->LockIndexBuffer(0, (void**)&i);
	//i[0] = 0; i[1] = 1; i[2] = 2;
	//i[3] = 0; i[4] = 2; i[5] = 3;
	//Water3->UnlockIndexBuffer();

	D3DXCreateMeshFVF(2, 4, 0, D3DFVF_XYZTEX1, Device, &Water4);
	//VertexTex1 *v;
	if ( FAILED(Water4->LockVertexBuffer(0, (void**)&v)))
		PostQuitMessage(0);
	v[0] = VertexTex1(0.0f, HEIGHT_SCALE * 63.5, 0.0f, 0.0f, 0.0f);
	v[1] = VertexTex1(0.0f, HEIGHT_SCALE * 63.5, 80000.0f, 50.0f, 0.0f);
	v[2] = VertexTex1(80000.0f, HEIGHT_SCALE * 63.5, 80000.0f, 50.0f, 50.0f);
	v[3] = VertexTex1(80000.0f, HEIGHT_SCALE * 63.5, 0.0f, 0.0f, 50.0f);
	Water4->UnlockVertexBuffer();

	//WORD *i;
	Water4->LockIndexBuffer(0, (void**)&i);
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;
	Water4->UnlockIndexBuffer();
}

void DrawWaterMesh(LPDIRECT3DTEXTURE9 tex)
{
	if (g_planet!=SNOW)
	{
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		Device->SetRenderState(D3DRS_LIGHTING, false);
		
		float x, y, z;
		g_Camera.GetPosition(x, y, z);
		D3DXMATRIX T;
		D3DXMatrixTranslation(&T, x - 40000.0f, 0.0f, z - 40000.0f);
		Device->SetTransform(D3DTS_WORLD, &T);
		//Device->SetTexture(0, g_planetTex[LAVA]);
		//Water->DrawSubset(0);
		if (g_planet == LAVA)
		{
			Device->SetTexture(0, g_planetTex[SNOW]);
			Water2->DrawSubset(0);
		}
		else
		{
			Device->SetTexture(0, g_planetTex[g_planet]);
			Water2->DrawSubset(0);
		}
		
		

		
		Device->SetTexture(0, g_planetTex[g_planet]);
		//Water3->DrawSubset(0);
		Water4->DrawSubset(0);
		Device->SetRenderState(D3DRS_ZENABLE, true);
		Device->SetRenderState(D3DRS_LIGHTING, true);
	}
	
}

void AnimateWaterMesh()
{
	VertexTex1 *v;

	/*Water->LockVertexBuffer(0, (void**)&v);
	v[0].m_U += 0.0002f;
	v[1].m_U += 0.0002f;
	v[2].m_U += 0.0002f;
	v[3].m_U += 0.0002f;
	Water->UnlockVertexBuffer();

	Water2->LockVertexBuffer(0, (void**)&v);
	v[0].m_U -= 0.0002f;
	v[1].m_U -= 0.0002f;
	v[2].m_U -= 0.0002f;
	v[3].m_U -= 0.0002f;
	Water2->UnlockVertexBuffer();

	Water3->LockVertexBuffer(0, (void**)&v);
	v[0].m_V -= 0.0002f;
	v[1].m_V -= 0.0002f;
	v[2].m_V -= 0.0002f;
	v[3].m_V -= 0.0002f;*/
	//Water3->UnlockVertexBuffer();

	float x, y, z;
	g_Camera.GetPosition(x, y, z);
	Water->LockVertexBuffer(0, (void**)&v);
	v[0].u += (z - oldz)/1600.0f - 0.0005; //(how big quad is)8000.0f/(max texture coord)50.0f
	v[1].u += (z - oldz)/1600.0f - 0.0005;
	v[2].u += (z - oldz)/1600.0f - 0.0005;
	v[3].u += (z - oldz)/1600.0f - 0.0005;
	v[0].v += (x - oldx)/1600.0f + 0.0005;
	v[1].v += (x - oldx)/1600.0f + 0.0005;
	v[2].v += (x - oldx)/1600.0f + 0.0005;
	v[3].v += (x - oldx)/1600.0f + 0.0005;
	Water->UnlockVertexBuffer();

	Water2->LockVertexBuffer(0, (void**)&v);
	v[0].u += (z - oldz)/1600.0f - 0.0005;
	v[1].u += (z - oldz)/1600.0f - 0.0005;
	v[2].u += (z - oldz)/1600.0f - 0.0005;
	v[3].u += (z - oldz)/1600.0f - 0.0005;
	v[0].v += (x - oldx)/1600.0f - 0.0005; //(how big quad is)8000.0f/(max texture coord)50.0f;
	v[1].v += (x - oldx)/1600.0f - 0.0005; //(how big quad is)8000.0f/(max texture coord)50.0f;
	v[2].v += (x - oldx)/1600.0f - 0.0005; //(how big quad is)8000.0f/(max texture coord)50.0f;
	v[3].v += (x - oldx)/1600.0f - 0.0005; //(how big quad is)8000.0f/(max texture coord)50.0f;
	Water2->UnlockVertexBuffer();

	Water4->LockVertexBuffer(0, (void**)&v);
	v[0].u += (z - oldz)/1600.0f + 0.0005; //(how big quad is)8000.0f/(max texture coord)50.0f
	v[1].u += (z - oldz)/1600.0f + 0.0005;
	v[2].u += (z - oldz)/1600.0f + 0.0005;
	v[3].u += (z - oldz)/1600.0f + 0.0005;
	v[0].v += (x - oldx)/1600.0f - 0.0005;
	v[1].v += (x - oldx)/1600.0f - 0.0005;
	v[2].v += (x - oldx)/1600.0f - 0.0005;
	v[3].v += (x - oldx)/1600.0f - 0.0005;
	Water4->UnlockVertexBuffer();
}

void DestroyWaterMesh()
{
	for (int i = 0; i < 3; i++)
	{
		SAFE_RELEASE(g_planetTex[i]);
		SAFE_RELEASE(g_terrainTex[i]);
	}
	SAFE_RELEASE(Water);
	SAFE_RELEASE(Water2);
	SAFE_RELEASE(Water3);
	SAFE_RELEASE(Water4);
	SAFE_RELEASE(waterTex);
}


//begin camera -------------------------
#include "CCamera.h"



CCamera::CCamera()
{
	m_CameraType = AIRCRAFT;

	m_Position  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Right     = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_Up        = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_Look      = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

CCamera::CCamera(CAMERA_TYPE cameraType)
{
}

CCamera::~CCamera()
{
}

void CCamera::Walk(float units)
{
	switch(m_CameraType)
	{
		case LANDOBJECT:
			m_Position += D3DXVECTOR3(m_Look.x, 0.0f, m_Look.z) * units;
			break;
		case AIRCRAFT:
			m_Position += m_Look * units;
			break;
	}
}

void CCamera::Strafe(float units)
{
	switch(m_CameraType)
	{
		case LANDOBJECT:
			m_Position += D3DXVECTOR3(m_Right.x, 0.0f, m_Right.z) * units;
			break;
		case AIRCRAFT:
			m_Position += m_Right * units;
			break;
	}
}

void CCamera::Fly(float units)
{
	switch(m_CameraType)
	{
		case LANDOBJECT:
			m_Position.y += units;
			break;
		case AIRCRAFT:
			m_Position += m_Up * units;
			break;
	}
}

void CCamera::Pitch(float angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_Right, angle);

	D3DXVec3TransformCoord(&m_Up, &m_Up, &T);
	D3DXVec3TransformCoord(&m_Look, &m_Look, &T);

}

void CCamera::Yaw(float angle)
{
	D3DXMATRIX T;

	if (m_CameraType == LANDOBJECT)
		D3DXMatrixRotationY(&T, angle);

	if (m_CameraType == AIRCRAFT)
		D3DXMatrixRotationAxis(&T, &m_Up, angle);

	D3DXVec3TransformCoord(&m_Right, &m_Right, &T);
	D3DXVec3TransformCoord(&m_Look, &m_Look, &T);
}

void CCamera::Roll(float angle)
{
	D3DXMATRIX T;

	if (m_CameraType == AIRCRAFT)
	{
		D3DXMatrixRotationAxis(&T, &m_Look, angle);

		D3DXVec3TransformCoord(&m_Right, &m_Right, &T);
		D3DXVec3TransformCoord(&m_Up, &m_Up, &T);
	}

}

void CCamera::GetViewMatrix(float offset, float height, D3DXMATRIX* V)
{
	//m_Position += vel;

	D3DXVec3Normalize(&m_Look, &m_Look);
 
	D3DXVECTOR3 pos((m_Position.x - offset * m_Look.x) + m_Up.x * height, (m_Position.y - offset * m_Look.y) + m_Up.y * height, (m_Position.z - offset * m_Look.z) + m_Up.z * height);

	D3DXVec3Cross(&m_Up, &m_Look, &m_Right);
	D3DXVec3Normalize(&m_Up, &m_Up);

	D3DXVec3Cross(&m_Right, &m_Up, &m_Look);
	D3DXVec3Normalize(&m_Right, &m_Right);

	float x1 = -D3DXVec3Dot(&m_Right, &pos);
	float y1 = -D3DXVec3Dot(&m_Up, &pos);
	float z1 = -D3DXVec3Dot(&m_Look, &pos);

	(*V)(0, 0) = m_Right.x;	(*V)(0, 1) = m_Up.x;  (*V)(0, 2) = m_Look.x;  (*V)(0, 3) = 0.0f;

	(*V)(1, 0) = m_Right.y;  (*V)(1, 1) = m_Up.y;  (*V)(1, 2) = m_Look.y;  (*V)(1, 3) = 0.0f;

	(*V)(2, 0) = m_Right.z; 	(*V)(2, 1) = m_Up.z;  (*V)(2, 2) = m_Look.z;  (*V)(2, 3) = 0.0f;

	(*V)(3, 0) = x1;        (*V)(3, 1) = y1;	 (*V)(3, 2) = z1; 	    (*V)(3, 3) = 1.0f;
	
	//Device->SetTransform(D3DTS_VIEW, &V);

}

void CCamera::GetPosition(float &x, float &y, float &z)
{
	x = m_Position.x;
	y = m_Position.y;
	z = m_Position.z;
}

void CCamera::SetPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void CCamera::GetLook(D3DXVECTOR3 &look)
{
	look = m_Look;
}

void CCamera::GetUp(D3DXVECTOR3 &up)
{
	up = m_Up;
}

void CCamera::GetRight(D3DXVECTOR3 &right)
{
	right = m_Right;
}

void CCamera::SetLook(D3DXVECTOR3 look)
{
	m_Look = look;
}

void CCamera::SetRight(D3DXVECTOR3 right)
{
	m_Right = right;
}

void CCamera::SetUp(D3DXVECTOR3 up)
{
	m_Up = up;
}
// end camera