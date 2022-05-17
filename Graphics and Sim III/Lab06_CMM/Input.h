#ifndef INPUT_H
#define INPUT_H

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

		int InitJoystick(int, int, int, int, int);
		
		bool JoystickButtonDown(int button);
		bool PrevJoystickButtonDown(int button);
		long JoystickDX();
		long JoystickDY();
		long JoystickDZ();

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

		LPDIRECTINPUTDEVICE8 m_joystick;
		DIJOYSTATE m_joyState;
		DIJOYSTATE m_prevJoyState;
		bool m_joystickFound;

};


#endif