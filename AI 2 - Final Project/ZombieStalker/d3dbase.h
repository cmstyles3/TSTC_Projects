
#ifndef D3DBASE_H
#define D3DBASE_H


#include <d3dx9.h>

// DEFINE ////////////////////////////////////////////////////////////////////////////////////////////////

// Define custom vertex format so that contains position, normal, and texture
#define D3DFVF_CUSTOM (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1) 
#define D3DFVF_COLOR (D3DFVF_XYZ | D3DFVF_DIFFUSE)



// FUNCTION PROTOTYPES ///////////////////////////////////////////////////////////////////////////////////////////////////

// Basic function to setup a window with Direct3D
int InitD3D(char *classname, char *windowname, int width, int height, bool windowed, D3DDEVTYPE DeviceType);
int KillD3D();

int EnterMsgLoop(int (*ptr_gamemain)(float timeDelta));
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

// Utility Light functions for setting up the 3 kinds of light sources (Directional, Point, and Spot).
D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color);
D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color);
D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color);

void SetFog(float start, float end, D3DCOLOR);

// Utility function for setting up simple material
D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p);

//some basic colors defined
const D3DXCOLOR      WHITE( D3DCOLOR_XRGB(255, 255, 255) );
const D3DXCOLOR      BLACK( D3DCOLOR_XRGB(  0,   0,   0) );
const D3DXCOLOR        RED( D3DCOLOR_XRGB(255,   0,   0) );
const D3DXCOLOR      GREEN( D3DCOLOR_XRGB(  0, 255,   0) );
const D3DXCOLOR       BLUE( D3DCOLOR_XRGB(  0,   0, 255) );
const D3DXCOLOR     YELLOW ( D3DCOLOR_XRGB(255, 255,   0) );
const D3DXCOLOR       CYAN( D3DCOLOR_XRGB(  0, 255, 255) );
const D3DXCOLOR    MAGENTA( D3DCOLOR_XRGB(255,   0, 255) );
const D3DXCOLOR     SILVER( D3DCOLOR_XRGB(216, 206, 215) );
const D3DXCOLOR      PINK( D3DCOLOR_XRGB(255, 192, 203) );
const D3DXCOLOR     WHEAT( D3DCOLOR_XRGB(245, 222, 179) );

//some materials defined
const D3DMATERIAL9 WHITE_MTRL  = InitMtrl(WHITE, WHITE, WHITE, BLACK, 2.0f);
const D3DMATERIAL9 RED_MTRL    = InitMtrl(RED, RED, RED, BLACK, 20.0f);
const D3DMATERIAL9 GREEN_MTRL  = InitMtrl(GREEN, GREEN, GREEN, BLACK, 2.0f);
const D3DMATERIAL9 BLUE_MTRL   = InitMtrl(BLUE, BLUE, BLUE, BLACK, 2.0f);
const D3DMATERIAL9 YELLOW_MTRL = InitMtrl(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);
const D3DMATERIAL9 SILVER_MTRL = InitMtrl(SILVER, SILVER, PINK, BLACK, 20.0f);
const D3DMATERIAL9 BLACK_MTRL = InitMtrl(BLACK, BLACK, SILVER, BLACK, 20.0f);
const D3DMATERIAL9 BULLET_MTRL = InitMtrl(GREEN, GREEN, GREEN, GREEN, 100.0f);

extern LPDIRECT3DDEVICE9 Device;

#endif

