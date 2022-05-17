// STATE.H

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef STATE_H
#define STATE_H

class State
{
	public:
		State(DWORD id = 0);

		virtual void Load() {};
		virtual void Close() {};

		virtual void OnLostDevice() {};
		virtual void OnResetDevice() {};

		virtual void Update(float dt) {};
		virtual void Draw() {};

		virtual void MsgProc(UINT msg, WPARAM wparam, LPARAM lparam) {};

		DWORD GetID();

		DWORD GetColor();


	private:
		DWORD m_id;

	protected:
		DWORD m_color;

};

#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////