#include "d3dx9.h"
#ifndef D3DBASE_H
#define D3DBASE_H

struct ColorVertex
{
	float x, y, z;
	int color;
	ColorVertex(){}
	ColorVertex(float X, float Y, float Z, int Color)
	{
		x = X, y = Y, z = Z, color = Color;
	}
};

#define D3DFVF_COLOR (D3DFVF_XYZ | D3DFVF_DIFFUSE)


int InitD3D(HINSTANCE hinstance, int width, int height, bool windowed, D3DDEVTYPE DeviceType, IDirect3DDevice9** Device);

int EnterMsgLoop(int (*ptr_gamemain)(float timeDelta));

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);



#endif

