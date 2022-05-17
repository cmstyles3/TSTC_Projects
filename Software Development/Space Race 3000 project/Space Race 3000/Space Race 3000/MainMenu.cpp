// MAINMENU.CPP

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "Main.h"

const char *name[NUM_NAMES] = 
{
	"Dude", 
	"Player", 
	"Somebody else", 
	"Your mom", 
	"Yourself", 
	"Sam",
};

enum {MENU, TIMETRIAL, MULTIPLAYER, SHIPSELECT, INSTRUCTIONS, CREDITS};

MainMenu::MainMenu() : State(MAINMENU_STATE)
{	
}

void MainMenu::Load()
{
	m_currScreen = MENU;

	m_timetrialBtnSelected = 
		m_multiplayerBtnSelected = 
		m_selectshipBtnSelected = 
		m_instructionsBtnSelected = 
		m_creditsBtnSelected = 
		m_exitBtnSelected = false;

	m_cursorPosition = D3DXVECTOR3(500.0f, 240.0f, 0.0f);
	m_timetrialBtnPos = D3DXVECTOR3(50.0f, 285.0f, 0.0f);
	m_multiplayerBtnPos = D3DXVECTOR3(50.0f, 330.0f, 0.0f);
	m_selectshipBtnPos = D3DXVECTOR3(50.0f, 375.0f, 0.0f);
	m_instructionsBtnPos = D3DXVECTOR3(50.0f, 420.0f, 0.0f);
	m_creditsBtnPos = D3DXVECTOR3(50.0f, 465.0f, 0.0f);
	m_exitBtnPos = D3DXVECTOR3(50.0f, 510.0f, 0.0f);

	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/MainMenuScreen.bmp", &m_menuTexture);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/cursor.dds", &m_cursorTexture);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/InstructionsScreen.bmp", &m_instrTexture);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/CreditsScreen.bmp", &m_creditsTexture);


	// Set timetrial variables
	m_startBtnSelected = false;
	m_startBtnPos = D3DXVECTOR3(300.0f, 500.0f, 0.0f);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/TimetrialScreen.bmp", &m_timetrialTexture);

	// Set multiplayer variables
	m_hostBtnSelected = false;
	m_joinBtnSelected = false;
	m_hostBtnDown = false;
	m_joinBtnDown = false;
	m_hostBtnPos = D3DXVECTOR3(100.0f, 280.0f, 0.0f);
	m_joinBtnPos = D3DXVECTOR3(100.0f, 340.0f, 0.0f);
	m_nameTextPos = D3DXVECTOR3(100.0f, 160.0f, 0.0f);
	m_ipTextPos = D3DXVECTOR3(100.0f, 220.0f, 0.0f);

	strcpy(m_name, name[rand()%NUM_NAMES]);
	memset(m_ip, 0, sizeof(char)*16);

	D3DXCreateTextureFromFile(g_engine->GetDevice(), "Assets/Textures/MultiplayerScreen.bmp", &m_multiplayerTexture);

	// Set shipselect variables
	m_shipObj.Reset();
	m_currShip = 0;
	m_rotation = 0.0f;
	//m_shipObj.SetLook(D3DXVECTOR3(0.0f, 0.0f, -1.0f));

	//number = 0;
	//strcpy(m_name, "0.0.0.0");//name[rand()%NUM_NAMES]);
}

void MainMenu::Close()
{
	SAFE_RELEASE(m_menuTexture);
	SAFE_RELEASE(m_cursorTexture);
	SAFE_RELEASE(m_instrTexture);
	SAFE_RELEASE(m_creditsTexture);
	SAFE_RELEASE(m_multiplayerTexture);
	SAFE_RELEASE(m_timetrialTexture);
}

void MainMenu::OnLostDevice()
{
	//g_font->OnLostDevice();
}

void MainMenu::OnResetDevice()
{
	//g_font->OnResetDevice();
}

void MainMenu::Update(float dt)
{
	Input *input = g_engine->GetInput();

	m_cursorPosition.x += g_engine->GetInput()->MouseDX();
	m_cursorPosition.y += g_engine->GetInput()->MouseDY();

	
	if (m_cursorPosition.x < 0)
	{
		m_cursorPosition.x = 0;
	}
	else if (m_cursorPosition.x >= 800)
	{
		m_cursorPosition.x = 799;
	}

	if (m_cursorPosition.y < 0)
	{
		m_cursorPosition.y = 0;
	}
	else if (m_cursorPosition.y >= 600)
	{
		m_cursorPosition.y = 599;
	}
	
	switch(m_currScreen)
	{
		case MENU:
		{
			if (input->KeyDown(DIK_E))
			{
				g_engine->ChangeState(EDITTRACK_STATE);
			}
			if (m_cursorPosition.x > m_timetrialBtnPos.x && m_cursorPosition.x < m_timetrialBtnPos.x + 250 &&
				m_cursorPosition.y > m_timetrialBtnPos.y && m_cursorPosition.y < m_timetrialBtnPos.y + 40)
			{
				if (m_timetrialBtnSelected == false)
					g_sounds[2]->Play();

				m_timetrialBtnSelected = true;

				if (g_engine->GetInput()->MouseButtonDown(0))
					m_timetrialBtnDown = true;

				else m_timetrialBtnDown = false;

				if (g_engine->GetInput()->PrevMouseButtonDown(0) && 
					!g_engine->GetInput()->MouseButtonDown(0))
				{
					g_sounds[1]->Play();
					m_currScreen = TIMETRIAL;
				}
			}
			else 
			{
				m_timetrialBtnSelected = false;
				m_timetrialBtnDown = false;
			}


			if (m_cursorPosition.x > m_multiplayerBtnPos.x && m_cursorPosition.x < m_multiplayerBtnPos.x + 250 &&
				m_cursorPosition.y > m_multiplayerBtnPos.y && m_cursorPosition.y < m_multiplayerBtnPos.y + 40)
			{
				if (m_multiplayerBtnSelected == false)
					g_sounds[2]->Play();

				m_multiplayerBtnSelected = true;

				if (g_engine->GetInput()->MouseButtonDown(0))
					m_multiplayerBtnDown = true;

				else m_multiplayerBtnDown = false;

				if (g_engine->GetInput()->PrevMouseButtonDown(0) && 
					!g_engine->GetInput()->MouseButtonDown(0))
				{
					g_sounds[1]->Play();
					m_currScreen = MULTIPLAYER;
				}
				
			}
			else 
			{
				m_multiplayerBtnSelected = false;
				m_multiplayerBtnDown = false;
			}

			if (m_cursorPosition.x > m_selectshipBtnPos.x && m_cursorPosition.x < m_selectshipBtnPos.x + 250 &&
				m_cursorPosition.y > m_selectshipBtnPos.y && m_cursorPosition.y < m_selectshipBtnPos.y + 40)
			{
				if (m_selectshipBtnSelected == false)
					g_sounds[2]->Play();

				m_selectshipBtnSelected = true;

				if (g_engine->GetInput()->MouseButtonDown(0))
					m_selectshipBtnDown = true;
				else m_selectshipBtnDown = false;

				if (!g_engine->GetInput()->MouseButtonDown(0) && 
					g_engine->GetInput()->PrevMouseButtonDown(0))
				{
					g_sounds[1]->Play();
					m_currScreen = SHIPSELECT;
				}
			}
			else 
			{
				m_selectshipBtnSelected = false;
				m_selectshipBtnDown = false;
			}

			if (m_cursorPosition.x > m_instructionsBtnPos.x && m_cursorPosition.x < m_instructionsBtnPos.x + 250 &&
				m_cursorPosition.y > m_instructionsBtnPos.y && m_cursorPosition.y < m_instructionsBtnPos.y + 40)
			{
				if (m_instructionsBtnSelected == false)
					g_sounds[2]->Play();

				m_instructionsBtnSelected = true;

				if (g_engine->GetInput()->MouseButtonDown(0))
					m_instructionsBtnDown = true;
				else m_instructionsBtnDown = false;

				if (!g_engine->GetInput()->MouseButtonDown(0) && 
					g_engine->GetInput()->PrevMouseButtonDown(0))
				{
					g_sounds[1]->Play();
					m_currScreen = INSTRUCTIONS;
				}
			}
			else 
			{
				m_instructionsBtnSelected = false;
				m_instructionsBtnDown = false;
			}

			if (m_cursorPosition.x > m_creditsBtnPos.x && m_cursorPosition.x < m_creditsBtnPos.x + 250 &&
				m_cursorPosition.y > m_creditsBtnPos.y && m_cursorPosition.y < m_creditsBtnPos.y + 40)
			{
				if (m_creditsBtnSelected == false)
					g_sounds[2]->Play();

				m_creditsBtnSelected = true;

				if (g_engine->GetInput()->MouseButtonDown(0))
					m_creditsBtnDown = true;
				else m_creditsBtnDown = false;

				if (!g_engine->GetInput()->MouseButtonDown(0) && 
					g_engine->GetInput()->PrevMouseButtonDown(0))
				{
					g_sounds[1]->Play();
					m_currScreen = CREDITS;
				}
			}
			else 
			{
				m_creditsBtnSelected = false;
				m_creditsBtnDown = false;
			}

			if (m_cursorPosition.x > m_exitBtnPos.x && m_cursorPosition.x < m_exitBtnPos.x + 250 &&
				m_cursorPosition.y > m_exitBtnPos.y && m_cursorPosition.y < m_exitBtnPos.y + 40)
			{
				if (m_exitBtnSelected == false)
					g_sounds[2]->Play();

				m_exitBtnSelected = true;

				if (g_engine->GetInput()->MouseButtonDown(0))
					m_exitBtnDown = true;
				else m_exitBtnDown = false;

				if (!g_engine->GetInput()->MouseButtonDown(0) && 
					g_engine->GetInput()->PrevMouseButtonDown(0))
				{
					PostQuitMessage(0);
				}
			}
			else 
			{
				m_exitBtnSelected = false;
				m_exitBtnDown = false;
			}
		}break;
		
		case TIMETRIAL:
		{
			if (input->KeyDown(DIK_ESCAPE))
				m_currScreen = MENU;

			if (input->KeyDown(DIK_LEFT) && 
				!input->PrevKeyDown(DIK_LEFT))
			{
				g_currTrack--;
			}
			if (input->KeyDown(DIK_RIGHT) && 
				!input->PrevKeyDown(DIK_RIGHT))
			{
				g_currTrack++;
			}
			if (g_currTrack < 0)
				g_currTrack = NUM_TRACKS-1;
			else if (g_currTrack > NUM_TRACKS-1)
				g_currTrack = 0;

			if (m_cursorPosition.x > m_startBtnPos.x && m_cursorPosition.x < m_startBtnPos.x + 200 &&
				m_cursorPosition.y > m_startBtnPos.y && m_cursorPosition.y < m_startBtnPos.y + 40)
			{
				if (m_startBtnSelected == false)
					g_sounds[2]->Play();

				m_startBtnSelected = true;

				if (g_engine->GetInput()->MouseButtonDown(0))
					m_startBtnDown = true;
				else m_startBtnDown = false;

				if (!g_engine->GetInput()->MouseButtonDown(0) && 
					g_engine->GetInput()->PrevMouseButtonDown(0))
				{
					g_sounds[1]->Play();
					g_engine->ChangeState(TIMETRIAL_STATE);
				}
			}
			else 
			{
				m_startBtnSelected = false;
				m_startBtnDown = false;
			}
		}break;

		case MULTIPLAYER:
		{
			if (input->KeyDown(DIK_ESCAPE))
				m_currScreen = MENU;

			if (input->KeyDown(DIK_LEFT) && 
				!input->PrevKeyDown(DIK_LEFT))
			{
				g_currTrack--;
			}
			if (input->KeyDown(DIK_RIGHT) && 
				!input->PrevKeyDown(DIK_RIGHT))
			{
				g_currTrack++;
			}
			if (g_currTrack < 0)
				g_currTrack = NUM_TRACKS-1;
			else if (g_currTrack > NUM_TRACKS-1)
				g_currTrack = 0;

			if (m_cursorPosition.x > m_hostBtnPos.x && m_cursorPosition.x < m_hostBtnPos.x + 200 &&
				m_cursorPosition.y > m_hostBtnPos.y && m_cursorPosition.y < m_hostBtnPos.y + 40)
			{
				if (m_hostBtnSelected == false)
					g_sounds[2]->Play();

				m_hostBtnSelected = true;

				if (g_engine->GetInput()->MouseButtonDown(0))
					m_hostBtnDown = true;
				else m_hostBtnDown = false;

				if (!g_engine->GetInput()->MouseButtonDown(0) && 
					g_engine->GetInput()->PrevMouseButtonDown(0))
				{
					g_sounds[1]->Play();
					PlayerData data;
					data.ship = g_currShip;
					data.track = g_currTrack;
					if (g_engine->GetNetwork()->Host(m_name, "Session", 6, &data, sizeof(data)))
						g_engine->ChangeState(MULTIPLAYER_STATE);
					else g_sounds[3]->Play();
				}
			}
			else 
			{
				m_hostBtnSelected = false;
				m_hostBtnDown = false;
			}

			if (m_cursorPosition.x > m_joinBtnPos.x && m_cursorPosition.x < m_joinBtnPos.x + 200 &&
				m_cursorPosition.y > m_joinBtnPos.y && m_cursorPosition.y < m_joinBtnPos.y + 40)
			{
				if (m_joinBtnSelected == false)
					g_sounds[2]->Play();

				m_joinBtnSelected = true;

				if (g_engine->GetInput()->MouseButtonDown(0))
					m_joinBtnDown = true;
				else m_joinBtnDown = false;

				if (!g_engine->GetInput()->MouseButtonDown(0) && 
					g_engine->GetInput()->PrevMouseButtonDown(0))
				{
					
					if (strlen(m_ip) < 7)
						g_engine->GetNetwork()->EnumerateSessions();
					else
						g_engine->GetNetwork()->EnumerateSessions(m_ip);
					
					if (g_engine->GetNetwork()->GetSession(0))
					{
						PlayerData data;
						data.ship = g_currShip;
						data.track = 2;
						if (g_engine->GetNetwork()->Join(m_name, 0, &data, sizeof(data)))
							g_engine->ChangeState(MULTIPLAYER_STATE);
					}
				}
			}
			else 
			{
				m_joinBtnSelected = false;
				m_joinBtnDown = false;
			}

			if (m_cursorPosition.x > m_nameTextPos.x && m_cursorPosition.x < m_nameTextPos.x + 256 &&
				m_cursorPosition.y > m_nameTextPos.y && m_cursorPosition.y < m_nameTextPos.y + 40)
			{
				if (m_nameTextActive == false)
					g_sounds[2]->Play();

				m_nameTextActive = true;
				if (g_engine->GetInput()->MouseButtonDown(0))
				{
					strcpy(m_name,"");
				}
			}
			else 
			{
				m_nameTextActive = false;
			}

			if (m_cursorPosition.x > m_ipTextPos.x && m_cursorPosition.x < m_ipTextPos.x + 256 &&
				m_cursorPosition.y > m_ipTextPos.y && m_cursorPosition.y < m_ipTextPos.y + 40)
			{
				if (m_ipTextActive == false)
					g_sounds[2]->Play();
				m_ipTextActive = true;
				if (g_engine->GetInput()->MouseButtonDown(0))
				{
					strcpy(m_ip,"");
				}
			}
			else 
			{
				m_ipTextActive = false;
			}

		}break;

		case SHIPSELECT:
		{
			if (input->KeyDown(DIK_ESCAPE))
				m_currScreen = MENU;

			D3DXMATRIX view;
			D3DXMatrixLookAtLH(&view, 
				&D3DXVECTOR3(0.0f, 150.0f, 500.0f), 
				&D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
				&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &view);

			if (input->KeyDown(DIK_LEFT) && !input->PrevKeyDown(DIK_LEFT))
			{
				g_currShip--;
				m_shipObj.Reset();
			}
			if (input->KeyDown(DIK_RIGHT) && !input->PrevKeyDown(DIK_RIGHT))
			{
				g_currShip++;
				m_shipObj.Reset();
			}

			//m_rotation += 2.0f * dt;
			m_shipObj.Yaw(1.0f * dt);

			if (g_currShip < 0) g_currShip = NUM_SHIPS - 1;
			else if (g_currShip >= NUM_SHIPS) g_currShip = 0;
		}break;

		case INSTRUCTIONS:
		case CREDITS:
		{
			if (input->KeyDown(DIK_ESCAPE))
				m_currScreen = MENU;

		}break;

	}
	
}

void MainMenu::Draw()
{
	LPD3DXSPRITE sprite = g_engine->GetSprite();

	switch(m_currScreen)
	{
		case MENU:
		{
			sprite->Begin(0);
			
			// draw background
			RECT rect = {0, 0, 800, 600};
			sprite->Draw(m_menuTexture, &rect, 0, 0, 0xffffffff);

			// draw play button
			rect.top = 600;
			rect.right = 250;
			rect.bottom = rect.top + 40;
			if (m_timetrialBtnDown)
				sprite->Draw(m_menuTexture, &rect, 0, &m_timetrialBtnPos,  0xffff0022);
			else
				sprite->Draw(m_menuTexture, &rect, 0, &m_timetrialBtnPos, m_timetrialBtnSelected ? 0xffffffff : 0xff555555);

			// draw exit button
			rect.top+= 40;
			rect.bottom+=40;
			if (m_multiplayerBtnDown)
				sprite->Draw(m_menuTexture, &rect, 0, &m_multiplayerBtnPos,  0xffff0022);
			else
				sprite->Draw(m_menuTexture, &rect, 0, &m_multiplayerBtnPos,  m_multiplayerBtnSelected ? 0xffffffff : 0xff555555);

			// draw exit button
			rect.top+= 40;
			rect.bottom+=40;
			if (m_selectshipBtnDown)
				sprite->Draw(m_menuTexture, &rect, 0, &m_selectshipBtnPos,  0xffff0022);
			else
				sprite->Draw(m_menuTexture, &rect, 0, &m_selectshipBtnPos,  m_selectshipBtnSelected ? 0xffffffff : 0xff555555);

			// draw exit button
			rect.top+= 40;
			rect.bottom+=40;
			if (m_instructionsBtnDown)
				sprite->Draw(m_menuTexture, &rect, 0, &m_instructionsBtnPos,  0xffff0022);
			else
				sprite->Draw(m_menuTexture, &rect, 0, &m_instructionsBtnPos,  m_instructionsBtnSelected ? 0xffffffff : 0xff555555);
			

			// credits button
			rect.top+= 40;
			rect.bottom+=40;
			if (m_creditsBtnDown)
				sprite->Draw(m_menuTexture, &rect, 0, &m_creditsBtnPos,  0xffff0022);
			else
				sprite->Draw(m_menuTexture, &rect, 0, &m_creditsBtnPos,  m_creditsBtnSelected ? 0xffffffff : 0xff555555);

			//exit button
			rect.top+= 40;
			rect.bottom+=40;
			if (m_exitBtnDown)
				sprite->Draw(m_menuTexture, &rect, 0, &m_exitBtnPos,  0xffff0022);
			else
				sprite->Draw(m_menuTexture, &rect, 0, &m_exitBtnPos,  m_exitBtnSelected ? 0xffffffff : 0xff555555);
			sprite->End();

			sprite->Begin(D3DXSPRITE_ALPHABLEND);
			sprite->Draw(m_cursorTexture, 0, 0, &m_cursorPosition,0xffffffff);
			sprite->End();
		}break;
		
		case TIMETRIAL:
		{
			sprite->Begin(0);
			RECT rect = {0, 0, 800, 600};
			sprite->Draw(m_timetrialTexture, &rect, 0, 0, 0xffffffff);

			rect.top = 600;
			rect.right = 200;
			rect.bottom += rect.top + 40;
			if (m_startBtnDown)
				sprite->Draw(m_timetrialTexture, &rect, 0, &m_startBtnPos, 0xffff0022);
			else
				sprite->Draw(m_timetrialTexture, &rect, 0, &m_startBtnPos, m_startBtnSelected ? 0xffffffff : 0xff555555);
			sprite->End();
	
			sprite->Begin(D3DXSPRITE_ALPHABLEND);
			sprite->Draw(m_cursorTexture, 0, 0, &m_cursorPosition,0xffffffff);
			sprite->End();

			g_font.Draw(80, 380, 0xffffffff, "Press left and right arrow to change track\nCurrent Track is %i", g_currTrack);
		}break;

		case MULTIPLAYER:
		{
			sprite->Begin(0);
			RECT rect = {0, 0, 800, 600};
			sprite->Draw(m_multiplayerTexture, &rect, 0, 0, 0xffffffff);

			rect.right = 200;
			rect.top = 600;
			rect.bottom = rect.top + 40;
			if (m_hostBtnDown)
				sprite->Draw(m_multiplayerTexture, &rect, 0, &m_hostBtnPos, 0xffff0022);
			else
				sprite->Draw(m_multiplayerTexture, &rect, 0, &m_hostBtnPos, m_hostBtnSelected ? 0xffffffff : 0xff555555);

			rect.top = rect.bottom;
			rect.bottom = rect.top + 40;
			if (m_joinBtnDown)
				sprite->Draw(m_multiplayerTexture, &rect, 0, &m_joinBtnPos, 0xffff0022);
			else
				sprite->Draw(m_multiplayerTexture, &rect, 0, &m_joinBtnPos, m_joinBtnSelected ? 0xffffffff : 0xff555555);

			rect.right = 256;
			rect.top = rect.bottom;
			rect.bottom = rect.top + 40;
			sprite->Draw(m_multiplayerTexture, &rect, 0, &m_nameTextPos, m_nameTextActive ? 0xffffffff : 0xff555555);

			sprite->Draw(m_multiplayerTexture, &rect, 0, &m_ipTextPos, m_ipTextActive ? 0xffffffff : 0xff555555);
	
			
			sprite->End();

			
			g_font.Draw(m_nameTextPos.x, m_nameTextPos.y+3, m_nameTextActive ? 0xffffffff : 0xff555555, m_name);
			g_font.Draw(m_ipTextPos.x, m_ipTextPos.y+3, m_ipTextActive ? 0xffffffff : 0xff555555, m_ip);
	
			g_font.Draw(m_nameTextPos.x-80, m_nameTextPos.y+3, m_nameTextActive ? 0xffffff00 : 0xffffffff, "NAME");
			g_font.Draw(m_ipTextPos.x-48, m_ipTextPos.y+3, m_ipTextActive ? 0xffffff00 : 0xffffffff, "IP");
			if (!m_nameTextActive && !m_ipTextActive)
			{
				sprite->Begin(D3DXSPRITE_ALPHABLEND);
				sprite->Draw(m_cursorTexture, 0, 0, &m_cursorPosition,0xffffffff);
				sprite->End();
			}

			g_font.Draw(80, 380, 0xffffffff, "Press left and right arrow to change track\nCurrent Track is %i", g_currTrack);

		}break;

		case SHIPSELECT:
		{
			g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &m_shipObj.GetMatrix());
			g_shipMesh[g_currShip].Render();
			g_font.Draw(0, 560, 0xffffffff, "Press left and right arrows to select ship");
		}break;

		case INSTRUCTIONS:
		{
			sprite->Begin(0);
			RECT rect = {0, 0, 800, 600};
			sprite->Draw(m_instrTexture, &rect, 0, 0, 0xffffffff);
			sprite->End();
		}break;

		case CREDITS:
		{	
			sprite->Begin(0);
			RECT rect = {0, 0, 800, 600};
			sprite->Draw(m_creditsTexture, &rect, 0, 0, 0xffffffff);
			sprite->End();

		}break;
	}

}


void MainMenu::MsgProc(UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		case WM_CHAR:
		{
			if (m_currScreen == MULTIPLAYER)
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
						g_sounds[4]->Play();
					}
					else if (len < 15 && ch >= ' ' && ch <= 'z')
					{
						m_name[len] = ch;
						m_name[len+1] = '\0';
						g_sounds[4]->Play();
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
						g_sounds[4]->Play();
					}
					else if (len < 15 && (ch == '.' || (ch >='0' && ch <= '9')))
					{
						m_ip[len] = ch;
						m_ip[len+1] = '\0';
						g_sounds[4]->Play();
					}
				}
			}
		}
			break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////