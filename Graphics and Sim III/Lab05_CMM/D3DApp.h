#ifndef D3DAPP_H
#define D3DAPP_H

#include <windows.h>

#include <d3dx9.h>
#include <string>
#include "input.h"


#define SAFE_RELEASE(x) if (x) { x->Release(); x = 0; }

struct Setup
{
	HINSTANCE instance;
	char appName[80];
	int width;
	int height;
	bool windowed;
};

class D3DApp
{
	public:
		D3DApp(Setup *setup);

		D3DApp(HINSTANCE hInstance,  std::string caption, D3DDEVTYPE devType, DWORD requestedVP,
			int width = 800, int height = 600);
		virtual ~D3DApp();

		HINSTANCE GetInstance();
		HWND GetWindow();

		LPDIRECT3DDEVICE9 GetDevice();

		// FRAME WORK FUNCTIONS
		virtual bool CheckDeviceCaps() { return true; }
		virtual void OnLostDevice() {}
		virtual void OnResetDevice() {}
		virtual void Update(float dt) = 0;
		virtual void Draw(float dt) = 0;

	//	virtual void InitWindow();
	//	virtual void InitD3D();
		int run();
		virtual LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);

		bool DeviceLost();

		int GetWidth();
		int GetHeight();

		Input* GetInput();

	private:
		std::string m_winCaption;
		D3DDEVTYPE m_devType;
		DWORD m_requestedVP;

		HINSTANCE m_instance;
		HWND m_window;
		LPDIRECT3D9 m_d3d;
		LPDIRECT3DDEVICE9 m_d3dDevice;
		D3DPRESENT_PARAMETERS m_d3dpp;

		int m_width, m_height;

		bool m_appPaused;

		Input *m_input;
		
};

extern D3DApp *g_app;
extern LPDIRECT3DDEVICE9 g_device;

#endif