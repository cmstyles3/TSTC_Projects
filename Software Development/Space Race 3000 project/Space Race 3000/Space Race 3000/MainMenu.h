// MAINMENU.H

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef MAINMENU_H
#define MAINMENU_H

#include "../Engine/Engine.h"

#define NUM_NAMES 6

class MainMenu : public State
{
	public:
		MainMenu();

		void Load();
		void Close();

		void OnLostDevice();
		void OnResetDevice();

		void Update(float dt);
		void Draw();

		void MsgProc(UINT msg, WPARAM wparam, LPARAM lparam);

		void UpdateMainMenu();
		void DrawMainMenu();

		void UpdateTimeTrialMenu();
		void DrawTimeTrialMenu();

	private:
		LPDIRECT3DTEXTURE9 m_menuTexture, m_cursorTexture;
		LPDIRECT3DTEXTURE9 m_instrTexture, m_creditsTexture;

		int m_currScreen;

		D3DXVECTOR3 m_cursorPosition;

		D3DXVECTOR3 m_timetrialBtnPos;
		D3DXVECTOR3 m_multiplayerBtnPos;
		D3DXVECTOR3 m_selectshipBtnPos;
		D3DXVECTOR3 m_instructionsBtnPos;
		D3DXVECTOR3 m_creditsBtnPos;
		D3DXVECTOR3 m_exitBtnPos;

		bool m_timetrialBtnSelected;
		bool m_multiplayerBtnSelected;
		bool m_selectshipBtnSelected;
		bool m_instructionsBtnSelected;
		bool m_creditsBtnSelected;
		bool m_exitBtnSelected;

		bool m_timetrialBtnDown;
		bool m_multiplayerBtnDown;
		bool m_selectshipBtnDown;
		bool m_instructionsBtnDown;
		bool m_creditsBtnDown;
		bool m_exitBtnDown;


		//timetrial variables
		LPDIRECT3DTEXTURE9 m_timetrialTexture;
		D3DXVECTOR3 m_startBtnPos;
		bool m_startBtnSelected;
		bool m_startBtnDown;

		//multiplayer variables
		LPDIRECT3DTEXTURE9 m_multiplayerTexture;
		D3DXVECTOR3 m_hostBtnPos;
		D3DXVECTOR3 m_joinBtnPos;
		D3DXVECTOR3 m_nameTextPos;
		D3DXVECTOR3 m_ipTextPos;

		bool m_nameTextActive;
		bool m_ipTextActive;

		bool m_hostBtnSelected;
		bool m_joinBtnSelected;

		bool m_hostBtnDown;
		bool m_joinBtnDown;

		char m_name[16];
		char m_ip[16];


		//shipselect variables
		int m_currShip;
		Object3D m_shipObj;
		float m_rotation;
		

};

#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////