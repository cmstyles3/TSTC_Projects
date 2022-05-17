#ifndef MENU_H
#define MENU_H

#include "main.h"

#define MENU_STATE 1

class MenuState : public State
{
	public:
		MenuState();
		
		virtual void Load();
		virtual void Close();
		virtual void Update(float dt);
		virtual void Draw();

	private:
		LPDIRECT3DTEXTURE9 m_texture, m_cursorTex;
		D3DXVECTOR2 m_cursor;

		D3DXVECTOR2 m_duckBtn;
		D3DXVECTOR2 m_ballBtn;
		D3DXVECTOR2 m_dartBtn;

		bool m_duckBtnSelected;
		bool m_ballBtnSelected;
		bool m_dartBtnSelected;

		Sound *m_sound;
		Sound *m_btnSelectSound;
};

#endif