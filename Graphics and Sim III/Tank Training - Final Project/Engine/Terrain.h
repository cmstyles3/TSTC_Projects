#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>

void GenTriGrid(int numVertRows, int numVertCols, 
	float dx, float dz, const D3DXVECTOR3& center, 
	std::vector<D3DXVECTOR3>& verts, std::vector<DWORD>& indices);

void GenTriGrid(int numVertRows, int numVertCols, 
	float dx, float dz, const D3DXVECTOR3& center, 
	D3DXVECTOR3 *verts, DWORD *indices);



class Terrain
{
	public:
		Terrain();

		Terrain(
			UINT numVertRows, UINT numVertCols, 
			float dx, float dz, char *heighmap,
			char *tex1, char *tex2, char *tex3, char *tex4,
			D3DCOLOR color = D3DCOLOR_XRGB(0,0,255),
			float heightScale = 1.0f, float heightOffset = 0.0f);

		~Terrain();
		
		void Init(
			UINT vertRows, UINT vertCols,
			float dx, float dz, char *heightmap,
			char *tex1, char *tex2, char *tex3, char *tex4,
			D3DCOLOR color = D3DCOLOR_XRGB(0,0,255),
			float heightScale = 1.0f, float heightOffset = 0.0f);

		void Kill();


		DWORD GetNumVertices();
		DWORD GetNumTriangles();

		float GetWidth();
		float GetDepth();
		float GetMaxHeight();

		float GetHeight(float x, float z);

		D3DXVECTOR3 GetNormal(float x, float z);

		void Draw();

		void GenTexture(int width, int height, char *filename = "Terrain Texture.bmp");

	private:
		void BuildGeometry();
		void BuildSubGridMesh(RECT &R, VertexPT *gridVerts);

		void LoadRaw(int m, int n, char *filename, float heightScale, float heightOffset);
		float SampleHeight3x3(int i, int j);
		void Filter3x3();

		float TexFactor(float h1, float h2);

		std::vector<float> m_heightmap;
		float m_heightScale;
		float m_heightOffset;

		std::vector<D3DXVECTOR3> m_normals;
		
		std::vector<LPD3DXMESH> m_subGridMeshes;

		DWORD m_vertRows;
		DWORD m_vertCols;

		float m_width;
		float m_depth;

		float m_dx;
		float m_dz;

		LPDIRECT3DTEXTURE9 m_tex[4];
		D3DCOLOR m_color;

		LPDIRECT3DTEXTURE9 m_terrainTex;
	
};

#endif