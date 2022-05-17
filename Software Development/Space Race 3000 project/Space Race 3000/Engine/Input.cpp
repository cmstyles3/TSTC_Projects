#include "Input.h"
#include "engine.h"

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
	m_di->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	m_mouse->SetDataFormat(&c_dfDIMouse);
	m_mouse->SetCooperativeLevel(m_window, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	m_mouse->Acquire();

	m_joystickFound = false;
	m_joystick = 0;
	InitJoystick(-1000, 1000, -1000, 1000, 20);
}

Input::~Input()
{
	SAFE_RELEASE(m_di);
	SAFE_RELEASE(m_keyboard);
	SAFE_RELEASE(m_mouse);
	SAFE_RELEASE(m_joystick);
}

void Input::Update()
{
	static HRESULT result;

	while(1)
	{
		m_keyboard->Poll();
		memcpy(m_prevKeyState, m_keyState, sizeof(unsigned char) * 256);
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
		memcpy(&m_prevMouseState, &m_mouseState, sizeof(DIMOUSESTATE));
		if (SUCCEEDED(result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState)))
			break;
		if (result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED)
			return;
		if (FAILED(m_mouse->Acquire()))
			return;
	}
	while(1)
	{
		if (!m_joystickFound)
		{
			memset(&m_joyState, 0, sizeof(DIJOYSTATE));
			memcpy(&m_prevJoyState, &m_joyState, sizeof(DIJOYSTATE));
			break;
		}

		m_joystick->Poll();
		memcpy(&m_prevJoyState, &m_joyState, sizeof(DIJOYSTATE));
		memset(&m_joyState, 0, sizeof(DIJOYSTATE));
		if (SUCCEEDED(result = m_joystick->GetDeviceState(sizeof(DIJOYSTATE), &m_joyState)))
			break;
		if (result != DIERR_INPUTLOST && result != DIERR_NOTACQUIRED)
			return;
		if (FAILED(m_joystick->Acquire()))
			return;
		
	}
	//GetCursorPos(&m_position);
	//ScreenToClient(m_window, &m_position);
}

bool Input::KeyDown(unsigned char key)
{
	if (m_keyState[key] & 0x80)
		return true;
	return false;
}

bool Input::PrevKeyDown(unsigned char key)
{
	if (m_prevKeyState[key] & 0x80)
		return true;
	return false;
}

bool Input::MouseButtonDown(int button)
{
	if (m_mouseState.rgbButtons[button] & 0x80)
		return true;
	return false;
}

bool Input::PrevMouseButtonDown(int button)
{
	if (m_prevMouseState.rgbButtons[button] & 0x80)
		return true;
	return false;
}

long Input::MouseDX()
{
	return m_mouseState.lX;
}

long Input::MouseDY()
{
	return m_mouseState.lY;
}

long Input::MouseDZ()
{
	return m_mouseState.lZ;
}


GUID joyGuid;

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{

	joyGuid = pdidInstance->guidInstance;
    // Obtain an interface to the enumerated joystick.
    
    return DIENUM_STOP;

}

int Input::InitJoystick(int minX, int maxX, int minY, int maxY, int deadZone)
{
	
	HRESULT hr;
	hr = m_di->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,
                      NULL, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
		return(0);

	hr = m_di->CreateDevice(joyGuid, &m_joystick, NULL);
	if (FAILED(hr))
		return(0);

	// set data format
	hr = m_joystick->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(hr))
		return(0);

	// set cooperation level
	hr = m_joystick->SetCooperativeLevel(m_window, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	if (FAILED(hr))
		return(0);


	//// set the range of the joystick
	DIPROPRANGE joyAxisRange;

	// first x axis
	joyAxisRange.lMin = minX;
	joyAxisRange.lMax = maxX;

	joyAxisRange.diph.dwSize       = sizeof(DIPROPRANGE); 
	joyAxisRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	joyAxisRange.diph.dwObj        = DIJOFS_X;
	joyAxisRange.diph.dwHow        = DIPH_BYOFFSET;

	m_joystick->SetProperty(DIPROP_RANGE,&joyAxisRange.diph);

	// now y-axis
	joyAxisRange.lMin = minY;
	joyAxisRange.lMax = maxY;

	joyAxisRange.diph.dwSize       = sizeof(DIPROPRANGE); 
	joyAxisRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	joyAxisRange.diph.dwObj        = DIJOFS_Y;
	joyAxisRange.diph.dwHow        = DIPH_BYOFFSET;

	m_joystick->SetProperty(DIPROP_RANGE,&joyAxisRange.diph);

	// now z-axis
	joyAxisRange.lMin = minY;
	joyAxisRange.lMax = maxY;

	joyAxisRange.diph.dwSize = sizeof(DIPROPRANGE);
	joyAxisRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	joyAxisRange.diph.dwObj        = DIJOFS_Z;
	joyAxisRange.diph.dwHow        = DIPH_BYOFFSET;

	m_joystick->SetProperty(DIPROP_RANGE,&joyAxisRange.diph);


	// R
	// first x axis
	joyAxisRange.lMin = minX;
	joyAxisRange.lMax = maxX;

	joyAxisRange.diph.dwSize       = sizeof(DIPROPRANGE); 
	joyAxisRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	joyAxisRange.diph.dwObj        = DIJOFS_RX;
	joyAxisRange.diph.dwHow        = DIPH_BYOFFSET;

	m_joystick->SetProperty(DIPROP_RANGE,&joyAxisRange.diph);

	// now y-axis
	joyAxisRange.lMin = minY;
	joyAxisRange.lMax = maxY;

	joyAxisRange.diph.dwSize       = sizeof(DIPROPRANGE); 
	joyAxisRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	joyAxisRange.diph.dwObj        = DIJOFS_RY;
	joyAxisRange.diph.dwHow        = DIPH_BYOFFSET;

	m_joystick->SetProperty(DIPROP_RANGE,&joyAxisRange.diph);

	joyAxisRange.diph.dwObj = DIJOFS_RZ;
	m_joystick->SetProperty(DIPROP_RANGE,&joyAxisRange.diph);


	joyAxisRange.diph.dwObj = DIJOFS_SLIDER(0);
	m_joystick->SetProperty(DIPROP_RANGE, &joyAxisRange.diph);

	joyAxisRange.diph.dwObj = DIJOFS_SLIDER(1);
	m_joystick->SetProperty(DIPROP_RANGE, &joyAxisRange.diph);

	joyAxisRange.diph.dwObj = DIJOFS_POV(0);
	m_joystick->SetProperty(DIPROP_RANGE, &joyAxisRange.diph);

	// and now the dead band
	DIPROPDWORD deadBand; // here's our property word

	// scale dead zone by 100
	deadZone*=100;

	deadBand.diph.dwSize       = sizeof(deadBand);
	deadBand.diph.dwHeaderSize = sizeof(deadBand.diph);
	deadBand.diph.dwObj        = DIJOFS_X;
	deadBand.diph.dwHow        = DIPH_BYOFFSET;

	// deadband will be used on both sides of the range +/-
	deadBand.dwData            = deadZone;

	// finally set the property
	m_joystick->SetProperty(DIPROP_DEADZONE,&deadBand.diph);

	deadBand.diph.dwSize       = sizeof(deadBand);
	deadBand.diph.dwHeaderSize = sizeof(deadBand.diph);
	deadBand.diph.dwObj        = DIJOFS_Y;
	deadBand.diph.dwHow        = DIPH_BYOFFSET;

	// deadband will be used on both sides of the range +/-
	deadBand.dwData            = deadZone;


	// finally set the property
	m_joystick->SetProperty(DIPROP_DEADZONE,&deadBand.diph);

	deadBand.diph.dwSize       = sizeof(deadBand);
	deadBand.diph.dwHeaderSize = sizeof(deadBand.diph);
	deadBand.diph.dwObj        = DIJOFS_Z;
	deadBand.diph.dwHow        = DIPH_BYOFFSET;

	// deadband will be used on both sides of the range +/-
	deadBand.dwData            = deadZone;

	// finally set the property
	m_joystick->SetProperty(DIPROP_DEADZONE,&deadBand.diph);
	

	deadBand.diph.dwSize       = sizeof(deadBand);
	deadBand.diph.dwHeaderSize = sizeof(deadBand.diph);
	deadBand.diph.dwObj        = DIJOFS_RX;
	deadBand.diph.dwHow        = DIPH_BYOFFSET;

	// deadband will be used on both sides of the range +/-
	deadBand.dwData            = deadZone;

	// finally set the property
	m_joystick->SetProperty(DIPROP_DEADZONE,&deadBand.diph);

	deadBand.diph.dwSize       = sizeof(deadBand);
	deadBand.diph.dwHeaderSize = sizeof(deadBand.diph);
	deadBand.diph.dwObj        = DIJOFS_RY;
	deadBand.diph.dwHow        = DIPH_BYOFFSET;

	// deadband will be used on both sides of the range +/-
	deadBand.dwData            = deadZone;
	// finally set the property
	m_joystick->SetProperty(DIPROP_DEADZONE,&deadBand.diph);


	deadBand.diph.dwObj = DIJOFS_RZ;
	m_joystick->SetProperty(DIPROP_DEADZONE,&deadBand.diph);

	deadBand.diph.dwObj = DIJOFS_SLIDER(0);
	m_joystick->SetProperty(DIPROP_DEADZONE, &deadBand.diph);

	deadBand.diph.dwObj = DIJOFS_SLIDER(1);
	m_joystick->SetProperty(DIPROP_DEADZONE, &deadBand.diph);

	deadBand.diph.dwObj = DIJOFS_POV(0);
	m_joystick->SetProperty(DIPROP_DEADZONE, &deadBand.diph);

	if (FAILED(m_joystick->Acquire()))
		return(0);

	m_joystickFound = true;

	return(1);
}

float Input::JoystickDX()
{
	return ((float)m_joyState.lX/1000.0f);
}

float Input::JoystickDY()
{
	return ((float)m_joyState.lY/1000.0f);
}

float Input::JoystickDZ()
{
	return ((float)m_joyState.lZ/1000.0f);
}

float Input::JoystickDRX()
{
	return ((float)m_joyState.lRx/1000.0f);
}

float Input::JoystickDRY()
{
	return ((float)m_joyState.lRy/1000.0f);
}

float Input::JoystickDRZ()
{
	return ((float)m_joyState.lRz/1000.0f);
}

float Input::JoystickSlider(int n)
{
	return ((float)m_joyState.rglSlider[n]/1000.0f);
}

float Input::JoystickPOV(int n)
{
	return ((float)m_joyState.rgdwPOV[n]/1000.0f);
}

bool Input::JoystickButtonDown(int button)
{
	if (m_joyState.rgbButtons[button] & 0x80)
		return true;
	return false;
}

bool Input::PrevJoystickButtonDown(int button)
{
	if (m_prevJoyState.rgbButtons[button] & 0x80)
		return true;
	return false;
}
