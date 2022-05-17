#include "Terrain.h"

Terrain::Terrain()
{
	m_pDevice = 0;

	m_pPatches = 0;

	m_pHeightBuffer = 0;

	m_numVertsPerRow = m_numVertsPerCol = 0;
	m_cellSpacing = 0;

	m_numCellsPerRow = 0;
	m_numCellsPerCol = 0;

	m_numVertices = 0;
	m_numTriangles = 0;

	m_heightScale = 0;

	//m_pHeightmap = 0;
	m_pHeightBuffer = 0;

	m_numPatches = 0;

	for (int i = 0; i < 4; i++)
		m_pTex[i] = 0;
}

Terrain::~Terrain()
{
	Kill();
}

void Terrain::Init(LPDIRECT3DDEVICE9 pDevice, int numPatches, int size, int cellSpacing, float heightScale)
{
	if (!m_pDevice)
	{
		m_pDevice = pDevice;
		m_pDevice->AddRef();
	}
	// Allocate memory for each Terrain patch
	m_pPatches = new Patch[numPatches];
	m_numPatches = numPatches;

	m_numVertsPerRow = m_numVertsPerCol = size;

	m_numCellsPerRow = m_numVertsPerRow - 1;
	m_numCellsPerCol = m_numVertsPerCol - 1;

	m_cellSpacing = cellSpacing;

	m_width = m_numCellsPerRow * cellSpacing;
	m_depth = m_numCellsPerCol * cellSpacing;

	m_numVertices = m_numVertsPerRow * m_numVertsPerCol;
	m_numTriangles = m_numCellsPerRow * m_numCellsPerCol * 2;

	for (int i = 0; i < numPatches; i++)
	{
		m_pPatches[i].m_pHeightmap = new int[m_numVertices];
		m_pPatches[i].m_pIB = 0;
		m_pPatches[i].m_pTexture = 0;
		m_pPatches[i].m_pVB = 0;
	}

	m_heightScale = heightScale;

}

void Terrain::Kill()
{
	for (int i = 0; i < 4; i++)
		SAFE_RELEASE(m_pTex[i]);
	
	SAFE_DELETE_ARRAY(m_pHeightBuffer);

	for (int i = 0; i < m_numPatches; i++)
	{
		SAFE_DELETE_ARRAY(m_pPatches[i].m_pHeightmap);
		SAFE_RELEASE(m_pPatches[i].m_pIB);
		SAFE_RELEASE(m_pPatches[i].m_pVB);
		SAFE_RELEASE(m_pPatches[i].m_pTexture);
	}
	SAFE_DELETE_ARRAY(m_pPatches);
	
	SAFE_RELEASE(m_pDevice);
}

void Terrain::LoadHeightmap(char *fileName, int size)
{
	SAFE_DELETE_ARRAY(m_pHeightBuffer);
	m_pHeightBuffer = new BYTE[size*size];

	ifstream inFile(fileName, ios_base::binary);
	inFile.read((char*)m_pHeightBuffer, sizeof(BYTE) * size * size);// number of bytes to read into buffer
	inFile.close();
}

void Terrain::SetHeightmap(int patch, int startX, int startZ, int size)
{
	for (int z = startZ, z2 = 0; z2 < m_numVertsPerCol; z++, z2++)
		for (int x = startX, x2 = 0; x2 < m_numVertsPerRow; x++, x2++)
			m_pPatches[patch].m_pHeightmap[z2 * m_numVertsPerRow + x2] = m_pHeightBuffer[z * size + x];
	
	for (int i = 0; i < m_numVertices; i++)
		m_pPatches[patch].m_pHeightmap[i] *= m_heightScale;
	
	ComputeVertices(patch);
	ComputeIndices(patch);
	
}

void Terrain::LoadTextures(char *texFile1, char *texFile2, char *texFile3, char *texFile4, D3DXCOLOR c)
{
	D3DXCreateTextureFromFile(m_pDevice, texFile1, &m_pTex[0]);
	D3DXCreateTextureFromFile(m_pDevice, texFile2, &m_pTex[1]);
	D3DXCreateTextureFromFile(m_pDevice, texFile3, &m_pTex[2]);
	D3DXCreateTextureFromFile(m_pDevice, texFile4, &m_pTex[3]);
	m_baseColor = c;
}
void Terrain::KillSrcTextures()
{
	SAFE_RELEASE(m_pTex[0]);
	SAFE_RELEASE(m_pTex[1]);
	SAFE_RELEASE(m_pTex[2]);
	SAFE_RELEASE(m_pTex[3]);
}

void Terrain::KillHeightMap()
{
	SAFE_DELETE_ARRAY(m_pHeightBuffer);
}

float Terrain::Texfactor(float h1, float h2)
{
  float percent;
  percent = (64.0f - abs(h1 - h2)) / 64.0f;
 
  if(percent < 0.0f) percent = 0.0f;
  else if(percent > 1.0f) percent = 1.0f;  

  return percent;
  
}

bool Terrain::GenerateTexture(char completePath[], int patch, int texWidth, int texHeight)
{
	
	//sprintf(completePath, "%s\\%s", path, filename);
	if (SUCCEEDED(D3DXCreateTextureFromFile(m_pDevice, completePath, &m_pPatches[patch].m_pTexture)))
	{
		
		return true;
	}
	SAFE_RELEASE(m_pPatches[patch].m_pTexture);
	

	D3DXCreateTexture(m_pDevice,
	texWidth,texHeight, 
		0, // create a complete mipmap chain
		0, // usage
		D3DFMT_X8R8G8B8,// 32 bit XRGB format
		D3DPOOL_MANAGED, &m_pPatches[patch].m_pTexture);

	//if(FAILED(hr))
	//	return false;

	D3DSURFACE_DESC textureDesc; 
	m_pPatches[patch].m_pTexture->GetLevelDesc(0 /*level*/, &textureDesc);

	// make sure we got the requested format because our code 
	// that fills the texture is hard coded to a 32 bit pixel depth.
	if( textureDesc.Format != D3DFMT_X8R8G8B8 )
		return false;
	D3DLOCKED_RECT destRect;
	D3DLOCKED_RECT srcRect[4];
	int srcTexWidth[4];
	int srcTexHeight[4];

	for (int i = 0; i < 4; i++)
	{
		m_pTex[i]->GetLevelDesc(0, &textureDesc);
		srcTexWidth[i] = textureDesc.Width;
		srcTexHeight[i] = textureDesc.Height;
	}

	DWORD *destImageData;

	m_pPatches[patch].m_pTexture->LockRect(0, &destRect, 0, 0);

	for (int i = 0; i < 4; i++)
		m_pTex[i]->LockRect(0, &srcRect[i], 0, 0);

	destImageData = (DWORD*)destRect.pBits;
		
	DWORD *srcImageData[4];
	for (int i = 0; i < 4; i++)
		srcImageData[i] = (DWORD*)srcRect[i].pBits;

	
	for (int i = 0; i < texHeight; i++)
	{
		for (int j = 0; j < texWidth; j++)
		{
			float height = (float)GetHeight(patch, (m_numCellsPerRow / 32.0f) * ((m_cellSpacing/2.0f*j)/(texWidth/64)) - (m_width/2.0f), 
				(m_numCellsPerCol / 32.0f) * ((-m_cellSpacing/2.0f*i)/(texHeight/64)) + (m_depth/2.0f))/m_heightScale;

			/*float height = GetHeight(patch, (m_numCellsPerRow *  m_cellSpacing/2.0f * j) / (texWidth /2.0f) - (m_width/2.0f),
				                            (m_numCellsPerCol * -m_cellSpacing/2.0f * i) / (texHeight/2.0f) - (m_depth/2.0f)) / m_heightScale;*/

		
			float tex[5];
			tex[0] = Texfactor(256, height);
			tex[1] = Texfactor(196, height);
			tex[2] = Texfactor(128, height);
			tex[3] = Texfactor(64, height);
			tex[4] = Texfactor(0, height);

			D3DXCOLOR c;
			c = (tex[0] * (D3DXCOLOR)srcImageData[0][((i%srcTexHeight[0])*srcRect[0].Pitch/4 + (j%srcTexWidth[0]))]) + 
				(tex[1] * (D3DXCOLOR)srcImageData[1][((i%srcTexHeight[1])*srcRect[1].Pitch/4 + (j%srcTexWidth[1]))]) +
				(tex[2] * (D3DXCOLOR)srcImageData[2][((i%srcTexHeight[2])*srcRect[2].Pitch/4 + (j%srcTexWidth[2]))]) + 
				(tex[3] * (D3DXCOLOR)srcImageData[3][((i%srcTexHeight[3])*srcRect[3].Pitch/4 + (j%srcTexWidth[3]))]) +
				(tex[4] * m_baseColor);

			destImageData[i * destRect.Pitch / 4 + j] = (D3DCOLOR)c;

		}
	}


	for (int i = 0; i < 4; i++)
		m_pTex[i]->UnlockRect(0);
	m_pPatches[patch].m_pTexture->UnlockRect(0);


	D3DXFilterTexture(
		m_pPatches[patch].m_pTexture,
		0, // default palette
		0, // use top level as source level
		D3DX_DEFAULT); // default filter


	D3DXSaveTextureToFile(completePath, D3DXIFF_BMP, m_pPatches[patch].m_pTexture, 0);

	return(true);
}

bool Terrain::ComputeVertices(int patch)
{
	m_pDevice->CreateVertexBuffer(m_numVertices * sizeof(VertexTex1), D3DUSAGE_WRITEONLY, D3DFVF_XYZTEX1, D3DPOOL_DEFAULT, &m_pPatches[patch].m_pVB, 0);

	float startX = -m_width/2;
	float startZ =  m_depth/2;

	float endX =  m_width/2;
	float endZ = -m_depth/2;

	float uCoordIncrementSize = 1.0f / ((float)m_numCellsPerRow);
	float vCoordIncrementSize = 1.0f / ((float)m_numCellsPerCol);

	VertexTex1 *v;
	m_pPatches[patch].m_pVB->Lock(0, 0, (void**)&v, 0);
	int i = 0;
	fstream outFile("vertices.txt", ios_base::trunc | ios_base::out);
	for (float z = startZ; z >= endZ; z -= m_cellSpacing)
	{
		int j = 0;
		for (float x = startX; x <= endX; x += m_cellSpacing)
		{
			int index = (i * m_numVertsPerRow + j);
			v[index] = VertexTex1(x, m_pPatches[patch].m_pHeightmap[index], z, 
				(float)(j * uCoordIncrementSize), (float)(i * vCoordIncrementSize));
			outFile << "x = " << v[index].x << " y = " << v[index].y << " z =  " << v[index].z << endl;
			j++;
		}
		i++;
	}
	outFile.close();
	m_pPatches[patch].m_pVB->Unlock();

	return true;
}

bool Terrain::ComputeIndices(int patch)
{
	m_pDevice->CreateIndexBuffer(m_numTriangles * 3 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pPatches[patch].m_pIB, 0);
	WORD *indices;
	int baseIndex = 0;
	m_pPatches[patch].m_pIB->Lock(0, 0, (void**)&indices, 0);
	for (int i = 0; i < m_numCellsPerRow; i++)
	{
		for(int j = 0; j < m_numCellsPerCol; j++)
		{
			indices[baseIndex]   = i * m_numVertsPerRow + j;
			indices[baseIndex+1] = i * m_numVertsPerRow + j + 1;
			indices[baseIndex+2] = (i+1) * m_numVertsPerRow + j;

			indices[baseIndex+3] = (i+1) * m_numVertsPerRow + j;
			indices[baseIndex+4] = i * m_numVertsPerRow + j + 1;
			indices[baseIndex+5] = (i+1) * m_numVertsPerRow + j + 1;
			baseIndex += 6;
		}
	}
	m_pPatches[patch].m_pIB->Unlock();
	return(true);
}

int Terrain::GetHeightmapEntry(int patch, int row, int col)
{
	return (m_pPatches[patch].m_pHeightmap[row * m_numVertsPerRow + col]);
}

void Terrain::SetHeightmapEntry(int patch, int row, int col, int value)
{
	m_pPatches[patch].m_pHeightmap[row * m_numVertsPerRow + col] = value;
}

float Terrain::GetHeight(int patch, float x, float z)
{
	// Translate on xz-plane by the transformation that takes
	// the terrain START point to the origin.
	x = ((float)m_width / 2.0f) + x;
	z = ((float)m_depth / 2.0f) - z;

	// Scale down by the transformation that makes the 
	// cellspacing equal to one.  This is given by 
	// 1 / cellspacing since; cellspacing * 1 / cellspacing = 1.
	x /= (float)m_cellSpacing;
	z /= (float)m_cellSpacing;

	// From now on, we will interpret our positive z-axis as
	// going in the 'down' direction, rather than the 'up' direction.
	// This allows to extract the row and column simply by 'flooring'
	// x and z:

	float col = floorf(x);
	float row = floorf(z);

	// get the heights of the quad we're in:
	// 
    //  A   B
    //  *---*
    //  | / |
    //  *---*  
    //  C   D

	float A = GetHeightmapEntry(patch, row,   col);
	float B = GetHeightmapEntry(patch, row,   col+1);
	float C = GetHeightmapEntry(patch, row+1, col);
	float D = GetHeightmapEntry(patch, row+1, col+1);

	//
	// Find the triangle we are in:
	//

	// Translate by the transformation that takes the upper-left
	// corner of the cell we are in to the origin.  Recall that our 
	// cellspacing was nomalized to 1.  Thus we have a unit square
	// at the origin of our +x -> 'right' and +z -> 'down' system.
	float dx = x - col;
	float dz = z - row;

	// Note the below compuations of u and v are unneccessary, we really
	// only need the height, but we compute the entire vector to emphasis
	// the books discussion.
	float height = 0.0f;
	if(dz < 1.0f - dx)  // upper triangle ABC
	{
		float uy = B - A; // A->B
		float vy = C - A; // A->C

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {A}, plus the heights
		// found by interpolating on each vector u and v.
		height = A + Lerp(0.0f, uy, dx) + Lerp(0.0f, vy, dz);
	}
	else // lower triangle DCB
	{
		float uy = C - D; // D->C
		float vy = B - D; // D->B

		// Linearly interpolate on each vector.  The height is the vertex
		// height the vectors u and v originate from {D}, plus the heights
		// found by interpolating on each vector u and v.
		height = D + Lerp(0.0f, uy, 1.0f - dx) + Lerp(0.0f, vy, 1.0f - dz);
		
	}

	return height;
}

bool Terrain::Draw(int patch, D3DXMATRIX* world, bool showTris)
{
	HRESULT hr = 0;

	if (m_pDevice)
	{
		if (showTris)
			m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		m_pDevice->SetTransform(D3DTS_WORLD, world);

		m_pDevice->SetStreamSource(0, m_pPatches[patch].m_pVB, 0, sizeof(VertexTex1));
		m_pDevice->SetFVF(D3DFVF_XYZTEX1);
		m_pDevice->SetIndices(m_pPatches[patch].m_pIB);
		
		m_pDevice->SetTexture(0, m_pPatches[patch].m_pTexture);

		// turn off lighting since we're lighting it ourselves
		m_pDevice->SetRenderState(D3DRS_LIGHTING, false);
		m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);

		hr = m_pDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			m_numVertices,
			0,
			m_numTriangles);

		m_pDevice->SetRenderState(D3DRS_LIGHTING, true);
		if (showTris)
			m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );

		if(FAILED(hr))
			return false;

		/*if (showTris)
		{
			m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			m_pDevice->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST,
			0,
			0,
			m_numVertices,
			0,
			m_numTriangles);
			m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );
		}*/
	}

	return true;
}

