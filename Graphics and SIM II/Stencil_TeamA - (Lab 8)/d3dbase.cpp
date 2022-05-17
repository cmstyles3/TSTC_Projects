#include "d3dbase.h"
#include "resource.h"

extern HWND hwnd_app;

int InitD3D(HINSTANCE hinstance, int width, int height, bool windowed, D3DDEVTYPE Devicetype, LPDIRECT3DDEVICE9* Device)
{
	HWND hwnd;
	WNDCLASS wc;

	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WindowProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hinstance;
	wc.hIcon         = LoadIcon(hinstance, MAKEINTRESOURCE(ICON_ID));
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = 0;
	wc.lpszClassName = "Project 8";


	if( !RegisterClass(&wc) ) 
	{
		MessageBox(0, "RegisterClass() - FAILED", 0, 0);
		return(0);
	}
		
	if (!(hwnd = CreateWindow("Project 8", "Stencil_TeamA", WS_EX_TOPMOST,	0, 0, width, height, 0 , 0, hinstance, 0)))
	{
		MessageBox(0, "CreateWindow() - FAILED", 0, 0);
		return(0);
	}
	hwnd_app = hwnd;
	ShowCursor(0);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	LPDIRECT3D9 d3d9;
	d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT,Devicetype, &caps);

	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.Windowed = windowed;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, Devicetype, hwnd, vp, &d3dpp, Device)))
	{
		MessageBox(0, "CreateDevice() - FAILED", 0, 0);
		return(0);
	}

	return(1);
}

int EnterMsgLoop(int (*ptr_gamemain)(float timedelta))
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	static float lasttime = (float)timeGetTime(); 

	while(1)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		float currtime  = (float)timeGetTime();
		float timedelta = (currtime - lasttime) * 0.001f;

		ptr_gamemain(timedelta);

		lasttime = currtime;
	}
    return msg.wParam;
	
}

D3DLIGHT9 InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = *color * 0.8f;
	light.Diffuse   = *color ;
	light.Specular  = *color * 0.0f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = *color * 0.6f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color)
{
	D3DLIGHT9 light;
	ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.4f;
	light.Position  = *position;
	light.Direction = *direction;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.6f;
	light.Phi          = 0.9f;

	return light;
}

D3DMATERIAL9 InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}

void CreateTexPlane(LPDIRECT3DDEVICE9 device, float width, float height, LPD3DXMESH *mesh)
{
	D3DXCreateMeshFVF(2, 4, 0, D3DFVF_CUSTOM, device, mesh);
	
	Vertex *v;
	(*mesh)->LockVertexBuffer(0,(void**)&v);
	//front face
	v[0] = Vertex(-width,-height, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-width, height, 0.0f, 0.0f, 0.0f,-1.0f, 0.0f, 0.0f);
	v[2] = Vertex( width, height, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 0.0f);
	v[3] = Vertex( width,-height, 0.0f, 0.0f, 0.0f,-1.0f, 1.0f, 1.0f);
	(*mesh)->UnlockVertexBuffer();

	WORD *i;
	(*mesh)->LockIndexBuffer(0,(void**)&i);
	
	i[0] = 0; i[1] = 1; i[2] = 2; //first tri of face
	i[3] = 0; i[4] = 2; i[5] = 3; //second tri of face
	
	(*mesh)->UnlockIndexBuffer();
}