#include "input.h"

char keystate[256];
DIMOUSESTATE mousestate;
DIJOYSTATE joystate;

LPDIRECTINPUT8 g_pDI = 0;
LPDIRECTINPUTDEVICE8 g_pKeyboard = 0;
LPDIRECTINPUTDEVICE8 g_pMouse = 0;
LPDIRECTINPUTDEVICE8 g_pJoystick = 0;
GUID joyGuid;

int joystickFound = 0;

int InitDirectInput()
{
	if (FAILED(DirectInput8Create(g_hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDI, NULL)))
		return(0);
	return(1);
}

void KillDirectInput()
{
	if (g_pDI)
	{
		g_pDI->Release();
		g_pDI = 0;
	}
}

int InitKeyboard()
{
	if (g_pDI->CreateDevice(GUID_SysKeyboard, &g_pKeyboard, NULL)!=DI_OK)
		return 0;

	if (g_pKeyboard->SetDataFormat(&c_dfDIKeyboard)!=DI_OK)
		return 0;

	if (g_pKeyboard->SetCooperativeLevel(g_hwnd,DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)!=DI_OK)
		return 0;

	if (g_pKeyboard->Acquire()!=DI_OK)
		return 0;

	return 1;
}

void KillKeyboard()
{
	if (g_pKeyboard)
	{
		g_pKeyboard->Unacquire();
		g_pKeyboard->Release();
		g_pKeyboard = 0;
	}
}

int ReadKeyboard()
{
	if (g_pKeyboard)
    {
		if (g_pKeyboard->GetDeviceState(256, (LPVOID)keystate) != DI_OK)
			return 0;
    }
	else
    {
		memset(keystate, 0, sizeof(keystate));
		return 0;
    }

	return 1;
}

int InitMouse()
{
	
	if (g_pDI->CreateDevice(GUID_SysMouse, &g_pMouse, NULL) != DI_OK)
		return 0;

	if (g_pMouse->SetDataFormat(&c_dfDIMouse) != DI_OK)
		return 0;

	if (g_pMouse->SetCooperativeLevel(g_hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND) != DI_OK)
		return 0;

	if (g_pMouse->Acquire() != DI_OK)
		return 0;
	
	return 1;
}

void KillMouse()
{
	if (g_pMouse)
	{
		g_pMouse->Unacquire();
		g_pMouse->Release();
		g_pMouse = 0;
	}
}

int ReadMouse()
{
	if (g_pMouse)    
    {
		
		if (g_pMouse->GetDeviceState(sizeof(DIMOUSESTATE),(LPVOID)&mousestate) != DI_OK)
			return 0;
    }
	else
    {
		memset(&mousestate, 0, sizeof(mousestate));
		return 0;
    }

	return 1;
}

BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{

	joyGuid = pdidInstance->guidInstance;
    // Obtain an interface to the enumerated joystick.
    
    return DIENUM_STOP;

}

int InitJoystick(int minX, int maxX, int minY, int maxY, int deadZone)
{
	HRESULT hr;
	hr = g_pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback,
                      NULL, DIEDFL_ATTACHEDONLY);
	if (FAILED(hr))
		return(0);

	hr = g_pDI->CreateDevice(joyGuid, &g_pJoystick, NULL);
	if (FAILED(hr))
		return(0);

	// set data format
	hr = g_pJoystick->SetDataFormat(&c_dfDIJoystick);
	if (FAILED(hr))
		return(0);

	// set cooperation level
	hr = g_pJoystick->SetCooperativeLevel(g_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
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

	g_pJoystick->SetProperty(DIPROP_RANGE,&joyAxisRange.diph);

	// now y-axis
	joyAxisRange.lMin = minY;
	joyAxisRange.lMax = maxY;

	joyAxisRange.diph.dwSize       = sizeof(DIPROPRANGE); 
	joyAxisRange.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	joyAxisRange.diph.dwObj        = DIJOFS_Y;
	joyAxisRange.diph.dwHow        = DIPH_BYOFFSET;

	g_pJoystick->SetProperty(DIPROP_RANGE,&joyAxisRange.diph);


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
	g_pJoystick->SetProperty(DIPROP_DEADZONE,&deadBand.diph);

	deadBand.diph.dwSize       = sizeof(deadBand);
	deadBand.diph.dwHeaderSize = sizeof(deadBand.diph);
	deadBand.diph.dwObj        = DIJOFS_Y;
	deadBand.diph.dwHow        = DIPH_BYOFFSET;

	// deadband will be used on both sides of the range +/-
	deadBand.dwData            = deadZone;


	// finally set the property
	g_pJoystick->SetProperty(DIPROP_DEADZONE,&deadBand.diph);

	if (FAILED(g_pJoystick->Acquire()))
		return(0);

	joystickFound = 1;

	return(1);
}

int ReadJoystick()
{
	// make sure the joystick was initialized
	if (!joystickFound)
		return(0);

	if (g_pJoystick)
    {
		// this is needed for joysticks only    
		if (g_pJoystick->Poll() != DI_OK)
		{
			g_pJoystick->Acquire();
		}
			//return(0);

		if (g_pJoystick->GetDeviceState(sizeof(DIJOYSTATE), (LPVOID)&joystate) != DI_OK)
			return(0);
    }
	else
    {
		// joystick isn't plugged in, zero out state
		memset(&joystate, 0, sizeof(joystate));

		// return error
		return(0);
    } // end else

// return sucess
return(1);
}

void KillJoystick()
{
	if (g_pJoystick)
	{
		g_pJoystick->Unacquire();
		g_pJoystick->Release();
		g_pJoystick = 0;
	}
}