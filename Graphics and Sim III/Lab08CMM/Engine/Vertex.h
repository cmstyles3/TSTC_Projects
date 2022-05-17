// VERTEX.H

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _VERTEX__H
#define _VERTEX__H


// DEFINE ////////////////////////////////////////////////////////////////////////////////////////////////

// Define custom vertex format so that contains position, normal, and texture
#define D3DFVF_XYZ_NORMAL_TEX1 (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1) 
#define D3DFVF_XYZ_DIFFUSE (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define D3DFVF_SPRITEVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
#define D3DFVF_XYZTEX1 (D3DFVF_XYZ | D3DFVF_TEX1)


// STRUCTURE //////////////////////////////////////////////////////////////////////////////////////////////

struct VertexTex1
{
	float x, y, z;
	float u, v;
	VertexTex1(float x, float y, float z, float u, float v)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->u = u;
		this->v = v;
	}
};


class ColorVertex
{
	public:
		D3DXVECTOR3 m_Pos;
		int m_Color;
		ColorVertex()
		{
			m_Pos.x = m_Pos.y = m_Pos.z = 0.0f;
			m_Color = 0;
		}
		ColorVertex(float x, float y, float z, int color)
		{
			m_Pos.x = x, m_Pos.y = y, m_Pos.z = z, m_Color = color;
		}
};

// Vertex structure to describe vertices that contain position, normal, and texture data
class Vertex
{
	public:
		D3DXVECTOR3 m_Pos;
		D3DXVECTOR3 m_Normal;
		float m_U, m_V;

		Vertex()
		{
			m_Pos.x = m_Pos.y = m_Pos.z = 0.0f;
			m_Normal.x = m_Normal.y = m_Normal.z = 0.0f;
		}
		Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		{
			m_Pos.x = x, m_Pos.y = y, m_Pos.z = z;
			m_Normal.x = nx, m_Normal.y = ny, m_Normal.z = nz;
			m_U = u, m_V = v;
		}
		Vertex(float x, float y, float z, float u, float v)
		{
			m_Pos.x = x, m_Pos.y = y, m_Pos.z = z;
			m_U = u, m_V = v;
		}
};

//
class SpriteVertex
{
	public:
		D3DXVECTOR3 m_pos;
		float m_rhw;
		DWORD m_color;
		float m_u, m_v;
		SpriteVertex()
		{
			m_pos.x = m_pos.y = m_pos.z = 0.0f;
			m_rhw = 1.0f;
			m_color = 0;
			m_u = m_v = 0.0f;
		}
		SpriteVertex(float x, float y, DWORD color, float u, float v)
		{
			m_pos.x = x;
			m_pos.y = y;
			m_pos.z = 0.0f;
			m_rhw = 1.0f;
			m_color = color;
			m_u = u;
			m_v = v;

		}
		
};

#define D3DFVF_PDT (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

struct VertexPDT
{
	D3DXVECTOR3 pos;
	DWORD diffuse;
	D3DXVECTOR2 tex;

	VertexPDT(){}

	VertexPDT(D3DXVECTOR3 pos, DWORD diffuse, D3DXVECTOR2 tex)
	{
		this->pos = pos;
		this->diffuse = diffuse;
		this->tex = tex;
	}

	VertexPDT(float x, float y, float z, DWORD diffuse, float u, float v)
	{
		pos = D3DXVECTOR3(x, y, z);
		this->diffuse = diffuse;
		tex = D3DXVECTOR2(u, v);
	}
};

#define D3DFVF_PT (D3DFVF_XYZ | D3DFVF_TEX1)

struct VertexPT
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
};

#define D3DFVF_PNT (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

struct VertexPNT
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex;
};

#define D3DFVF_VERTEX ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

struct VERTEX 
{
    float	 x;
    float	 y;
    float	 z;
	D3DCOLOR rgba;
	float	 tu; 
	float	 tv; 
};

#endif _VERTEX__H

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////