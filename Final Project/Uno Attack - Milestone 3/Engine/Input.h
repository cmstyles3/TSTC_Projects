#ifndef INPUT_H
#define INPUT_H

#include <d3dx9.h>
#include <dinput.h>


class Input
{
	public:
		Input(HWND window);
		virtual ~Input();

		void Update();
		bool KeyDown(unsigned char key);
		bool PrevKeyDown(unsigned char key);

		bool MouseButtonDown(int button);
		bool PrevMouseButtonDown(int button);

		long MouseDX();
		long MouseDY();
		long MouseDZ();

		void SetCursorPos(int x, int y);
		D3DXVECTOR3 GetCursorPos();

		int InitJoystick(int, int, int, int, int);
		
		bool JoystickButtonDown(int button);
		bool PrevJoystickButtonDown(int button);
		float JoystickDX();
		float JoystickDY();
		float JoystickDZ();

		float JoystickSlider(int n);
		float JoystickPOV(int n);

		float JoystickDRX();
		float JoystickDRY();
		float JoystickDRZ();

		DIJOYSTATE m_joyState;
	private:
		HWND m_window;
		LPDIRECTINPUT8 m_di;

		LPDIRECTINPUTDEVICE8 m_keyboard;
		char m_keyState[256];
		char m_prevKeyState[256];

		LPDIRECTINPUTDEVICE8 m_mouse;
		DIMOUSESTATE m_mouseState;
		DIMOUSESTATE m_prevMouseState;
		POINT m_position;
		D3DXVECTOR3 m_cursorPos;

		LPDIRECTINPUTDEVICE8 m_joystick;
		
		DIJOYSTATE m_prevJoyState;
		bool m_joystickFound;

};


#endif