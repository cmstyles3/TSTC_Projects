// ENGINE.H

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef ENGINE_H
#define ENGINE_H

#define DIRECTINPUT_VERSION 0x800

#include <windows.h>
#include <d3dx9.h>
#include <dinput.h>
#include <dplay8.h>
#include <dmusici.h>
#include <string>
#include <vector>


#define SAFE_DELETE(p)       { if (p) { delete  p ;   p = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] p ; p = NULL; } }
#define SAFE_RELEASE(p)      { if (p) { p->Release();   p = NULL; } }

// Engine Includes
#include "Vertex.h"
#include "Network.h"
#include "State.h"
#include "Input.h"
#include "Sound.h"
#include "Font.h"
#include "Camera.h"
#include "Terrain.h"

#include "Mesh.h"
#include "Object3D.h"
#include "D3DUtility.h"


struct EngineSetup
{
	HINSTANCE instance;
	char *name;
	int width;
	int height;
	bool windowed;
	GUID guid;

	DWORD clearColor;


	void (*Init)();
	void (*Kill)();

	void (*OnLostDevice)();
	void (*OnResetDevice)();

	void (*Update)(float);
	void (*Draw)();

	void (*NetworkMessageHandler)(ReceivedMessage *msg);


	EngineSetup()
	{
		instance = NULL;
		name = "Application";	
		width = 800;
		height = 600;
		ZeroMemory(&guid, sizeof(GUID));

		clearColor = 0xff000000;

		Init = NULL;
		Kill = NULL;

		OnLostDevice = NULL;
		OnResetDevice = NULL;

		Update = NULL;
		Draw = NULL;

		NetworkMessageHandler = NULL;
		windowed = false;
	}
};

class Engine
{
	public:
		Engine(EngineSetup *setup);

		virtual ~Engine();

		HINSTANCE GetInstance();
		HWND GetWindow();

		LPDIRECT3DDEVICE9 GetDevice();

		LPD3DXSPRITE GetSprite();

		void Run();

		int GetWidth();
		int GetHeight();

		bool IsLoaded();

		Input* GetInput();

		SoundSystem *GetSoundSystem();

		Network *GetNetwork();

		void AddState(State *state, bool change = true);
		void RemoveState(State *state);
		void ChangeState(DWORD id);
		State *GetCurrentState();

		void SetAppPaused(bool appPaused);

	private:
		bool DeviceLost();

		HWND m_window;

		LPDIRECT3DDEVICE9 m_device;

		LPD3DXSPRITE m_sprite;

		D3DPRESENT_PARAMETERS m_d3dpp;

		bool m_appPaused;

		bool m_loaded;
		
		EngineSetup *m_setup;

		Input *m_input;

		SoundSystem *m_soundSystem;

		Network *m_network;

		std::vector<State*> m_states;
		State *m_currentState;
		bool m_stateChanged;

};

extern Engine *g_engine;

#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////