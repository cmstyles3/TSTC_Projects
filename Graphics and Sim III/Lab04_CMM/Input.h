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
		bool MouseButtonDown(int button);

		long GetPosX();
		long GetPosY();
		float MouseDX();
		float MouseDY();
		float MouseDZ();

	private:
		HWND m_window;
		LPDIRECTINPUT8 m_di;

		LPDIRECTINPUTDEVICE8 m_keyboard;
		char m_keyState[256];

		LPDIRECTINPUTDEVICE8 m_mouse;
		DIMOUSESTATE m_mouseState;
		POINT m_position;

};

#endif