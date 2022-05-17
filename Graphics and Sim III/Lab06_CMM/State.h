#ifndef STATE_H
#define STATE_H

#include <windows.h>

class State
{
	public:
		State(DWORD id = 0);

		virtual void Load() {};
		virtual void Close() {};

		virtual void OnLostDevice() {}
		virtual void OnResetDevice() {}

		virtual void Update(float dt) {};
		virtual void Draw() {};

		DWORD GetID();

		DWORD GetColor();

		void (*MsgProc)(UINT msg, WPARAM, LPARAM);

	private:
		DWORD m_id;

	protected:
		DWORD m_color;

};

#endif