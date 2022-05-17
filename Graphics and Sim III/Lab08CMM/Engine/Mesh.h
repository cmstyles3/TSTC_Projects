// MESH.H

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MESH__H
#define _MESH__H

#include <stdio.h>
#include <fstream>

using namespace std;

class Mesh
{
	public:
		Mesh();
		~Mesh();
		
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

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////