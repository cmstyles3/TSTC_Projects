#ifndef ENGINE_H
#define ENGINE_H

#include <windows.h>

#include <d3dx9.h>
#include <string>
#include <vector>
#include "State.h"
#include "input.h"
#include "Sound.h"

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if (p) { delete  p ;   p = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] p ; p = NULL; } }
#define SAFE_RELEASE(p)      { if (p) { p->Release();   p = NULL; } }


struct EngineSetup
{
	HINSTANCE instance;
	char *name;
	int width;
	int height;
	bool windowed;

	void (*Init)();

	void (*Kill)();


	EngineSetup()
	{
		instance = NULL;
		name = "Application";	
		width = 800;
		height = 600;
		Init = NULL;
		Kill = NULL;
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

		Input* GetInput();

		SoundSystem *GetSoundSystem();

		void AddState(State *state, bool change = true);
		void RemoveState(State *state);
		void ChangeState(DWORD id);
		State *GetCurrentState();

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

		std::vector<State*> m_states;
		State *m_currentState;
		bool m_stateChanged;

};

extern Engine *g_engine;

#endif