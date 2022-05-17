#include "Engine.h"

const int SUBGRID_ROWS = 33;
const int SUBGRID_COLS = 33;
const int SUBGRID_VERTS = SUBGRID_ROWS * SUBGRID_COLS;
const int SUBGRID_TRIS = (SUBGRID_ROWS - 1) * (SUBGRID_COLS - 1) * 2;


void GenTriGrid(int numVertRows, int numVertCols, 
	float dx, float dz, const D3DXVECTOR3& center, 
	std::vector<D3DXVECTOR3>& verts, std::vector<DWORD>& indices)
{
	int numVertices = numVertRows * numVertCols;
	int numCellRows = numVertRows - 1;
	int numCellCols = numVertCols - 1;

	int numTriangles = numCellRows * numCellCols * 2;

	float width = (float)numCellCols * dx;
	float depth = (float)numCellRows * dz;

	verts.resize(numVertices);

	float xOffset = -width * 0.5f;
	float zOffset =  depth * 0.5f;

	int k = 0;

	for (int i = 0; i < numVertRows; i++)
	{
		for (int j = 0; j < numVertCols; j++)
		{
			verts[k].x =  (float)j * dx + xOffset;
			verts[k].z = -(float)i * dz + zOffset;
			verts[k].y = 0.0f;

			D3DXMATRIX T;
			D3DXMatrixTranslation(&T, center.x, center.y, center.z);
			D3DXVec3TransformCoord(&verts[k], &verts[k], &T);

			k++;
		}
	}


	indices.resize(numTriangles * 3);

	k = 0;
	for (DWORD i = 0; i < (DWORD)numCellRows; i++)
	{
		for (DWORD j = 0; j < (DWORD)numCellCols; j++)
		{
			indices[k] = i * numVertCols + j;
			indices[k+1] = i * numVertCols + j + 1;
			indices[k+2] = (i+1) * numVertCols + j;

			indices[k+3] = (i+1) * numVertCols + j;
			indices[k+4] = i * numVertCols + j + 1;
			indices[k+5] = (i+1) * numVertCols + j + 1;

			k += 6;
		}
	}
}

void GenTriGrid(int numVertRows, int numVertCols, 
	float dx, float dz, const D3DXVECTOR3& center, 
	D3DXVECTOR3 *verts, DWORD *indices)
{
	int numVertices = numVertRows * numVertCols;
	int numCellRows = numVertRows - 1;
	int numCellCols = numVertCols - 1;

	int numTriangles = numCellRows * numCellCols * 2;

	float width = (float)numCellCols * dx;
	float depth = (float)numCellRows * dz;

	//verts.resize(numVertices);

	float xOffset = -width * 0.5f;
	float zOffset =  depth * 0.5f;

	int k = 0;

	for (int i = 0; i < numVertRows; i++)
	{
		for (int j = 0; j < numVertCols; j++)
		{
			verts[k].x =  (float)j * dx + xOffset;
			verts[k].z = -(float)i * dz + zOffset;
			verts[k].y = 0.0f;

			D3DXMATRIX T;
			D3DXMatrixTranslation(&T, center.x, center.y, center.z);
			D3DXVec3TransformCoord(&verts[k], &verts[k], &T);

			k++;
		}
	}


	//indices.resize(numTriangles * 3);

	k = 0;
	for (DWORD i = 0; i < (DWORD)numCellRows; i++)
	{
		for (DWORD j = 0; j < (DWORD)numCellCols; j++)
		{
			indices[k] = i * numVertCols + j;
			indices[k+1] = i * numVertCols + j + 1;
			indices[k+2] = (i+1) * numVertCols + j;

			indices[k+3] = (i+1) * numVertCols + j;
			indices[k+4] = i * numVertCols + j + 1;
			indices[k+5] = (i+1) * numVertCols + j + 1;

			k += 6;
		}
	}
}


Terrain::Terrain()
{
	for (int i = 0; i < 4; i++)
		m_tex[i] = 0;
	m_color = D3DCOLOR_XRGB(255, 255, 255);
}

Terrain::Terrain(UINT vertRows, UINT vertCols, 
				 float dx, float dz, char *heightmap,
				 char *tex1, char *tex2, char *tex3, char *tex4, 
				 D3DCOLOR color,
				 float heightScale, float heightOffset)
{
	Init(vertRows, vertCols, dx, dz, 
		heightmap, tex1, tex2, tex3, tex4, 
		color, heightScale, heightOffset);
}

Terrain::~Terrain()
{
	Kill();
}

void Terrain::Init(UINT vertRows, UINT vertCols, 
				   float dx, float dz, char *heightmap, 
				   char *tex1, char *tex2, char *tex3, char *tex4,
				   D3DCOLOR color,
				   float heightScale, float heightOffset)
{
	m_vertRows = vertRows;
	m_vertCols = vertCols;

	m_dx = dx;
	m_dz = dz;

	m_width = (m_vertRows-1) * m_dx;
	m_depth = (m_vertCols-1) * m_dz;

	m_heightScale = heightScale;
	m_heightOffset = heightOffset;
	LoadRaw(vertRows, vertCols, heightmap, heightScale, heightOffset);

	D3DXCreateTextureFromFile(g_engine->GetDevice(), tex1, &m_tex[0]);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), tex2, &m_tex[1]);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), tex3, &m_tex[2]);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), tex4, &m_tex[3]);

	m_color = color;

	BuildGeometry();
}

void Terrain::Kill()
{
	for (int i = 0; i < 4; i++)
		SAFE_RELEASE(m_tex[i]);
	SAFE_RELEASE(m_terrainTex);
	for (int i = 0; i < m_subGridMeshes.size(); i++)
		SAFE_RELEASE(m_subGridMeshes[i]);

	m_subGridMeshes.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////
// HEIGHTMAP FUNCTIONS
void Terrain::LoadRaw(int m, int n, char *filename, float heightScale, float heightOffset)
{
	std::vector<unsigned char> in(m*n);
	
	//unsigned char *in = new unsigned char[m*n];
	std::ifstream inFile;
	inFile.open(filename, ios_base::binary);

	inFile.read((char*)&in[0], (streamsize)sizeof(unsigned char) * m * n);

	inFile.close();

	m_heightmap.resize(m*n);

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			int k = i * n + j;
			m_heightmap[k] = (float)in[k] * heightScale + heightOffset;
		}
	}
	//delete[] in;

	//for (int c = 0; c < 1000; c++)
	Filter3x3();
	
}

float Terrain::SampleHeight3x3(int i, int j)
{
	float avg = 0.0f;
	float num = 0.0f;

	for (int m = i-1; m < i+1; m++)
	{
		for (int n = j-1; n < j+1; n++)
		{
			if (m >= 0 && m < m_vertRows &&
				n >= 0 && n < m_vertCols)
			{
				int index = m * m_vertCols + n;
				avg += m_heightmap[index];
				num += 1.0f;
			}
		}
	}
	return avg/num;
}

void Terrain::Filter3x3()
{
	for (int i = 0; i < m_vertRows; i++)
	{
		for (int j = 0; j < m_vertCols; j++)
		{
			SampleHeight3x3(i, j);
		}
	}
}
// END OF HEIGHMAP FUNCTIONS ///////////////////////////////////////////////////////////////////////////


void Terrain::BuildGeometry()
{
	DWORD numTris = (m_vertRows-1) * (m_vertCols-1) * 2;
	DWORD numVerts = m_vertRows * m_vertCols;

	LPD3DXMESH mesh;

	D3DXCreateMeshFVF(numTris, numVerts, D3DXMESH_SYSTEMMEM  | D3DXMESH_32BIT, D3DFVF_PNT, g_engine->GetDevice(), &mesh);

	VertexPNT *v = 0;
	
	/*std::vector<D3DXVECTOR3> verts;
	std::vector<DWORD> indices;*/
	D3DXVECTOR3 *verts = new D3DXVECTOR3[numVerts];
	DWORD *indices = new DWORD[numTris*3];
	GenTriGrid(m_vertRows, m_vertCols, m_dx, m_dz, D3DXVECTOR3(0.0f, 0.0f, 0.0f), verts, indices);

	float w = m_width;
	float d = m_depth;

	mesh->LockVertexBuffer(0, (void**)&v);
	for (UINT i = 0; i < mesh->GetNumVertices(); i++)
	{
		v[i].pos = verts[i];
		v[i].pos.y = m_heightmap[i];

		v[i].tex.x = (v[i].pos.x + (0.5f * w)) / w;
		v[i].tex.y = -(v[i].pos.z - (0.5f * d)) / d;

	}
	mesh->UnlockVertexBuffer();//(0, (void**)&v);
	

	DWORD *indexBuffer = 0;
	mesh->LockIndexBuffer(0, (void**)&indexBuffer);
	for (UINT i = 0; i < mesh->GetNumFaces(); i++)
	{
		indexBuffer[i*3+0] = indices[i*3+0];
		indexBuffer[i*3+1] = indices[i*3+1];
		indexBuffer[i*3+2] = indices[i*3+2];
	}
	
	
	
	D3DXComputeNormals(mesh, 0);

	m_normals.resize(mesh->GetNumVertices());
	for (int i = 0; i < mesh->GetNumVertices(); i++)
	{
		m_normals[i] = v[i].normal;
	}
	mesh->UnlockIndexBuffer();

	LPD3DXMESH mesh2;
	mesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM | D3DXMESH_32BIT, D3DFVF_PT, g_engine->GetDevice(), &mesh);

	VertexPT *v2;

	mesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&v2);

	int subGridRows = (m_vertRows-1)/(SUBGRID_ROWS-1);
	int subGridCols = (m_vertCols-1)/(SUBGRID_COLS-1);

	for (int r = 0; r < subGridRows; r++)
	{
		for (int c = 0; c < subGridCols; c++)
		{
			RECT R = 
			{
				c * (SUBGRID_COLS-1),
				r * (SUBGRID_ROWS-1),
				(c+1) * (SUBGRID_COLS-1),
				(r+1) * (SUBGRID_ROWS-1)
			};
			
			BuildSubGridMesh(R, v2);
		}
	}
	delete [] verts;
	delete [] indices;
	mesh->UnlockVertexBuffer();

	SAFE_RELEASE(mesh);

}

void Terrain::BuildSubGridMesh(RECT &R, VertexPT *gridVerts)
{
	LPD3DXMESH subMesh;
	D3DXCreateMeshFVF(SUBGRID_TRIS, SUBGRID_VERTS, D3DXMESH_SYSTEMMEM | D3DXMESH_WRITEONLY, D3DFVF_PT, g_engine->GetDevice(), &subMesh);

	VertexPT *v;
	subMesh->LockVertexBuffer(0, (void**)&v);
	int k = 0;
	for (int i = R.top; i <= R.bottom; i++)
	{
		for (int j = R.left; j <= R.right; j++)
		{
			v[k] = gridVerts[i*m_vertCols+j];
			k++;
		}
	}
	subMesh->UnlockVertexBuffer();


	/*std::vector<D3DXVECTOR3> tempVerts;
	std::vector<DWORD> tempIndices;*/

	D3DXVECTOR3 *tempVerts = new D3DXVECTOR3[SUBGRID_ROWS*SUBGRID_COLS];
	DWORD *tempIndices = new DWORD[SUBGRID_TRIS*3];
	GenTriGrid(SUBGRID_ROWS, SUBGRID_COLS, m_dx, m_dz, D3DXVECTOR3(0.0f,0.0f,0.0f), tempVerts, tempIndices);

	WORD *indices;
	DWORD *attrBuffer;
	subMesh->LockIndexBuffer(0, (void**)&indices);
	subMesh->LockAttributeBuffer(0, &attrBuffer);
	for (int i = 0; i < SUBGRID_TRIS; i++)
	{
		indices[i*3+0] = (WORD)tempIndices[i*3+0];
		indices[i*3+1] = (WORD)tempIndices[i*3+1];
		indices[i*3+2] = (WORD)tempIndices[i*3+2];

		attrBuffer[i] = 0;
	}
	subMesh->UnlockIndexBuffer();
	subMesh->UnlockAttributeBuffer();

	delete [] tempVerts;
	delete [] tempIndices;

	DWORD *adj = new DWORD[subMesh->GetNumFaces()*3];
	subMesh->GenerateAdjacency(0.001f, adj);
	subMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE|D3DXMESHOPT_ATTRSORT, adj, 0, 0, 0);
	delete[] adj;

	m_subGridMeshes.push_back(subMesh);
}

void Terrain::Draw()
{
	bool lighting;
	/*g_engine->GetDevice()->GetRenderState(D3DRS_LIGHTING, (DWORD *)&lighting);
	if (lighting)*/
		g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);

	g_engine->GetDevice()->SetTexture(0, m_terrainTex);
	for (int i = 0; i < m_subGridMeshes.size(); i++)
		m_subGridMeshes[i]->DrawSubset(0);
	
	g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, true);
}

float Terrain::GetHeight(float x, float z)
{
	float c = (x + 0.5f * m_width) / m_dx;
	float r = (z - 0.5f * m_depth) / -m_dz;
	
	int row = (int)floorf(r);
	int col = (int)floorf(c);

	if (row < 0 || col < 0 || row >= m_vertRows-1 || col >= m_vertCols-1)
		return -1;

	float A = m_heightmap[row * m_vertCols + col];
	float B = m_heightmap[row * m_vertCols + col+1];
	float C = m_heightmap[(row+1) * m_vertCols + col];
	float D = m_heightmap[(row+1) * m_vertCols + col+1];

	float s = c - (float)col;
	float t = r - (float)row;

	if (t < 1.0f - s)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s * uy + t * vy;
	}
	else
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}

}

D3DXVECTOR3 Terrain::GetNormal(float x, float z)
{
	float c = (x + 0.5f * m_width) / m_dx;
	float r = (z - 0.5f * m_depth) / -m_dz;
	
	int row = (int)floorf(r);
	int col = (int)floorf(c);

	D3DXVECTOR3 A = m_normals[row * m_vertCols + col];
	D3DXVECTOR3 B = m_normals[row * m_vertCols + col+1];
	D3DXVECTOR3 C = m_normals[(row+1) * m_vertCols + col];
	D3DXVECTOR3 D = m_normals[(row+1) * m_vertCols + col+1];

	float s = c - (float)col;
	float t = r - (float)row;

	if (t < 1.0f - s)
	{
		D3DXVECTOR3 uy = B - A;
		D3DXVECTOR3 vy = C - A;
		D3DXVECTOR3 normal1, normal2;
		D3DXVec3Lerp(&normal1,&D3DXVECTOR3(0.0f,0.0f, 0.0f), &uy, s);
		D3DXVec3Lerp(&normal2, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &vy, t);
		return A + normal1 + normal2;
	}
	else
	{
		D3DXVECTOR3 uy = C - D;
		D3DXVECTOR3 vy = B - D;
		D3DXVECTOR3 normal1, normal2;
		D3DXVec3Lerp(&normal1,&D3DXVECTOR3(0.0f,0.0f, 0.0f), &uy, 1.0f - s);
		D3DXVec3Lerp(&normal2, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &vy, 1.0f - t);
		return D + normal1 + normal2;
	}
}

float Terrain::TexFactor(float h1, float h2)
{
  float percent;
  percent = (64.0f - abs(h1 - h2)) / 64.0f;
 
  if (percent < 0.0f) percent = 0.0f;
  else if (percent > 1.0f) percent = 1.0f;  

  return percent;
  
}

void Terrain::GenTexture(int width, int height, char *filename)
{
	int numCellRows = m_vertRows-1;
	int numCellCols = m_vertCols-1;

	if (SUCCEEDED(D3DXCreateTextureFromFile(g_engine->GetDevice(), filename, &m_terrainTex)))
		return;

	D3DXCreateTexture(g_engine->GetDevice(), width, height, 0, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_terrainTex);
	
	D3DSURFACE_DESC surfaceDesc;
	int texWidth[4], texHeight[4];
	for (int i = 0; i < 4; i++)
	{
		m_tex[i]->GetLevelDesc(0, &surfaceDesc);
		texWidth[i] = surfaceDesc.Width;
		texHeight[i] = surfaceDesc.Height;
	}

	D3DLOCKED_RECT destRect;
	DWORD *destImageData;
	m_terrainTex->LockRect(0, &destRect, 0, 0);
	destImageData = (DWORD*)destRect.pBits;

	D3DLOCKED_RECT srcRect[4];
	DWORD *srcImageData[4];
	for (int i = 0; i < 4; i++)
	{
		m_tex[i]->LockRect(0, &srcRect[i], 0, 0);
		srcImageData[i] = (DWORD*)srcRect[i].pBits;
	}

	for (int i = 0; i < height-1; i++)
	{
		for (int j = 0; j < width-1; j++)
		{
			float x = ((float)j/(float)width) * m_width - (0.5f * m_width);
			float z = -((float)i/(float)height) * m_depth + (0.5f * m_depth);

			float h = GetHeight(x, z) / m_heightScale;

			float texFactor[5];
			texFactor[0] = TexFactor(256, h);
			texFactor[1] = TexFactor(196, h);
			texFactor[2] = TexFactor(128, h);
			texFactor[3] = TexFactor( 64, h);
			texFactor[4] = TexFactor(  0, h);

			D3DXCOLOR pixel[5];
			D3DXCOLOR c(0xff000000);
			for (int k = 0; k < 4; k++)
			{
				unsigned int index = (i%texHeight[k]) * srcRect[k].Pitch/4 + (j%texWidth[k]);
				pixel[k] = texFactor[k] * (D3DXCOLOR)srcImageData[k][index];
				c += pixel[k];
			}
			pixel[4] = texFactor[4] * (D3DXCOLOR)m_color;
			
			c += pixel[4];
			
			D3DXVECTOR3 normal = GetNormal(x, z);
			D3DXVECTOR3 dir(0.0f, 1.0f, 0.0f);
			D3DXVec3Normalize(&normal, &normal);

			float cosine = D3DXVec3Dot(&normal, &dir);
			if (cosine < 0.0f) cosine = 0.0f;

			c *= cosine;

			destImageData[i * destRect.Pitch / 4 + j] = (D3DCOLOR)c;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		m_tex[i]->UnlockRect(0);
	}
	m_terrainTex->UnlockRect(0);

	D3DXFilterTexture(
		m_terrainTex,
		0, 
		0, 
		D3DX_DEFAULT); 

	D3DXSaveTextureToFile(filename, D3DXIFF_BMP, m_terrainTex, 0);
}

float Terrain::GetWidth()
{
	return m_width;
}

float Terrain::GetDepth()
{
	return m_depth;
}

float Terrain::GetMaxHeight()
{
	return m_heightScale * 255;
}