#ifndef _PTPMULTI__H
#define _PTPMULTI__H

#include <windows.h>
#include <dplay8.h>
#include <stdio.h>
#include <d3dx9.h>
#include "dxutil/dxutil.h"

#define TIMERID_CONNECT_COMPLETE    1

#define MAX_BULLETS 1000

//GUID
extern GUID PTP;

//Dplay objects

extern IDirectPlay8Peer*       g_pDP;
extern IDirectPlay8Address*    g_pDeviceAddress;
extern IDirectPlay8Address*    g_pHostAddress;
extern DPNHANDLE				g_hConnectAsyncOp;
extern DPNID					g_dpnidLocalPlayer;

#define MAX_PLAYERS		6

struct PLAYER_INFORMATION
{
	bool	bActive;
    DPNID	dpnidPlayer;
    char	szPlayerName[32];
	D3DXMATRIX matrixMagic;
	float x, y, z;
	int ship;
};

extern PLAYER_INFORMATION	PlayerInfo[MAX_PLAYERS];

//other variables

extern HANDLE                  g_hConnectCompleteEvent;
extern HRESULT					g_hrConnectComplete;
extern CRITICAL_SECTION        g_csModifyPlayer;

extern LONG	g_lNumberOfActivePlayers;
extern BOOL	bHost;


#define PACKET_TYPE_GENERIC 0
#define PACKET_TYPE_MAGICMATRIX 1
#define PACKET_TYPE_SHIP 2
#define PACKET_TYPE_MAP 3
#define PACKET_TYPE_BULLET 4
#define PACKET_TYPE_POSITION 5

//packet struct

struct BULLETS
{
	DPNID dpOwner;
	D3DXMATRIX R;
	bool bActive;
	float x, y, z;
	float xv, yv, zv;
	float life;
	int type;
};

class GENERIC_PACKET
{
	public:
		int type;
		int size;
};

class MAGICMATRIX_PACKET : public GENERIC_PACKET
{
	public:
		D3DXMATRIX magicMatrix;
};

class SHIP_PACKET : public GENERIC_PACKET
{
	public:
		int ship;
};

class BULLET_PACKET : public GENERIC_PACKET
{
	public:
		BULLETS bullet;
};

class POSITION_PACKET : public GENERIC_PACKET
{
	public:
		float x, y, z;
};


extern BULLETS Bullets[MAX_BULLETS];
extern D3DXMATRIX bulletRot;


//Functions

HRESULT InitDirectPlay( HWND hwnd );
void vCleanup(void);
HRESULT WINAPI DirectPlayMessageHandler( PVOID pvUserContext, DWORD dwMessageId, PVOID pMsgBuffer );
HRESULT	SendPeerMessage(int player, PVOID pMsgBuffer);
HRESULT	HostGame( HWND hwnd );
HRESULT	JoinGame( HWND hwnd );
HRESULT	CreatePlayer( PVOID pvUserContext, PVOID pMsgBuffer );
HRESULT	DestroyPlayer( PVOID pvUserContext, PVOID pMsgBuffer );


int GetPlayerID(DPNID dpid);

void CreateBullet(DPNID dpowner, D3DXMATRIX R, float x, float y, float z, float xv, float yv, float zv);

#endif