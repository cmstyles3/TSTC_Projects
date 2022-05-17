#ifndef _INPUT__H_
#define _INPUT__H_
#include <dinput.h>

struct Input
{
	Input();
	bool initDI();
	void killDI();
	bool initKB();
	void killKB();
	bool readKB();
	bool initMouse();
	void killMouse();
	bool readMouse();
private:
	LPDIRECTINPUT8 di;
	LPDIRECTINPUTDEVICE8 dikb;
	LPDIRECTINPUTDEVICE8 dimouse;
};

extern bool kb_state[256], prev_kb_state[256];
extern DIMOUSESTATE mouse_state, prev_mouse_state;

#endif