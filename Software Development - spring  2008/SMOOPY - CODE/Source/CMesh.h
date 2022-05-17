#ifndef _CMESH__H
#define _CMESH__H

#include <stdio.h>
#include <fstream>
#include <d3dx9.h>
#include "d3dbase.h"
#include "Vertex.h"
#include "BasicUtility.h"
//#include "CReport.h"

using namespace std;

class CMesh
{
	public:
		CMesh();
		~CMesh();
		
		int LoadXFile(const char file[], const char modelPath[] = "Models", const char texturePath[] = "Textures");

		void Render(bool bBoundingSphere = false);

		void Kill();

		void Scale(float s);

		void Center();
		void Center(float x, float y, float z);

		float GetRadius();

		float GetHeight();

		float GetOffset();

	private:
		DWORD m_NumMaterials;
		LPD3DXMESH m_pMesh;
		LPDIRECT3DTEXTURE9* m_pTextures;
		D3DMATERIAL9* m_pMaterials;
		fstream fout;
		LPD3DXMESH m_pBoundingSphere;
		//CReport m_Report;

};

#endif _CMESH__H