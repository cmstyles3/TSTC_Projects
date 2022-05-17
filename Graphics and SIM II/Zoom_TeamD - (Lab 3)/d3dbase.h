#ifndef D3DBASE_H
#define D3DBASE_H

#include "d3dx9.h"

int InitD3D(HINSTANCE hinstance, int width, int height, bool windowed, D3DDEVTYPE DeviceType, IDirect3DDevice9** Device);

int EnterMsgLoop(int (*ptr_gamemain)(float timeDelta));

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

extern HWND g_hwnd;

#endif

