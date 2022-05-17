#include "Main.h"




Menu::Menu(DWORD id) : State(id)
{
}

void Menu::Load()
{
	m_menuState = MENU;

	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/menu.bmp", &m_bkgdTex);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/mpscreen.bmp", &m_mpScreenTex);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/cursor.dds", &m_cursorTex);

	// INITIALIZE SINGLE PLAYER BUTTON
	m_spBtnSelected = m_spBtnDown = false;
	m_spBtnWidth = 256;
	m_spBtnHeight = 50;
	m_spBtnPosition = D3DXVECTOR3(380, 450, 0);
	m_spBtnRect.left = 0;
	m_spBtnRect.right = m_spBtnRect.left+m_spBtnWidth;
	m_spBtnRect.top = 768;
	m_spBtnRect.bottom = m_spBtnRect.top+m_spBtnHeight;

	// INITIALIZE MULTIPLAYER BUTTON
	m_mpBtnSelected = m_mpBtnDown = false;
	m_mpBtnWidth = 256;
	m_mpBtnHeight = 50;
	m_mpBtnPosition = D3DXVECTOR3(380, 510, 0);
	m_mpBtnRect.left = 256;
	m_mpBtnRect.right = m_mpBtnRect.left+m_mpBtnWidth;
	m_mpBtnRect.top = 768;
	m_mpBtnRect.bottom = m_mpBtnRect.top+m_mpBtnHeight;


	// INITIALIZE HOST BUTTON
	m_hostBtnSelected = m_hostBtnDown = false;
	m_hostBtnWidth = 128;
	m_hostBtnHeight = 50;
	m_hostBtnPosition = D3DXVECTOR3(380, 450, 0);
	m_hostBtnRect.left = 0;
	m_hostBtnRect.right = m_hostBtnRect.left+m_hostBtnWidth;
	m_hostBtnRect.top = 768;
	m_hostBtnRect.bottom = m_hostBtnRect.top+m_hostBtnHeight;

	// INITIALIZE JOIN BUTTON
	m_joinBtnSelected = m_joinBtnDown = false;
	m_joinBtnWidth = 128;
	m_joinBtnHeight = 50;
	m_joinBtnPosition = D3DXVECTOR3(380, 510, 0);
	m_joinBtnRect.left = 128;
	m_joinBtnRect.right = m_joinBtnRect.left+m_joinBtnWidth;
	m_joinBtnRect.top = 768;
	m_joinBtnRect.bottom = m_joinBtnRect.top+m_joinBtnHeight;

	// INITIALIZE TEXT
	m_textWidth = 256;
	m_textHeight = 40;
	m_textRect.left = 0;
	m_textRect.right = m_textWidth;
	m_textRect.top = 818;
	m_textRect.bottom = m_textRect.top+m_textHeight;

	// INITIALIZE NAME TEXT
	strcpy(m_name, "");
	m_nameTextActive = false;
	m_nameTextPosition = D3DXVECTOR3(380, 300, 0);

	// INITIALIZE IP TEXT
	strcpy(m_ip, "");
	m_ipTextActive = false;
	m_ipTextPosition = D3DXVECTOR3(380, 350, 0);


	m_cursorPosition = D3DXVECTOR3(500.0f, 240.0f, 0.0f);

}

void Menu::Close()
{
	SAFE_RELEASE(m_bkgdTex);
	SAFE_RELEASE(m_mpScreenTex);
	SAFE_RELEASE(m_cursorTex);
}

void Menu::OnLostDevice()
{
}

void Menu::OnResetDevice()
{
}

void Menu::Update(float dt)
{
	Input *input = g_engine->GetInput();

	switch(m_menuState)
	{
		case MENU:
		{
			UpdateMenu();
		}break;

		case MULTIPLAYER:
		{
			UpdateMultiplayer();
		}break;

	}	

}

void Menu::Draw()
{

	switch(m_menuState)
	{
		case MENU:
		{
			DrawMenu();
		}break;

		case MULTIPLAYER:
		{
			
			DrawMultiplayer();
		}break;

	}
	
	

	LPD3DXSPRITE sprite = g_engine->GetSprite();
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->Draw(m_cursorTex, 0, 0, &g_engine->GetInput()->GetCursorPos(), 0xffffffff);
	sprite->End();

}

void Menu::MsgProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_CHAR:
		{
				if (m_menuState == MULTIPLAYER)
				{
					if (m_nameTextActive)
					{
						int len = strlen(m_name);
						char ch = LOWORD(wparam);
						if (ch == 13)
						{
							m_nameTextActive = false;
						}
						else if (ch == 8 && len > 0)
						{
							m_name[len-1] = '\0';
							//_sounds[4]->Play();
						}
						else if (len < 15 && ch >= ' ' && ch <= 'z')
						{
							m_name[len] = ch;
							m_name[len+1] = '\0';
							//g_sounds[4]->Play();
						}
					}
					else if (m_ipTextActive)
					{
						int len = strlen(m_ip);
						char ch = LOWORD(wparam);
						if (ch == 13)
						{
							m_ipTextActive = false;
						}
						else if (ch == 8 && len > 0)
						{
							m_ip[len-1] = '\0';
							//_sounds[4]->Play();
						}
						else if (len < 15 && ch >= ' ' && ch <= 'z')
						{
							m_ip[len] = ch;
							m_ip[len+1] = '\0';
							//g_sounds[4]->Play();
						}
					}
				}
		}break;
	}
	
}

void Menu::UpdateMenu()
{
	if (g_engine->GetInput()->KeyDown(DIK_ESCAPE) &&
		!g_engine->GetInput()->PrevKeyDown(DIK_ESCAPE))
		PostQuitMessage(0);

	// UPDATE BUTTONS
	UpdateSpBtn();
	UpdateMpBtn();
}

void Menu::UpdateSpBtn()
{
	D3DXVECTOR3 cursorPos = g_engine->GetInput()->GetCursorPos();
	if (cursorPos.x > m_spBtnPosition.x && cursorPos.x < m_spBtnPosition.x + m_spBtnWidth && 
		cursorPos.y > m_spBtnPosition.y && cursorPos.y < m_spBtnPosition.y + m_spBtnHeight)
	{
		if (m_spBtnSelected == false)
		{//play sound
			g_buttonSound->Play();
		}
		m_spBtnSelected = true;

		if (g_engine->GetInput()->MouseButtonDown(0))
			m_spBtnDown = true;

		if (g_engine->GetInput()->PrevMouseButtonDown(0) &&
			!g_engine->GetInput()->MouseButtonDown(0))
		{
			//Do something
			g_engine->ChangeState(GS_SINGLEPLAYER);
			m_spBtnDown = false;
		}
	}
	else
	{
		m_spBtnSelected = false;
		m_spBtnDown= false;
	}
}

void Menu::UpdateMpBtn()
{
	D3DXVECTOR3 cursorPos = g_engine->GetInput()->GetCursorPos();
	if (cursorPos.x > m_mpBtnPosition.x && cursorPos.x < m_mpBtnPosition.x + m_mpBtnWidth && 
		cursorPos.y > m_mpBtnPosition.y && cursorPos.y < m_mpBtnPosition.y + m_mpBtnHeight)
	{
		if (m_mpBtnSelected == false)
		{//play sound
			g_buttonSound->Play();
		}
		m_mpBtnSelected = true;

		if (g_engine->GetInput()->MouseButtonDown(0))
			m_mpBtnDown = true;

		if (g_engine->GetInput()->PrevMouseButtonDown(0) &&
			!g_engine->GetInput()->MouseButtonDown(0))
		{
			//Do something
			//g_engine->ChangeState(GS_);
			// m_menuState = MULTIPLAYER
			m_menuState = MULTIPLAYER;
			m_mpBtnDown = false;
		}
	}
	else
	{
		m_mpBtnSelected = false;
		m_mpBtnDown= false;
	}
}

void Menu::DrawMenu()
{
	LPD3DXSPRITE sprite = g_engine->GetSprite();
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->Draw(m_bkgdTex, 0, 0, 0, 0xffffffff);
	
	sprite->Draw(m_bkgdTex, &m_spBtnRect, 0, &m_spBtnPosition, m_spBtnSelected ? 0xffffffff : 0x66ffffff); 
	sprite->Draw(m_bkgdTex, &m_mpBtnRect, 0, &m_mpBtnPosition, m_mpBtnSelected ? 0xffffffff : 0x66ffffff); 

	sprite->End();
}

void Menu::UpdateMultiplayer()
{
	if (g_engine->GetInput()->KeyDown(DIK_ESCAPE) &&
		!g_engine->GetInput()->PrevKeyDown(DIK_ESCAPE))
		m_menuState = MENU;

	UpdateHostBtn();
	UpdateJoinBtn();
	UpdateNameText();
	UpdateIPText();
}

void Menu::UpdateHostBtn()
{
	D3DXVECTOR3 cursorPos = g_engine->GetInput()->GetCursorPos();
	if (cursorPos.x > m_hostBtnPosition.x && cursorPos.x < m_hostBtnPosition.x + m_hostBtnWidth && 
		cursorPos.y > m_hostBtnPosition.y && cursorPos.y < m_hostBtnPosition.y + m_hostBtnHeight)
	{
		if (m_hostBtnSelected == false)
		{//play sound
			g_buttonSound->Play();
		}
		m_hostBtnSelected = true;

		if (g_engine->GetInput()->MouseButtonDown(0))
			m_hostBtnDown = true;

		if (g_engine->GetInput()->PrevMouseButtonDown(0) &&
			!g_engine->GetInput()->MouseButtonDown(0))
		{
			if (g_engine->GetNetwork()->Host(m_name, "Uno Attack Session", 4))
			{
				g_engine->ChangeState(GS_MULTIPLAYER);
			}
			m_hostBtnDown = false;
		}
	}
	else
	{
		m_hostBtnSelected = false;
		m_hostBtnDown= false;
	}
}

void Menu::UpdateJoinBtn()
{
	D3DXVECTOR3 cursorPos = g_engine->GetInput()->GetCursorPos();
	if (cursorPos.x > m_joinBtnPosition.x && cursorPos.x < m_joinBtnPosition.x + m_joinBtnWidth && 
		cursorPos.y > m_joinBtnPosition.y && cursorPos.y < m_joinBtnPosition.y + m_joinBtnHeight)
	{
		if (m_joinBtnSelected == false)
		{//play sound
			g_buttonSound->Play();
		}
		m_joinBtnSelected = true;

		if (g_engine->GetInput()->MouseButtonDown(0))
			m_joinBtnDown = true;

		if (g_engine->GetInput()->PrevMouseButtonDown(0) &&
			!g_engine->GetInput()->MouseButtonDown(0))
		{
			if (strlen(m_ip) < 7)
				g_engine->GetNetwork()->EnumerateSessions();
			else g_engine->GetNetwork()->EnumerateSessions(m_ip);

			if (g_engine->GetNetwork()->Join(m_name))
			{
				g_engine->ChangeState(GS_MULTIPLAYER);
			}
			//Do something
			//g_engine->ChangeState(GS_);
			// m_menuState = MULTIPLAYER
			m_joinBtnDown = false;
		}
	}
	else
	{
		m_joinBtnSelected = false;
		m_joinBtnDown = false;
	}
}

void Menu::UpdateNameText()
{
	D3DXVECTOR3 cursorPos = g_engine->GetInput()->GetCursorPos();
	if (cursorPos.x > m_nameTextPosition.x && cursorPos.x < m_nameTextPosition.x + m_textWidth && 
		cursorPos.y > m_nameTextPosition.y && cursorPos.y < m_nameTextPosition.y + m_textHeight)
	{
		if (m_nameTextActive == false)
		{//play sound
			g_buttonSound->Play();
		}
		m_nameTextActive = true;

		if (g_engine->GetInput()->MouseButtonDown(0))
			strcpy(m_name, "");
		/*if (g_engine->GetInput()->MouseDZ() > 0)
		{
			
			strcpy(m_name, names[num++]);
		}*/
	}
	else
	{
		m_nameTextActive = false;
	}
}

void Menu::UpdateIPText()
{
	D3DXVECTOR3 cursorPos = g_engine->GetInput()->GetCursorPos();
	if (cursorPos.x > m_ipTextPosition.x && cursorPos.x < m_ipTextPosition.x + m_textWidth && 
		cursorPos.y > m_ipTextPosition.y && cursorPos.y < m_ipTextPosition.y + m_textHeight)
	{
		if (m_ipTextActive == false)
		{//play sound
			g_buttonSound->Play();
		}
		m_ipTextActive = true;

		if (g_engine->GetInput()->MouseButtonDown(0))
			strcpy(m_ip, "");

	}
	else
	{
		m_ipTextActive = false;
	}
}

void Menu::DrawMultiplayer()
{
	LPD3DXSPRITE sprite = g_engine->GetSprite();
	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->Draw(m_mpScreenTex, 0, 0, 0, 0xffffffff);
	
	sprite->Draw(m_mpScreenTex, &m_hostBtnRect, 0, &m_hostBtnPosition, m_hostBtnSelected ? 0xffffffff : 0x66ffffff); 
	sprite->Draw(m_mpScreenTex, &m_joinBtnRect, 0, &m_joinBtnPosition, m_joinBtnSelected ? 0xffffffff : 0x66ffffff);

	g_font->Draw(300, 300, 0xffffffff, "Name"); 
	g_font->Draw(330, 350, 0xffffffff, "IP");
	sprite->Draw(m_mpScreenTex, &m_textRect, 0, &m_nameTextPosition, m_nameTextActive ? 0xffffffff : 0x66ffffff);
	sprite->Draw(m_mpScreenTex, &m_textRect, 0, &m_ipTextPosition, m_ipTextActive ? 0xffffffff : 0x66ffffff);

	g_font->Draw(m_nameTextPosition.x, m_nameTextPosition.y+3, m_nameTextActive ? 0xffffffff : 0x66ffffff, m_name);
	g_font->Draw(m_ipTextPosition.x, m_ipTextPosition.y+3, m_ipTextActive ? 0xffffffff : 0x66ffffff, m_ip);

	sprite->End();
}

/*m_button.Init(m_bkgdTex, 0, 768, 256, 50);
	m_button.SetPosition(380, 450);
	m_button.SetEventFunc(EventFunc);

	m_button2.Init(m_bkgdTex, 256, 768, 256, 50);
	m_button2.SetPosition(380, 510);
	m_button2.SetEventFunc(DoStuff);*/


	//LPD3DXSPRITE sprite = g_engine->GetSprite();
	//sprite->Begin(0);
	//sprite->Draw(m_bkgdTex, 0, 0, 0, 0xffffffff);
	//sprite->End();

	//sprite->Begin(D3DXSPRITE_ALPHABLEND);
	////RECT rect = {656, 177, 756, 256};
	//
	///*sprite->Draw(g_cardTexture, &rect, &D3DXVECTOR3(50, 128, 0), &D3DXVECTOR3(512, 302, 0), 0xffffffff);
	//m_cardParticles.Draw();

	//rect.top = 0;
	//rect.bottom = 177;
	//sprite->Draw(g_cardTexture, &rect, &D3DXVECTOR3(50, 128, 0), &D3DXVECTOR3(512, 125, 0), 0xffffffff);*/

	////sprite->Draw(m_spBtnTex, 0, 0, &m_spBtnPosition, m_spBtnSelected ? 0xffff00ff : 0xffffffff);
	//
	//sprite->Draw(m_bkgdTex, &m_spBtnRect, 0, &m_spBtnPosition, m_spBtnSelected ? 0xffffffff : 0x66ffffff); 
	//sprite->Draw(m_bkgdTex, &m_mpBtnRect, 0, &m_mpBtnPosition, m_mpBtnSelected ? 0xffffffff : 0x66ffffff); 
	////m_button.Draw();
	////m_button2.Draw();
	//sprite->Draw(m_cursorTex, 0, 0, &g_engine->GetInput()->GetCursorPos(), 0xffffffff);

	//sprite->End();