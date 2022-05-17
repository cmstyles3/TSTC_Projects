#ifndef _INPUT__H_
#define _INPUT__H_
#include <dinput.h>

#define KILL_DEVICE(d) if (d) { d->Unaquire(); d->Release(); d = 0; }

class CInput
{
	public:
		CInput();
		~CInput();

		bool InitDI();
		void KillDI();

		bool InitKB();
		void KillKB();
		bool ReadKB();

		bool IsKeyDown(int key);
		bool WasKeyDown(int key);

		bool InitMouse();
		void KillMouse();
		bool ReadMouse();

		bool IsButtonDown(int button);
		bool WasButtonDown(int button);

		char m_KBState[256], m_PrevKBState[256];
		DIMOUSESTATE m_MouseState, m_PrevMouseState;
	private:
		LPDIRECTINPUT8 m_pDI;
		LPDIRECTINPUTDEVICE8 m_pKBDev;
		LPDIRECTINPUTDEVICE8 m_pMouseDev;
};

//extern bool kb_state[256], prev_kb_state[256];
//extern DIMOUSESTATE mouse_state, prev_mouse_state;

#endif