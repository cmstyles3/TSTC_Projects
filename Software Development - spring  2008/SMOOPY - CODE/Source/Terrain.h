#ifndef _TERRAIN__H
#define _TERRAIN__H


#include <d3dx9.h>
#include <math.h>
#include <fstream>
#include "Vertex.h"
#include "basicutility.h"
#include "dxutil/dxutil.h"

using namespace std;

class Terrain
{
	struct Patch
	{
		LPDIRECT3DTEXTURE9 m_pTexture;
		LPDIRECT3DVERTEXBUFFER9 m_pVB;
		LPDIRECT3DINDEXBUFFER9 m_pIB;
		int *m_pHeightmap;
	};

	public:
		Terrain();
		~Terrain();

		void Init(LPDIRECT3DDEVICE9 pDevice, int numPatches, int size, int cellSpacing, float heightScale);

		void Kill();

		void KillSrcTextures();

		void KillHeightMap();

		//void Kill

		void LoadHeightmap(char fileName[], int size);

		void SetHeightmap(int patch, int startX, int startZ, int size);

		void LoadTextures(char texFile1[], char texFile2[], char texFile3[], char texFile4[], D3DXCOLOR c);

		int GetHeightmapEntry(int patch, int row, int col);

		void SetHeightmapEntry(int patch, int row, int col, int value);

		float GetHeight(int patch, float x, float z);

		bool GenerateTexture(char completePath[], int patch, int texWidth, int texHeight);

		bool Draw(int patch, D3DXMATRIX *world, bool showTris = false);

		float Texfactor(float h1, float h2);

	private:
		LPDIRECT3DDEVICE9 m_pDevice;
		Patch *m_pPatches;
		

		LPDIRECT3DTEXTURE9 m_pTex[4];
		D3DXCOLOR m_baseColor;

		int m_numVertsPerRow;
		int m_numVertsPerCol;

		int m_cellSpacing;
		int m_numCellsPerRow;
		int m_numCellsPerCol;

		int m_width;
		int m_depth;

		int m_numVertices;
		int m_numTriangles;

		float m_heightScale;

		int m_numPatches;

		
		BYTE *m_pHeightBuffer;

		bool  ComputeVertices(int patch);
		bool  ComputeIndices(int patch);

};

#endif _TERRAIN__H