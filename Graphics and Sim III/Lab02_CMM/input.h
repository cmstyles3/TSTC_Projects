#ifndef INPUT_H
#define INPUT_H

#include <dinput.h>

int  InitDirectInput();
void KillDirectInput();

int  InitKeyboard();
void KillKeyboard();
int  ReadKeyboard();

bool IsKeyDown(int key);

int  InitMouse();
void KillMouse();
int  ReadMouse();

bool IsButtonDown(int button);

int  InitJoystick(int min_x=-256, int max_x=256, int min_y=-256, int max_y=256, int dead_zone = 10);
void KillJoystick();
int  ReadJoystick();



extern HWND g_hwnd;
extern HINSTANCE g_hinstance;

extern char keystate[256];
extern DIMOUSESTATE mousestate;
extern DIJOYSTATE joystate;

extern LPDIRECTINPUT8 g_pDI;
extern LPDIRECTINPUTDEVICE8 g_pKeyboard;
extern LPDIRECTINPUTDEVICE8 g_pMouse;
extern LPDIRECTINPUTDEVICE8 g_pJoystick;

#endif
