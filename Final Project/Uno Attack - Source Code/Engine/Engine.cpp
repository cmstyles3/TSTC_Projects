// ENGINE.CPP

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "engine.h"


Engine *g_engine = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		case WM_ACTIVATE:
			if (LOWORD(wparam) == WA_INACTIVE)
				g_engine->SetAppPaused(true);
			else g_engine->SetAppPaused(false);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	// Process messages used in the current state
	if (g_engine->IsLoaded() == true &&
		g_engine->GetCurrentState() != NULL)
	{
		g_engine->GetCurrentState()->MsgProc(msg, wparam, lparam);
	}

	return (DefWindowProc(hwnd, msg, wparam, lparam));
}

Engine::Engine(EngineSetup *setup)
{
	m_loaded = false;

	srand(GetTickCount());

	m_setup = new EngineSetup;
	if (setup != NULL)
		memcpy(m_setup, setup, sizeof(EngineSetup));

	g_engine = this;

	//Initialize Window
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = m_setup->instance;
	wc.hIcon = LoadIcon(m_setup->instance, "unoattackicon");
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "WindowClass";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, "Register Class Failed", 0, 0);
		PostQuitMessage(0);
	}

	// 
	//CoInitializeEx(NULL, COINIT_MULTITHREADED);
	CoInitialize(0);

	DWORD style;
	if (m_setup->windowed)
		style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	else style = WS_POPUP;

	m_window = CreateWindow("WindowClass", m_setup->name, style, 
		0, 0, 
		m_setup->width, m_setup->height, 0, 0, m_setup->instance, 0);

	if (!m_window)
	{
		MessageBox(0, "CreateWindow Failed", 0, 0);
		PostQuitMessage(0);
	}


	// create d3d object

	LPDIRECT3D9 d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d9)
	{
		MessageBox(0, "Direct3DCreate9 Failed", 0, 0);
		PostQuitMessage(0);
	}


	// check for requested  vertex processing
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	DWORD devBehaviorFlags = 0;

	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		devBehaviorFlags  |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		devBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	// If pure device and HW T&L supported
	if( caps.DevCaps & D3DDEVCAPS_PUREDEVICE &&
		devBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
			devBehaviorFlags |= D3DCREATE_PUREDEVICE;

	// Fill out D3DPRESENT_PARAMETERS structure

	ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	m_d3dpp.BackBufferWidth            = m_setup->width;
	m_d3dpp.BackBufferHeight           = m_setup->height;
	m_d3dpp.BackBufferFormat           = D3DFMT_X8R8G8B8;
	m_d3dpp.BackBufferCount            = 1;
	m_d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	m_d3dpp.MultiSampleQuality         = 0;
	m_d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.hDeviceWindow              = m_window;
	m_d3dpp.Windowed                   = m_setup->windowed;
	m_d3dpp.EnableAutoDepthStencil     = true;
	m_d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	m_d3dpp.Flags                      = 0;
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, devBehaviorFlags | D3DCREATE_MULTITHREADED , &m_d3dpp, &m_device)))
	{
		MessageBox(0, "Can't Create Device", 0, 0);
		PostQuitMessage(0);
	}

	// Create sprite object
	D3DXCreateSprite(m_device, &m_sprite);	
	
	// Initialize input
	m_input = new Input(m_window);

	// Initialize sound system
	m_soundSystem = new SoundSystem;

	m_network = new Network(m_setup->guid, m_setup->port, m_setup->NetworkMessageHandler);

	// Clear State vector and current state
	m_states.clear();
	m_currentState = NULL;

	// Allow the application to perform any state setup now.
	if( m_setup->Init != NULL )
		m_setup->Init();
	
	m_appPaused = false;

	ShowCursor(false);
	ShowWindow(m_window, SW_SHOW);
	UpdateWindow(m_window);

	m_loaded = true;

}

Engine::~Engine()
{
	if (m_setup->Kill != NULL)
		m_setup->Kill();

	SAFE_DELETE(m_network);
	SAFE_DELETE(m_soundSystem);
	SAFE_DELETE(m_input);
	SAFE_RELEASE(m_sprite);
	SAFE_RELEASE(m_device);

	SAFE_DELETE(m_setup);

	
	UnregisterClass("WindowClass", 0);

	CoUninitialize();
}


void Engine::Run()
{
	if (m_loaded)
	{
		MSG msg;

		__int64 cntsPerSec = 0;
		QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
		float secsPerCnt = 1.0f / (float)cntsPerSec;

		__int64 prevTimeStamp = 0;
		QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

		while(1)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				if (m_appPaused)
				{
					
					Sleep(20);
					continue;
				}

				/*__int64 currTimeStamp = 0;
				QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
					
				float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;*/
				static float lastTime = (float)timeGetTime();
				float currTime = (float)timeGetTime();
				float dt = (currTime - lastTime)*0.001f;

				// UPDATE NETWORK
				m_network->Update();

				// UPDATE INPUT
				m_input->Update();

					
				if (!DeviceLost())
				{	
					if (m_currentState != NULL)
						m_currentState->Update(dt);

					if (m_currentState != NULL)
						GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_currentState->GetColor(), 1.0f, 0);

					else GetDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);

					GetDevice()->BeginScene();

					if (m_currentState != NULL)
						m_currentState->Draw();

					GetDevice()->EndScene();

					GetDevice()->Present(0,0,0,0);
	
				}

				lastTime = currTime;			
				
			}
		}
	}
	SAFE_DELETE(g_engine);
}

bool Engine::DeviceLost()
{
	HRESULT hr = m_device->TestCooperativeLevel();

	if (hr == D3DERR_DEVICELOST)
	{
		Sleep(20);
		return true;
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		
		m_sprite->OnLostDevice();

		if (m_setup->OnLostDevice != NULL)
			m_setup->OnLostDevice();

		if (m_currentState != NULL)
			m_currentState->OnLostDevice();

		m_device->Reset(&m_d3dpp);
		
		m_sprite->OnResetDevice();

		if (m_setup->OnResetDevice != NULL)
			m_setup->OnResetDevice();

		if (m_currentState != NULL)
			m_currentState->OnResetDevice();

		return false;
	}

	return false;
}

HINSTANCE Engine::GetInstance()
{
	return (m_setup->instance);
}

HWND Engine::GetWindow()
{
	return (m_window);
}

LPDIRECT3DDEVICE9 Engine::GetDevice()
{
	return (m_device);
}

LPD3DXSPRITE Engine::GetSprite()
{
	return m_sprite;
}

int Engine::GetWidth()
{
	return m_setup->width;
}

int Engine::GetHeight()
{
	return m_setup->height;
}

bool Engine::IsLoaded()
{
	return m_loaded;
}

Input* Engine::GetInput()
{
	return m_input;
}

SoundSystem *Engine::GetSoundSystem()
{
	return m_soundSystem;
}

Network *Engine::GetNetwork()
{
	return m_network;
}

void Engine::AddState(State *state, bool change)
{
	m_states.push_back(state);
	if (change == false)
		return;

	if (m_currentState != NULL)
		m_currentState->Close();

	m_currentState = m_states.back();
	m_currentState->Load();
}

void Engine::RemoveState(State *state)
{
	for (int i = 0; i < m_states.size(); i++)
	{
		if (state == m_states[i])
		{
			m_states[i] = m_states[m_states.size()-1];
			m_states.pop_back();
		}
	}

}

void Engine::ChangeState(DWORD id)
{
	for (int i = 0; i < m_states.size(); i++)
	{
		if (m_states[i]->GetID() == id)
		{
			if (m_currentState != NULL)
				m_currentState->Close();

			m_currentState = m_states[i];
			m_currentState->Load();

			m_stateChanged = true;

			break;
		}
	}
}

State *Engine::GetCurrentState()
{
	return m_currentState;
}

void Engine::SetAppPaused(bool appPaused)
{
	m_appPaused = appPaused;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////