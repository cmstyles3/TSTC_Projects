// MULTIPLAYERGAME.H

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MAIN_H
#define MAIN_H

#include "../Engine/Engine.h"

// Game Includes
#include "Loading.h"
#include "MainMenu.h"
#include "TimeTrial.h"
#include "Multiplayer.h"
#include "EditTrack.h"

//#include "shipselection.h"
//#include "multiplayergame.h"
//#include "editmap.h"

// GAME STATES
enum 
{
	LOADING_STATE, 
	MAINMENU_STATE, 
	TIMETRIAL_STATE,
	MULTIPLAYER_STATE,
	EDITTRACK_STATE
};

// DEFINES FOR SHIP CONTROL ---------
#define ROLL_SPEED 2.0f            //
#define PITCH_SPEED 1.2f           //
#define YAW_SPEED 0.5f             //
                                   //
#define MAX_SHIP_SPEED    10000.0f  //
#define CRUISE_SHIP_SPEED 4000.0f  //
#define MIN_SHIP_SPEED    2000.0f  //
                                   //
#define SHIP_ACCEL 1000.0f         //
#define SHIP_DECEL 1200.0f         //
#define CRUISE_ACCEL_DECEL 500.0f //
// ----------------------------------


enum 
{
	CRUISE_THROTTLE, 
	SPEEDUP_THROTTLE, 
	SLOWDOWN_THROTTLE
};

#define NUM_SHIPS 6
#define NUM_TRACKS 3


struct PlayerData
{
	int ship;
	int track;
};

struct Track
{
	//Terrain terrain;
	int terrainId;
	std::vector<Object3D> rings;
	D3DCOLOR color;
};

void LoadingThread();
void SetGlobalD3DOptions();

void LoadTrack(Track *track, char *filename);
void SaveTrack(Track track, char *filename);

// ENGINE FUNCTIONS
void Init();
void Kill();
void OnLostDevice();
void OnResetDevice();
void NetworkMessageHandler(ReceivedMessage *msg);

//MAP GLOBALS
extern int g_currTrack;
extern Track g_track[NUM_TRACKS];

extern Sound *g_music;
extern Sound *g_sounds[10];

extern Terrain g_terrain[2];

enum {XWING, SWEEPWING};
extern int g_currShip;
extern Mesh g_shipMesh[NUM_SHIPS];
extern Mesh g_ringMesh;
extern Mesh g_arrowMesh;

extern bool ready;

extern Font g_font;
extern Font g_smallFont;
extern char output[80];
extern float percentLoaded;

extern D3DXVECTOR3 g_cursorPosition;

#endif

