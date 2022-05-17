#include "Input.h"

#define SAFE_RELEASE(x) if (x) { x->Release(); x = 0; }

Input::Input(HWND window)
{
	m_window = window;

	DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_di, NULL);

	// Setup keyboard device
	m_di->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	m_keyboard->SetCooperativeLevel(m_window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_keyboard->Acquire();

	// Setup mouse device
	m_di->CreateDevice(GUID_SysKeyboard, &m_mouse, NULL);
	m_mouse->SetDataFormat(&c_dfDIMouse);
	m_mouse->SetCooperativeLevel(m_window, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	m_mouse->Acquire();
}

Input::~Input()
{
	SAFE_RELEASE(m_di);
	SAFE_RELEASE(m_keyboard);
	SAFE_RELEASE(m_mouse);
}

void Input::Update()
{
	static HRESULT result;

	while(1)
	{
		m_keyboard->Poll();
		if (SUCCEEDED(result = m_keyboard->GetDeviceState(256, (LPVOID)&m_keyState)))
			break;
		if (result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED)
			return;
		if (FAILED(m_keyboard->Acquire()))
			return;
	}

	while(1)
	{
		m_mouse->Poll();
		if (SUCCEEDED(result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState)))
			break;
		if (result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED)
			return;
		if (FAILED(m_mouse->Acquire()))
			return;
	}
	GetCursorPos(&m_position);
	ScreenToClient(m_window, &m_position);
}

bool Input::KeyDown(unsigned char key)
{
	if (m_keyState[key] & 0x80)
		return true;
	return false;
}

bool Input::MouseButtonDown(int button)
{
	if (m_mouseState.rgbButtons[button] & 0x80)
		return true;
	return false;
}

float Input::MouseDX()
{
	return m_mouseState.lX;
}

float Input::MouseDY()
{
	return m_mouseState.lY;
}

float Input::MouseDZ()
{
	return m_mouseState.lZ;
}