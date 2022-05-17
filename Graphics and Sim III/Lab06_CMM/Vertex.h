#ifndef VERTEX_H
#define VERTEX_H

#include <d3dx9.h>

#define D3DFVF_PD (D3DFVF_XYZ | D3DFVF_DIFFUSE)
#define D3DFVF_PT (D3DFVF_XYZ | D3DFVF_TEX1)
#define D3DFVF_PNT (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

struct VertexPD
{
	float x, y, z;
	DWORD color;

	VertexPD(
		float x, float y, float z,
		DWORD color)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->color = color;
	}
};

struct VertexPT
{
	float x, y, z;
	float u, v;

	VertexPT(
		float x, float y, float z, 
		float u, float v)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->u = u;
		this->v = v;
	}
};

struct VertexPNT
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;

	VertexPNT(
		float x, float y, float z, 
		float nx, float ny, float nz, 
		float u, float v)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->nx = nx;
		this->ny = ny;
		this->nz = nz;
		this->u = u;
		this->v = v;
	}
};

#endif
