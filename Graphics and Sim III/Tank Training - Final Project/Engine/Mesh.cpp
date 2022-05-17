#include "Engine.h"


#define D3DFVF_CUSTOM (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

#define KILL_COM_OBJECT(x) if (x) { x->Release(); x = 0; }
#define KILL(x) if (x) { delete x; x = 0; }
#define KILL_ARRAY(x) if (x) { delete [] x; x = 0; }

Mesh::Mesh()
{
	m_pTextures = 0;
	m_pMaterials = 0;
	m_pMesh = 0;
}

Mesh::~Mesh()
{
	Kill();
}

int Mesh::LoadXFile(char *filename, char *texPath)
{
	LPD3DXBUFFER pMaterialBuffer;
	

	if (!m_pMesh && !m_pMaterials && !m_pTextures)
	{
	
		if (FAILED(D3DXLoadMeshFromX(filename, D3DXMESH_MANAGED, g_engine->GetDevice(), 0, &pMaterialBuffer,  0, &m_NumMaterials, &m_pMesh)))
		{
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

			char path[100];
			sprintf(path, "%s%s", texPath, pMaterials[i].pTextureFilename);
			if (FAILED(D3DXCreateTextureFromFile(g_engine->GetDevice(), path, &m_pTextures[i])))
			{
				MessageBox(0,pMaterials[i].pTextureFilename, 0, 0);
				m_pTextures[i] = 0;
			}
		}
		
		pMaterialBuffer->Release();

		// Create sphere for testing collision
		/*this->Center();
		if (!m_pBoundingSphere) D3DXCreateSphere(g_engine->GetDevice(), this->GetRadius(), 10, 10, &m_pBoundingSphere, 0);

		LPD3DXMESH mesh = m_pMesh;
		DWORD* adjacencyInfo = new DWORD[mesh->GetNumFaces()*3];
		mesh->GenerateAdjacency(0.0f, adjacencyInfo);
		mesh->Optimize(D3DXMESH_MANAGED | D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, adjacencyInfo, 0, 0, 0, &m_pMesh);*/
		//delete adjacencyInfo;
	}
	
	return(1);
}


void Mesh::Render(bool bBoundingSphere)
{
	for (int i = 0; i < m_NumMaterials; i++)
	{
		g_engine->GetDevice()->SetMaterial(&m_pMaterials[i]);
		g_engine->GetDevice()->SetTexture(0, m_pTextures[i]);
		m_pMesh->DrawSubset(i);
	}
	g_engine->GetDevice()->SetFVF(D3DFVF_CUSTOM);
	
	if (m_pBoundingSphere && bBoundingSphere)
	{
		g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
		g_engine->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		//g_engine->GetDevice()->SetMaterial(&WHITE_MTRL);
		g_engine->GetDevice()->SetTexture(0,0);
		m_pBoundingSphere->DrawSubset(0);
		g_engine->GetDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, true);
	}
}

//Destroys all materials, textures, and mesh object associated with the mesh.
void Mesh::Kill()
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
void Mesh::Scale(float s)
{
	VertexPNT *v;
	if (m_pMesh)
	{
		m_pMesh->LockVertexBuffer(0, (void**)&v);
		for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
		{
			v[i].pos.x *= s;
			v[i].pos.y *= s;
			v[i].pos.z *= s;
		}
		m_pMesh->UnlockVertexBuffer();
	}
}

//Centers mesh object around its origin (0, 0, 0)
void Mesh::Center()
{
	//fout.open("report.txt", ios_base::out | ios_base::app);
	VertexPNT *v;
	float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
	m_pMesh->LockVertexBuffer(0, (void**)&v);
	
	// Set min and max to the first vertex for x, y, z
	minX = maxX = v[0].pos.x;
	minY = maxY = v[0].pos.y;
	minZ = maxZ = v[0].pos.z;

	// Go through mesh object and get minimum and maximum values for x, y, z
	for (int i = 1; i < m_pMesh->GetNumVertices(); i++)
	{
		if (v[i].pos.x < minX) minX = v[i].pos.x;
		else if (v[i].pos.x > maxX) maxX = v[i].pos.x;

		if (v[i].pos.y < minY) minY = v[i].pos.y;
		else if (v[i].pos.y > maxY) maxY = v[i].pos.y;

		if (v[i].pos.z < minZ) minZ = v[i].pos.z;
		else if (v[i].pos.z > maxZ) maxZ = v[i].pos.z;
	}

	float  midX = (minX + maxX) / 2;
	float  midY = (minY + maxY) / 2;
	float  midZ = (minZ + maxZ) / 2;
	/*fout << "minX = " << minX << " minY = " << minY << " minZ = " << minZ << " "<< endl;
	fout << "maxX = " << maxX << " maxY = " << maxY << " maxZ = " << maxZ << " "<< endl;
	fout << "midX = " << midX << " midY = " << midY << " midZ = " << midZ << " "<< endl;*/
	
	for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		v[i].pos.x = v[i].pos.x - midX;
		v[i].pos.y = v[i].pos.y - midY;
		v[i].pos.z = v[i].pos.z - midZ;
	}
	
	m_pMesh->UnlockVertexBuffer();
	//fout.close();
}

//Centers mesh object around specified point (x, y, z)
void Mesh::Center(float x, float y, float z)
{
	//fout.open("report.txt", ios_base::out | ios_base::app);
	VertexPNT *v;
	float minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
	m_pMesh->LockVertexBuffer(0, (void**)&v);
	
	// Set min and max to the first vertex for x, y, z
	minX = maxX = v[0].pos.x;
	minY = maxY = v[0].pos.y;
	minZ = maxZ = v[0].pos.z;

	// Go through mesh object and get minimum and maximum values for x, y, z
	for (int i = 1; i < m_pMesh->GetNumVertices(); i++)
	{
		if (v[i].pos.x < minX) minX = v[i].pos.x;
		else if (v[i].pos.x > maxX) maxX = v[i].pos.x;

		if (v[i].pos.y < minY) minY = v[i].pos.y;
		else if (v[i].pos.y > maxY) maxY = v[i].pos.y;

		if (v[i].pos.z < minZ) minZ = v[i].pos.z;
		else if (v[i].pos.z > maxZ) maxZ = v[i].pos.z;
	}

	float  midX = (minX + maxX) / 2;
	float  midY = (minY + maxY) / 2;
	float  midZ = (minZ + maxZ) / 2;
	/*fout << "minX = " << minX << " minY = " << minY << " minZ = " << minZ << " "<< endl;
	fout << "maxX = " << maxX << " maxY = " << maxY << " maxZ = " << maxZ << " "<< endl;
	fout << "midX = " << midX << " midY = " << midY << " midZ = " << midZ << " "<< endl;
	*/
	for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		v[i].pos.x = v[i].pos.x - (midX - x);
		v[i].pos.y = v[i].pos.y - (midY - y);
		v[i].pos.z = v[i].pos.z - (midZ - z);
	}
	
	m_pMesh->UnlockVertexBuffer();
	//fout.close();
}

float Mesh::GetRadius()
{
	float radius = 0.0f;
	VertexPNT *v;
	m_pMesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		float dist = sqrt(
			v[i].pos.x * v[i].pos.x + 
			v[i].pos.y * v[i].pos.y + 
			v[i].pos.z * v[i].pos.z);
		if (dist > radius)
			radius = dist;
	}
	m_pMesh->UnlockVertexBuffer();
	return radius;
}

float Mesh::GetHeight()
{
	float height = 0;
	VertexPNT *v;
	m_pMesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		if (v[i].pos.y > height)
			height = v[i].pos.y;
	}
	m_pMesh->UnlockVertexBuffer();
	return height;
}

float Mesh::GetOffset()
{
	float offset = 0;
	VertexPNT *v;
	m_pMesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < m_pMesh->GetNumVertices(); i++)
	{
		if (v[i].pos.z < offset)
			offset = v[i].pos.z;
	}
	m_pMesh->UnlockVertexBuffer();
	return offset;
}
