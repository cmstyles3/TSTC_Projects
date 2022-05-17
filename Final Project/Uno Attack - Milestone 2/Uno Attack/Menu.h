#ifndef MENU_H
#define MENU_H

#include "button.h"

class Menu : public State
{
	public:
		Menu(DWORD id);

		virtual void Load();
		virtual void Close();
		virtual void OnLostDevice();
		virtual void OnResetDevice();
		virtual void Update(float dt);
		virtual void Draw();

	private:
		LPDIRECT3DTEXTURE9 m_bkgdTex;
		LPDIRECT3DTEXTURE9 m_cursorTex;
		LPDIRECT3DTEXTURE9 m_spBtnTex;
		LPDIRECT3DTEXTURE9 m_exitBtnTex;

		bool m_spBtnDown;
		bool m_spBtnSelected;
		int m_spBtnWidth;
		int m_spBtnHeight;
		D3DXVECTOR3 m_spBtnPosition;

		D3DXVECTOR3 m_cursorPosition;

		Button m_button;
		Button m_button2;

};

#endif