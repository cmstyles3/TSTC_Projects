#ifndef MENU_H
#define MENU_H

#include "../Engine/Engine.h"


struct Char16
{
	char s[16];
};

class Menu : public State
{
	enum {MENU, MULTIPLAYER, INSTRUCTIONS, CREDITS};

	public:
		Menu(DWORD id);

		virtual void Load();
		virtual void Close();
		virtual void OnLostDevice();
		virtual void OnResetDevice();
		virtual void Update(float dt);
		virtual void Draw();
		virtual void MsgProc(UINT msg, WPARAM wparam, LPARAM lparam);

		// MENU
		void UpdateMenu();
		void UpdateSpBtn();
		void UpdateMpBtn();
		void DrawMenu();

		// MULTIPLAYER MENU
		void UpdateMultiplayer();
		void UpdateHostBtn();
		void UpdateJoinBtn();
		void UpdateNameText();
		void UpdateIPText();
		void DrawMultiplayer();

	private:
		int m_menuState;

		LPDIRECT3DTEXTURE9 m_bkgdTex;
		LPDIRECT3DTEXTURE9 m_mpScreenTex;
		LPDIRECT3DTEXTURE9 m_cursorTex;

		// SINGLE PLAYER BUTTON VARIABLES
		bool m_spBtnDown;
		bool m_spBtnSelected;
		int m_spBtnWidth;
		int m_spBtnHeight;
		D3DXVECTOR3 m_spBtnPosition;
		RECT m_spBtnRect;

		// MULTIPLAYER BUTTON VARIABLES
		bool m_mpBtnDown;
		bool m_mpBtnSelected;
		int m_mpBtnWidth;
		int m_mpBtnHeight;
		D3DXVECTOR3 m_mpBtnPosition;
		RECT m_mpBtnRect;

		// HOST BUTTON VARIABLES
		bool m_hostBtnDown;
		bool m_hostBtnSelected;
		int m_hostBtnWidth;
		int m_hostBtnHeight;
		D3DXVECTOR3 m_hostBtnPosition;
		RECT m_hostBtnRect;

		// JOIN BUTTON VARIABLES
		bool m_joinBtnDown;
		bool m_joinBtnSelected;
		int m_joinBtnWidth;
		int m_joinBtnHeight;
		D3DXVECTOR3 m_joinBtnPosition;
		RECT m_joinBtnRect;

		int m_textWidth;
		int m_textHeight;
		RECT m_textRect;
		// NAME TEXT VARIABLES
		char m_name[16];
		bool m_nameTextActive;
		D3DXVECTOR3 m_nameTextPosition;
		
		// IP TEXT VARIABLES
		char m_ip[16];
		bool m_ipTextActive;
		D3DXVECTOR3 m_ipTextPosition;
		std::vector<Char16> m_ipList;

		D3DXVECTOR3 m_cursorPosition;

};

#endif