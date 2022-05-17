#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <d3dx9.h>

#define D3DFVF_VERTEX D3DFVF_XYZ | D3DFVF_DIFFUSE

struct Vertex
{
	D3DXVECTOR3 pos;
	DWORD color;
};



//void MeshCenter(LPD3DXMESH mesh)
//{
//	Vertex *v;
//	mesh->LockVertexBuffer(0, (void**)&v);
//	D3DXVECTOR3 min(0.0f, 0.0f, 0.0f), max(0.0f, 0.0f, 0.0f);
//	for (int i = 0; i < mesh->GetNumVertices(); i++)
//	{
//	
//	}
//	mesh->UnlockVertexBuffer();
//}

void MeshScale(LPD3DXMESH mesh,float s)
{
	Vertex *v;
	mesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < mesh->GetNumVertices(); i++)
		v[i].pos *= s;
	mesh->UnlockVertexBuffer();
}
	
// BEGIN POLYGON OBJECT 0 --------------------------------------------
// Vertices for Polygon 0 --------------------------------------------
Vertex vertices0[] = 
{
	{ D3DXVECTOR3(-3.0f, 3.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3( 0.0f, 3.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3( 3.0f, 3.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) }, 
	{ D3DXVECTOR3(-1.0f, 2.0f, 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3( 1.0f, 2.0f, 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(-3.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  0,255,  0) },
	{ D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  0,255,  0) },
	{ D3DXVECTOR3( 1.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  0,255,  0) },
	{ D3DXVECTOR3( 3.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  0,255,  0) },
	{ D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(  0,150,150) },
	{ D3DXVECTOR3( 1.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(  0,150,150) },
	{ D3DXVECTOR3(-3.0f,-1.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3(-1.0f,-1.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3( 1.0f,-1.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3( 3.0f,-1.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3( 0.0f,-2.0f, 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(-3.0f,-3.0f, 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3( 3.0f,-3.0f, 0.0f), D3DCOLOR_XRGB(  0,  0,255) }

};
// Indices for Polygon 0 -----------------------------------------------
WORD indices0[] = 
{ 
	 0,  3,  6,
  	 3,  1,  4,
	 6,  3,  4,
	 6,  4,  7,
	 7,  4,  2,
	 5,  6,  9,
	 9,  6,  7,
	 9,  7, 10,
	10,  7,  8,
	11,  9, 12,
	12,  9, 10,
	12, 10, 13,
	13, 10, 14,
	12, 13, 15,
	16, 12, 15,
	15, 13, 17
};
// END POLYGON OBJECT 0 ----------------------------------------------------
	
// BEGIN POLYGON OBJECT 1 --------------------------------------------------
// Vertices for Polgon 1
Vertex vertices1[] =
{
	{ D3DXVECTOR3(-3.0f, 3.0f, 0.0f), D3DCOLOR_XRGB(  0,150,140) },
	{ D3DXVECTOR3(-1.0f, 2.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3( 0.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  0,255,  0) },
	{ D3DXVECTOR3( 1.0f, 2.0f, 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3( 3.0f, 3.0f, 0.0f), D3DCOLOR_XRGB(  0,255,  0) },
	{ D3DXVECTOR3(-2.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(  0,  0,190) },
	{ D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(  0,190,  0) },
	{ D3DXVECTOR3( 1.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3( 2.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3(-3.0f,-3.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3(-1.0f,-3.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3( 0.0f,-2.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3( 1.0f,-3.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3( 3.0f,-3.0f, 0.0f), D3DCOLOR_XRGB(255,  0,  0) }
};

WORD indices1[] = 
{
	 0,  1,  5,
	 5,  1,  2,
	 5,  2,  6,
	 6,  2, 11,
	11,  2,  7,
	 7,  2,  3,
	 7,  3,  8,
	 8,  3,  4,
	 9,  6, 11,
	 9, 11, 10,
	11,  7, 13,
	12, 11, 13
};


Vertex vertices2[] =
{
	{ D3DXVECTOR3(               0.0f  ,                0.0f  , 0.0f), D3DCOLOR_XRGB(255,  0,  0) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 0.0f ), sinf(D3DX_PI * 0.0f ), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 0.125f), sinf(D3DX_PI * 0.125f), 0.0f), D3DCOLOR_XRGB(  0,255,  0) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 0.25f ), sinf(D3DX_PI * 0.25f ), 0.0f), D3DCOLOR_XRGB(  130,150,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 0.375f ), sinf(D3DX_PI * 0.375f ), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 0.50f), sinf(D3DX_PI * 0.50f), 0.0f), D3DCOLOR_XRGB( 140,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 0.625f ), sinf(D3DX_PI * 0.625f ), 0.0f), D3DCOLOR_XRGB(255, 80,155) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 0.75f), sinf(D3DX_PI * 0.75f), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 0.875f), sinf(D3DX_PI * 0.875f), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 1.0f ), sinf(D3DX_PI * 1.0f ), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 1.125f), sinf(D3DX_PI * 1.125f), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 1.25f), sinf(D3DX_PI * 1.25f), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 1.375f), sinf(D3DX_PI * 1.375f), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 1.50f), sinf(D3DX_PI * 1.50f), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 1.625f), sinf(D3DX_PI * 1.625f), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 1.75f), sinf(D3DX_PI * 1.75f), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
	{ D3DXVECTOR3(cosf(D3DX_PI * 1.875f), sinf(D3DX_PI * 1.875f), 0.0f), D3DCOLOR_XRGB(  0,  0,255) },
};


WORD indices2[] =
{
	 0,  2,  1,
	 0,  3,  2,
	 0,  4,  3,
	 0,  5,  4,
	 0,  6,  5,
	 0,  7,  6,
	 0,  8,  7,
	 0,  9,  8,
	 0, 10,  9,
	 0, 11, 10,
	 0, 12, 11,
	 0, 13, 12,
	 0, 14, 13,
	 0, 15, 14,
	 0, 16, 15,
	 0,  1, 16
};

Vertex vertices3[] = 
{
	{ D3DXVECTOR3(-1.0f, 3.0f, 0.0f), D3DCOLOR_XRGB(  250,150,140) },
	{ D3DXVECTOR3( 1.0f, 3.0f, 0.0f), D3DCOLOR_XRGB(  140,150,140) },
	{ D3DXVECTOR3( 0.0f, 2.0f, 0.0f), D3DCOLOR_XRGB(  200,150,140) },
	{ D3DXVECTOR3(-3.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  100,150,140) },
	{ D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  50,150,140) },
	{ D3DXVECTOR3( 1.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  0, 0,140) },
	{ D3DXVECTOR3( 3.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  0, 0,255) },
	{ D3DXVECTOR3(-2.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(  0, 250,140) },
	{ D3DXVECTOR3( 2.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(  255, 0,140) },
	{ D3DXVECTOR3(-3.0f,-1.0f, 0.0f), D3DCOLOR_XRGB(  0, 0,140) },
	{ D3DXVECTOR3(-1.0f,-1.0f, 0.0f), D3DCOLOR_XRGB(  0, 0,140) },
	{ D3DXVECTOR3( 1.0f,-1.0f, 0.0f), D3DCOLOR_XRGB(  50,150,140) },
	{ D3DXVECTOR3( 3.0f,-1.0f, 0.0f), D3DCOLOR_XRGB(  100,150,140) },
	{ D3DXVECTOR3( 0.0f,-2.0f, 0.0f), D3DCOLOR_XRGB(  200,150,140) },
	{ D3DXVECTOR3(-1.0f,-3.0f, 0.0f), D3DCOLOR_XRGB(  140,150,140) },
	{ D3DXVECTOR3( 1.0f,-3.0f, 0.0f), D3DCOLOR_XRGB(  250,150,140) }
};

WORD indices3[] = 
{
	4, 0, 2,
	2, 1, 5,
	4, 2, 5,
	3, 4, 7,
	7, 4, 10,
	9, 7, 10,
	10, 4, 5,
	10, 5, 11,
	11, 5, 8,
	5, 6, 8,
	11, 8, 12,
	10, 11, 13,
	10, 13, 14,
	13, 11, 15

};

//16 VERTICES
Vertex vertices4[] = 
{
	{ D3DXVECTOR3( 0.0f, 4.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3(-3.0f, 2.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3(-1.0f, 2.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3( 1.0f, 2.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3( 3.0f, 2.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3(-1.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3( 1.0f, 1.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3(-2.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3( 2.0f, 0.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3(-1.0f,-1.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3( 1.0f,-1.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3(-1.0f,-2.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3( 1.0f,-2.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3(-3.0f,-3.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3( 3.0f,-3.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) },
	{ D3DXVECTOR3( 0.0f,-4.0f, 0.0f), D3DCOLOR_XRGB(  0, 255, 0) }
};

//14 FACES
WORD indices4[] = 
{
	2, 0, 3,
	1, 2, 5,
	5, 2, 3,
	5, 3, 6,
	6, 3, 4,
	7, 5, 9,
	9, 5, 6,
	9, 6, 10,
	10, 6, 8,
	11, 9, 10,
	11, 10, 12,
	13, 9, 11,
	12, 10, 14,
	11, 12, 15
};


/*v[0] = Vertex(-3 * SCALE, 3 * SCALE, 0, D3DCOLOR_XRGB(255, 0, 0));
	v[1] = Vertex(-1 * SCALE, 2 * SCALE, 0, D3DCOLOR_XRGB(0, 255, 0));
	v[2] = Vertex(  0, SCALE, 0, D3DCOLOR_XRGB(0, 0, 155));
	v[3] = Vertex( SCALE, 2 * SCALE, 0, D3DCOLOR_XRGB(255, 150, 0));
	v[4] = Vertex( 3 * SCALE, 3 * SCALE, 0, D3DCOLOR_XRGB(255, 0, 0));
	v[5] = Vertex(-2 * SCALE,  0, 0, D3DCOLOR_XRGB(255, 0, 0));
	v[6] = Vertex(-SCALE,-SCALE, 0, D3DCOLOR_XRGB(0, 255, 0));
	v[7] = Vertex( SCALE,-SCALE, 0, D3DCOLOR_XRGB(0, 0, 255));
	v[8] = Vertex( 2 * SCALE,  0, 0, D3DCOLOR_XRGB(255, 0, 0));


	v[9] = Vertex(-3 * SCALE,-3 * SCALE, 0, D3DCOLOR_XRGB(255, 0, 0));
	v[10] = Vertex(-SCALE,-3 * SCALE, 0, D3DCOLOR_XRGB(255, 0, 0));
	v[11] = Vertex(  0,-2 * SCALE, 0, D3DCOLOR_XRGB(255, 0, 0));
	v[12] = Vertex( SCALE,-3 * SCALE, 0, D3DCOLOR_XRGB(255, 0, 0));
	v[13] = Vertex( 3 * SCALE,-3 * SCALE, 0, D3DCOLOR_XRGB(255, 0, 0));*/

/*i[0] = 0; i[1] = 1; i[2] = 5;

	i[3] = 5; i[4] = 1; i[5] = 2;

	i[6] = 5; i[7] = 2; i[8] = 6;

	i[9] = 6; i[10] = 2; i[11] = 11;

	i[12] = 11; i[13] = 2; i[14] = 7;

	i[15] = 7; i[16] = 2; i[17] = 3;

	i[18] = 7; i[19] = 3; i[20] = 8;

	i[21] = 8; i[22] = 3; i[23] = 4;

	i[24] = 9; i[25] = 6; i[26] = 11;

	i[27] = 9; i[28] = 11; i[29] = 10;

	i[30] = 11; i[31] = 7; i[32] = 13;

	i[33] = 12; i[34] = 11; i[35] = 13;*/

#endif