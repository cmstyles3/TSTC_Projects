#include "CMesh.h"

extern LPDIRECT3DDEVICE9 Device;

CMesh::CMesh()
{
	m_pTextures = 0;
	m_pMaterials = 0;
	m_pMesh = 0;
}

CMesh::~CMesh()
{
	Kill();
}

int CMesh::LoadXFile(const char file[], const char modelPath[], const char texturePath[])
{
	LPD3DXBUFFER pMaterialBuffer;
	fout.open("report.txt", ios_base::out | ios_base::app);
	char path[256];
	sprintf(path, "%s\\%s", modelPath, file);
	if (!m_pMesh && !m_pMaterials && !m_pTextures)
	{
	
		if (FAILED(D3DXLoadMeshFromX(path, D3DXMESH_MANAGED, Device, 0, &pMaterialBuffer,  0, &m_NumMaterials, &m_pMesh)))
		{
			fout << "D3DXLoadMeshFromX - FAILED: " << path << endl;
			return(0);
		}

		D3DXMATERIAL* pMaterials = (D3DXMATERIAL*)pMaterialBuffer->GetBufferPointer();

		m_pMaterials = new D3DMATERIAL9[m_NumMaterials];
		m_pTextures  = new LPDIRECT3DTEXTURE9[m_NumMaterials];

		for (int i = 0; i < m_NumMaterials; i++)
		{
			m_pMaterials[i] = pMaterials[i].MatD3D;
			
			m_pMaterials[i].Ambient = m_pMaterials[i].Diffuse;

			if (!pMaterials[i].pTextureFilename)
			{
				m_pTextures[i] = 0;
				continue;
			}

			sprintf(path, "%s\\%s", texturePath, pMaterials[i].pTextureFilename);
			if (FAILED(D3DXCreateTextureFromFile(Device, path, &m_pTextures[i])))
			{
				//MessageBox(0,"D3DXCreateTextureFromFile - FAILED: ", 0, 0);
				m_pTextures[i] = 0;
			}
		}
		fout.close();
		pMaterialBuffer->Release();

		// Create sphere for testing collision
		this->Center();
		if (!m_pBoundingSphere) D3DXCreateSphere(Device, this->GetRadius(), 10, 10, &m_pBoundingSphere, 0);
	}
	
	return(1);
}


void CMesh::Render(bool bBoundingSphere)
{
	for (int i = 0; i < m_NumMaterials; i++)
	{
		Device->SetMaterial(&m_pMaterials[i]);
		Device->SetTexture(0, m_pTextures[i]);
		m_pMesh->DrawSubset(i);
	}
	Device->SetFVF(D3DFVF_CUSTOM);
	
	if (m_pBoundingSphere && bBoundingSphere)
	{
		Device->SetRenderState(D3DRS_LIGHTING, false);
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		Device->SetMaterial(&WHITE_MTRL);
		Device->SetTexture(0,0);
		m_pBoundingSphere->DrawSubset(0);
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		Device->SetRenderState(D3DRS_LIGHTING, true);
	}
}

//Destroys all materials, textures, and mesh object associated with the mesh.
void CMesh::Kill()
{
	// deallocates array used for the material list
	KILL_ARRAY(m_pMaterials);
	
	if (m_pTextures)
	{
		// deallocates all the loaded textures
		for (int i = 0; i < m_NumMaterials; i++)
			KILL_COM_OBJECT(m_pTextures[i]);

		// deallocates array used for the texture list
		KILL_ARRAY(m_pTextures);
	}

	//deallocate mesh object
	KILL_COM_OBJECT(m_pMesh);
	
}

//Uniformly scales x, y, z of mesh by the specified by parameter s
void CMesh::Scale(float s)
{
	Vertex *v;
	if (m_pMesh)
	{
		m_pMesh->LockVertexBuffer(0, (void**)&v);
		for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
		{
			v[i].m_Pos.x *= s;
			v[i].m_Pos.y *= s;
			v[i].m_Pos.z *= s;
		}
		m_pMesh->UnlockVertexBuffer();
	}
}

//Centers mesh object around its origin (0, 0, 0)
void CMesh::Center()
{
	fout.open("report.txt", ios_base::out | ios_base::app);
	Vertex *v;
	float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
	m_pMesh->LockVertexBuffer(0, (void**)&v);
	
	// Set min and max to the first vertex for x, y, z
	minX = maxX = v[0].m_Pos.x;
	minY = maxY = v[0].m_Pos.y;
	minZ = maxZ = v[0].m_Pos.z;

	// Go through mesh object and get minimum and maximum values for x, y, z
	for (int i = 1; i < m_pMesh->GetNumVertices(); i++)
	{
		if (v[i].m_Pos.x < minX) minX = v[i].m_Pos.x;
		else if (v[i].m_Pos.x > maxX) maxX = v[i].m_Pos.x;

		if (v[i].m_Pos.y < minY) minY = v[i].m_Pos.y;
		else if (v[i].m_Pos.y > maxY) maxY = v[i].m_Pos.y;

		if (v[i].m_Pos.z < minZ) minZ = v[i].m_Pos.z;
		else if (v[i].m_Pos.z > maxZ) maxZ = v[i].m_Pos.z;
	}

	float  midX = (minX + maxX) / 2;
	float  midY = (minY + maxY) / 2;
	float  midZ = (minZ + maxZ) / 2;
	fout << "minX = " << minX << " minY = " << minY << " minZ = " << minZ << " "<< endl;
	fout << "maxX = " << maxX << " maxY = " << maxY << " maxZ = " << maxZ << " "<< endl;
	fout << "midX = " << midX << " midY = " << midY << " midZ = " << midZ << " "<< endl;
	
	for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		v[i].m_Pos.x = v[i].m_Pos.x - midX;
		v[i].m_Pos.y = v[i].m_Pos.y - midY;
		v[i].m_Pos.z = v[i].m_Pos.z - midZ;
	}
	
	m_pMesh->UnlockVertexBuffer();
	fout.close();
}

//Centers mesh object around specified point (x, y, z)
void CMesh::Center(float x, float y, float z)
{
	fout.open("report.txt", ios_base::out | ios_base::app);
	Vertex *v;
	float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
	m_pMesh->LockVertexBuffer(0, (void**)&v);
	
	// Set min and max to the first vertex for x, y, z
	minX = maxX = v[0].m_Pos.x;
	minY = maxY = v[0].m_Pos.y;
	minZ = maxZ = v[0].m_Pos.z;

	// Go through mesh object and get minimum and maximum values for x, y, z
	for (int i = 1; i < m_pMesh->GetNumVertices(); i++)
	{
		if (v[i].m_Pos.x < minX) minX = v[i].m_Pos.x;
		else if (v[i].m_Pos.x > maxX) maxX = v[i].m_Pos.x;

		if (v[i].m_Pos.y < minY) minY = v[i].m_Pos.y;
		else if (v[i].m_Pos.y > maxY) maxY = v[i].m_Pos.y;

		if (v[i].m_Pos.z < minZ) minZ = v[i].m_Pos.z;
		else if (v[i].m_Pos.z > maxZ) maxZ = v[i].m_Pos.z;
	}

	float  midX = (minX + maxX) / 2;
	float  midY = (minY + maxY) / 2;
	float  midZ = (minZ + maxZ) / 2;
	fout << "minX = " << minX << " minY = " << minY << " minZ = " << minZ << " "<< endl;
	fout << "maxX = " << maxX << " maxY = " << maxY << " maxZ = " << maxZ << " "<< endl;
	fout << "midX = " << midX << " midY = " << midY << " midZ = " << midZ << " "<< endl;
	
	for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		v[i].m_Pos.x = v[i].m_Pos.x - (midX - x);
		v[i].m_Pos.y = v[i].m_Pos.y - (midY - y);
		v[i].m_Pos.z = v[i].m_Pos.z - (midZ - z);
	}
	
	m_pMesh->UnlockVertexBuffer();
	fout.close();
}

float CMesh::GetRadius()
{
	float radius = 0.0f;
	Vertex *v;
	m_pMesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		float dist = sqrt(
			v[i].m_Pos.x * v[i].m_Pos.x + 
			v[i].m_Pos.y * v[i].m_Pos.y + 
			v[i].m_Pos.z * v[i].m_Pos.z);
		if (dist > radius)
			radius = dist;
	}
	m_pMesh->UnlockVertexBuffer();
	return radius;
}

float CMesh::GetHeight()
{
	float height = 0;
	Vertex *v;
	m_pMesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		if (v[i].m_Pos.y > height)
			height = v[i].m_Pos.y;
	}
	m_pMesh->UnlockVertexBuffer();
	return height;
}

float CMesh::GetOffset()
{
	float offset = 0;
	Vertex *v;
	m_pMesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		if (v[i].m_Pos.z < offset)
			offset = v[i].m_Pos.z;
	}
	m_pMesh->UnlockVertexBuffer();
	return offset;
}
