// INPUT : Uses DirectInput for Keyboard and Mouse devices

#include <dinput.h>
#include "CInput.h"

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;


CInput::CInput()
{
	m_pDI = 0;
	m_pKBDev = 0;
	m_pMouseDev = 0;

	InitDI();
}

CInput::~CInput()
{
	KillKB();
	KillMouse();
	KillDI();
}

bool CInput::InitDI()
{
	if (FAILED(DirectInput8Create(g_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&m_pDI, NULL)))
		return false;

	return true;
}

void CInput::KillDI()
{
	if (m_pDI)
	{
		m_pDI->Release();
		m_pDI = 0;
	}
}

bool CInput::InitKB()
{
	if (m_pDI->CreateDevice(GUID_SysKeyboard, &m_pKBDev, NULL) != DI_OK)
		return false;

	if (m_pKBDev->SetDataFormat(&c_dfDIKeyboard) != DI_OK)
		return false;

	if (m_pKBDev->SetCooperativeLevel(g_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND) != DI_OK)
		return false;

	if (m_pKBDev->Acquire() != DI_OK)
		return false;

	return true;
}
	
  
void CInput::KillKB()
{
	//KILL_DEVICE(m_pKBDev);

	if (m_pKBDev) 
	{ 
		m_pKBDev->Unacquire(); 
        m_pKBDev->Release();
        m_pKBDev = NULL; 
    }
}

bool CInput::ReadKB()
{
	if (m_pKBDev)
    {
		memcpy(m_PrevKBState, m_KBState, 256);

		if (m_pKBDev->GetDeviceState(256, (LPVOID)m_KBState) != DI_OK)
			return false;
    }
	else
    {
		memset(m_KBState,0,sizeof(m_KBState));
		return false;
    }

	return true;
}

bool CInput::IsKeyDown(int key)
{
	if (m_KBState[key] & 0x80)
		return true;
	return false;
}

bool CInput::WasKeyDown(int key)
{
	if (m_PrevKBState[key] & 0x80)
		return true;
	return false;
}

bool CInput::InitMouse()
{
	if (m_pDI->CreateDevice(GUID_SysMouse, &m_pMouseDev, NULL) != DI_OK)
		return false;

	if (m_pMouseDev->SetDataFormat(&c_dfDIMouse) != DI_OK)
		return false;

	if (m_pMouseDev->SetCooperativeLevel(g_hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND) != DI_OK)
		return false;

	if (m_pMouseDev->Acquire() != DI_OK)
		return false;
	
	return true;
}

void CInput::KillMouse()
{
	if (m_pMouseDev) 
	{ 
		m_pMouseDev->Unacquire(); 
        m_pMouseDev->Release();
        m_pMouseDev = 0; 
    } 
}

bool CInput::ReadMouse()
{
	if (m_pMouseDev)    
    {
		memcpy(&m_PrevMouseState, &m_MouseState, sizeof(DIMOUSESTATE));
		//prev_mouse_state.rgbButtons[0] = mouse_state.rgbButtons[0];
		if (m_pMouseDev->GetDeviceState(sizeof(DIMOUSESTATE),(LPVOID)&m_MouseState) != DI_OK)
			return false;
    }
	else
    {
		memset(&m_MouseState, 0, sizeof(m_MouseState));
		return false;
    }

	return true;
}

bool CInput::IsButtonDown(int button)
{
	if (m_MouseState.rgbButtons[button] & 0x80)
		return true;
	return false;
}

bool CInput::WasButtonDown(int button)
{
	if (m_PrevMouseState.rgbButtons[button] & 0x80)
		return true;
	return false;
}