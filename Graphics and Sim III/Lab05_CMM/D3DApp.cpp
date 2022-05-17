#include "d3dapp.h"

#define SAFE_RELEASE(x) if (x) { x->Release(); x = 0; }

D3DApp *g_app = 0;
LPDIRECT3DDEVICE9 g_device = 0;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Don't start processing messages until the application has been created
	if (g_app != 0)
		return (g_app->MsgProc(msg, wParam, lParam));

	return (DefWindowProc(hWnd, msg, wParam, lParam));
}

D3DApp::D3DApp(Setup *setup)
{
	srand(GetTickCount());

	m_instance = setup->instance;
	
	ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	//Initialize Window

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = m_instance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "ClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, "Register Class Failed", 0, 0);
		PostQuitMessage(0);
	}

	int width  = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);


	DWORD style;
	if (setup->windowed)
		style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
	else style = WS_POPUP;

	m_window = CreateWindow("ClassName", setup->appName, style, 
		0, 0, 
		setup->width, setup->height, 0, 0, m_instance, 0);

	if (!m_window)
	{
		MessageBox(0, "CreateWindow Failed", 0, 0);
		PostQuitMessage(0);
	}

	ShowWindow(m_window, SW_SHOW);
	UpdateWindow(m_window);

	// create d3d object

	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_d3d)
	{
		MessageBox(0, "Direct3DCreate9 Failed", 0, 0);
		PostQuitMessage(0);
	}


	// check for requested  vertex processing
	D3DCAPS9 caps;
	m_d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

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
	m_d3dpp.BackBufferWidth            = setup->width;
	m_d3dpp.BackBufferHeight           = setup->height;
	m_d3dpp.BackBufferFormat           = D3DFMT_X8R8G8B8;
	m_d3dpp.BackBufferCount            = 1;
	m_d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	m_d3dpp.MultiSampleQuality         = 0;
	m_d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.hDeviceWindow              = m_window;
	m_d3dpp.Windowed                   = setup->windowed;
	m_d3dpp.EnableAutoDepthStencil     = true;
	m_d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	m_d3dpp.Flags                      = 0;
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(m_d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, devBehaviorFlags, &m_d3dpp, &g_device)))
	{
		MessageBox(0, "this sucks", 0, 0);
	}

	m_input = new Input(m_window);
	m_appPaused = false;

}
//
//D3DApp::D3DApp(HINSTANCE hInstance, std::string caption, D3DDEVTYPE devType, DWORD requestedVP, int width, int height)
//{
//	srand(GetTickCount());
//	m_winCaption  = caption;
//	m_devType     = devType;
//	m_requestedVP = requestedVP;
//
//	m_instance   = hInstance;
//	//m_hWnd = 0;
//	m_d3d = 0;
//	m_d3dDevice = 0;
//	m_appPaused = false;
//	m_width = width;
//	m_height = height;
//	ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
//
//	
//	InitWindow();
//	
//	InitD3D();
//
//	//m_input = new Input(m_hWnd);
//	
//}

D3DApp::~D3DApp()
{
	SAFE_RELEASE(m_d3d);
	SAFE_RELEASE(g_device);
}

HINSTANCE D3DApp::GetInstance()
{
	return (m_instance);
}

HWND D3DApp::GetWindow()
{
	return (m_window);
}

LPDIRECT3DDEVICE9 D3DApp::GetDevice()
{
	return (m_d3dDevice);
}

//void D3DApp::InitWindow()
//{
//	WNDCLASS wc;
//	ZeroMemory(&wc, sizeof(WNDCLASS));
//	wc.style = CS_HREDRAW | CS_VREDRAW;
//	wc.lpfnWndProc = WindowProc;
//	wc.hInstance = m_hInstance;
//	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
//	wc.hCursor = LoadCursor(0, IDC_ARROW);
//	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
//	wc.lpszClassName = "D3DAPP";
//
//	if (!RegisterClass(&wc))
//	{
//		MessageBox(0, "Register Class Failed", 0, 0);
//		PostQuitMessage(0);
//	}
//
//	int width  = GetSystemMetrics(SM_CXSCREEN);
//	int height = GetSystemMetrics(SM_CYSCREEN);
//
//	m_hWnd = CreateWindow("D3DAPP", m_winCaption.c_str(), WS_POPUP, 
//		width/2 - m_width/2, height/2 - m_height/2, 
//		m_width, m_height, 0, 0, m_hInstance, 0);
//
//	if (!m_hWnd)
//	{
//		MessageBox(0, "CreateWindow Failed", 0, 0);
//		PostQuitMessage(0);
//	}
//
//	ShowWindow(m_hWnd, SW_SHOW);
//	UpdateWindow(m_hWnd);
//}
//
//void D3DApp::InitD3D()
//{
//	// create d3d object
//
//	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);
//	if (!m_d3d)
//	{
//		MessageBox(0, "Direct3DCreate9 Failed", 0, 0);
//		PostQuitMessage(0);
//	}
//
//
//	// check for requested  vertex processing
//	D3DCAPS9 caps;
//	m_d3d->GetDeviceCaps(D3DADAPTER_DEFAULT, m_devType, &caps);
//
//	DWORD devBehaviorFlags = 0;
//
//	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
//		devBehaviorFlags  |= m_requestedVP;
//	else
//		devBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
//
//	// If pure device and HW T&L supported
//	if( caps.DevCaps & D3DDEVCAPS_PUREDEVICE &&
//		devBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
//			devBehaviorFlags |= D3DCREATE_PUREDEVICE;
//
//	// Fill out D3DPRESENT_PARAMETERS structure
//
//	ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
//	m_d3dpp.BackBufferWidth            = m_width;
//	m_d3dpp.BackBufferHeight           = m_height;
//	m_d3dpp.BackBufferFormat           = D3DFMT_X8R8G8B8;
//	m_d3dpp.BackBufferCount            = 1;
//	m_d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
//	m_d3dpp.MultiSampleQuality         = 0;
//	m_d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
//	m_d3dpp.hDeviceWindow              = m_window;
//	m_d3dpp.Windowed                   = true;
//	m_d3dpp.EnableAutoDepthStencil     = true;
//	m_d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
//	m_d3dpp.Flags                      = 0;
//	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
//	m_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
//
//	if (FAILED(m_d3d->CreateDevice(D3DADAPTER_DEFAULT, m_devType, m_window, devBehaviorFlags, &m_d3dpp, &m_d3dDevice)))
//	{
//		//MessageBox(0, "this sucks", 0, 0);
//	}
//
//}

int D3DApp::run()
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
			__int64 currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
				
			float dt = (currTimeStamp - prevTimeStamp)*secsPerCnt;

			if (m_appPaused)
			{
				
				Sleep(20);
				continue;
			}

			m_input->Update();
			
			if (!DeviceLost())
			{
				Update(dt);
				Draw(dt);
			}

			prevTimeStamp = currTimeStamp;				
			
		}
	}
	return((int)msg.wParam);
}

LRESULT D3DApp::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_ACTIVATE:
			if (LOWORD(wParam) == WA_INACTIVE)
				m_appPaused = true;
			else m_appPaused = false;
			return 0;

		case WM_CLOSE:
			DestroyWindow(m_window);
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return(0);
		//case WM_SIZE:
		//	
		///*	m_width = m_d3dpp.BackBufferWidth  = LOWORD(lParam);
		//	m_height = m_d3dpp.BackBufferHeight = HIWORD(lParam);
		//	OnLostDevice();
		//	m_d3dDevice->Reset(&m_d3dpp);
		//	OnResetDevice();*/
		//	return(0);
		
		case WM_KEYDOWN:
			/*if (LOWORD(wParam) == 0x27)
			{
				PostQuitMessage(0);
			}*/
			/*if (LOWORD(wParam) == 0x46)
			{
				if (m_d3dpp.Windowed)
				{
					SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_POPUP);
					m_d3dpp.BackBufferWidth = m_width = 800;
					m_d3dpp.BackBufferHeight = m_height = 600;
					m_d3dpp.Windowed = false;
					SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 800, 600, SWP_NOZORDER | SWP_SHOWWINDOW);	
				}
				else 
				{
					RECT R = {0, 0, 800, 600};
					AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
					
					m_d3dpp.BackBufferWidth = m_width =  800;
					m_d3dpp.BackBufferHeight = m_height = 600;
					m_d3dpp.Windowed = true;

					SetWindowLongPtr(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
					SetWindowPos(m_hWnd, HWND_TOP, 100, 100, 800, 600, SWP_NOZORDER | SWP_SHOWWINDOW);
					
				}*/
				/*OnLostDevice();
				m_d3dDevice->Reset(&m_d3dpp);
				OnResetDevice();
			}*/
		
			break;
	}
	return(DefWindowProc(m_window, msg, wParam, lParam));
}

bool D3DApp::DeviceLost()
{
	HRESULT hr = g_device->TestCooperativeLevel();

	if (hr == D3DERR_DEVICELOST)
	{
		Sleep(20);
		return true;
	}
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		OnLostDevice();
		g_device->Reset(&m_d3dpp);
		OnResetDevice();
		return false;
	}

	return false;
}


int D3DApp::GetWidth()
{
	return m_width;
}

int D3DApp::GetHeight()
{
	return m_height;
}


Input* D3DApp::GetInput()
{
	return m_input;
}