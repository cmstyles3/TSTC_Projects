#include <dinput.h>
#include "input.h"

extern HWND hwnd_app;
extern HINSTANCE hinstance_app;
bool kb_state[256];
bool prev_kb_state[256];
DIMOUSESTATE mouse_state, prev_mouse_state;
	

Input::Input()
{
	di = 0;
	dikb = 0;
	dimouse = 0;
}

bool Input::initDI()
{
	if (FAILED(DirectInput8Create(hinstance_app, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&di,NULL)))
		return false;

	return true;
}

void Input::killDI()
{
	if (di)
		di->Release();
}

bool Input::initKB()
{
	if (di->CreateDevice(GUID_SysKeyboard, &dikb, NULL)!=DI_OK)
		return false;

	if (dikb->SetDataFormat(&c_dfDIKeyboard)!=DI_OK)
		return false;

	if (dikb->SetCooperativeLevel(hwnd_app,DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)!=DI_OK)
		return false;

	if (dikb->Acquire()!=DI_OK)
		return false;

	return true;
}
	
  
void Input::killKB()
{
	if (dikb) 
	{ 
		dikb->Unacquire(); 
        dikb->Release();
        dikb = NULL; 
    } 
}

bool Input::readKB()
{
	if (dikb)
    {
		memcpy(prev_kb_state, kb_state, 256 * sizeof(bool));
		if (dikb->GetDeviceState(256, (LPVOID)kb_state)!=DI_OK)
			return false;
    }
	else
    {
		memset(kb_state,0,sizeof(kb_state));
		return false;
    }

	return true;
}

bool Input::initmouse()
{
	if (di->CreateDevice(GUID_SysMouse, &dimouse, NULL)!=DI_OK)
		return false;

	if (dimouse->SetDataFormat(&c_dfDIMouse)!=DI_OK)
		return false;

	if (dimouse->SetCooperativeLevel(hwnd_app,DISCL_EXCLUSIVE | DISCL_FOREGROUND)!=DI_OK)
		return false;

	if (dimouse->Acquire()!=DI_OK)
		return false;
	
	return true;
}

void Input::killmouse()
{
	if (dimouse) 
	{ 
		dimouse->Unacquire(); 
        dimouse->Release();
        dimouse = NULL; 
    } 
}

bool Input::readmouse()
{
	if (dimouse)    
    {
		prev_mouse_state.rgbButtons[0] = mouse_state.rgbButtons[0];
		if (dimouse->GetDeviceState(sizeof(DIMOUSESTATE),(LPVOID)&mouse_state)!=DI_OK)
			return false;
    }
	else
    {
		memset(&mouse_state,0,sizeof(mouse_state));
		return false;
    }

	return true;
}