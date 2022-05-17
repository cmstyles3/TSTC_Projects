#include "DuckShootGame.h"
#include "main.h"

DuckShootGameState::DuckShootGameState() : State(DUCKSHOOTGAME_STATE)
{
	//m_color = 0xffffff00;
	//m_texture = 0;
}

void DuckShootGameState::Load()
{
	m_currentGameState = START;
	m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "duck.dds", &m_duckTex);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "cross-hair.dds", &m_crosshairTex);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "duck.png", &m_bkgdTex);

	g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
	// Create projection matrix
	D3DXMatrixPerspectiveFovLH(
		&m_proj, 
		D3DX_PI/4, 
		(float)g_engine->GetWidth()/(float)g_engine->GetHeight(), 
		1.0f, 50.0f);

	g_engine->GetDevice()->SetTransform(D3DTS_PROJECTION, &m_proj);

	// Create view matrix
	D3DXMatrixLookAtLH(
		&m_view, 
		&D3DXVECTOR3(0.0f, 0.0f, -40.0f), 
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	
	g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &m_view);

	D3DXCreateTextureFromFile(g_engine->GetDevice(), "ammo.bmp", &m_ammoTex);
	D3DXCreateBox(g_engine->GetDevice(),5, 5, 5, &m_boxAmmo, 0);
	m_boxAmmo->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_PT, g_engine->GetDevice(), &m_boxAmmo);

	VertexPT *v;
	m_boxAmmo->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < m_boxAmmo->GetNumVertices(); i+=4)
	{
		v[i].u = 0.0f;
		v[i].v = 0.0f;
		v[i+1].u = 1.0f;
		v[i+1].v = 0.0f;
		v[i+2].u = 1.0f;
		v[i+2].v = 1.0f;
		v[i+3].u = 0.0f;
		v[i+3].v = 1.0f;
	}
	m_boxAmmo->UnlockVertexBuffer();


	m_duckSound = new Sound("duck.wav");
	m_gunSound = new Sound("gun.wav");

	m_displayHelp = false;

	m_font = new Font;

	ZeroMemory(&m_explosions, sizeof(Explosion) * 10);

}

void DuckShootGameState::Close()
{
	SAFE_RELEASE(m_ammoTex);
	SAFE_RELEASE(m_boxAmmo);
	SAFE_DELETE(m_font);
	SAFE_DELETE(m_gunSound);
	SAFE_DELETE(m_duckSound);
	SAFE_RELEASE(m_bkgdTex);
	SAFE_RELEASE(m_duckTex);
	SAFE_RELEASE(m_crosshairTex);
}

void DuckShootGameState::OnLostDevice()
{
	m_font->OnLostDevice();
}

void DuckShootGameState::OnResetDevice()
{
	m_font->OnResetDevice();
	g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
	g_engine->GetDevice()->SetTransform(D3DTS_PROJECTION, &m_proj);
	g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &m_view);
}

void DuckShootGameState::Update(float dt)
{
	Input *input = g_engine->GetInput();

	if ( input->KeyDown(DIK_H) && 
		!input->PrevKeyDown(DIK_H))
		m_displayHelp = !m_displayHelp;

	if (m_displayHelp == false)
	{
		switch(m_currentGameState)
		{
			case START:
				{
				if (input->KeyDown(DIK_ESCAPE) && !input->PrevKeyDown(DIK_ESCAPE))
					g_engine->ChangeState(MENU_STATE);
				if (input->KeyDown(DIK_SPACE))
				{
					ZeroMemory(&m_explosions, sizeof(Explosion) * MAX_EXPLOSIONS);
					ZeroMemory(m_ducks, sizeof(Duck) * MAX_DUCKS);
					m_duckPos.x = -100;
					m_numHits = 0;
					m_numShots = 0;
					m_currentGameState = PLAY;
				}

				m_position.x += input->JoystickDX() * 10.0f * dt;
				m_position.y += input->JoystickDY() * 10.0f * dt;
				}break;
			case PLAY:
				{
				if (input->KeyDown(DIK_ESCAPE))
					m_currentGameState = START;
				m_position.x += input->MouseDX();
				m_position.y += input->MouseDY();

				if (m_position.x < 0) m_position.x = 0;
				else if (m_position.x > 799) m_position.x = 799;
				if (m_position.y < 0) m_position.y = 0;
				else if (m_position.y > 599) m_position.y = 599;

				m_target = false;
				for (int i = 0; i < MAX_DUCKS; i++)
				{
					if (m_position.x > m_ducks[i].pos.x && m_position.x < m_ducks[i].pos.x + 64 && 
						m_position.y > m_ducks[i].pos.y && m_position.y < m_ducks[i].pos.y + 64)
					{
						m_target = true;
						
						
						
						
					}
				}
				static DWORD gunTimer = GetTickCount();
				if (input->MouseButtonDown(0) && GetTickCount() - gunTimer > 600)
				{
					m_numShots++;
					gunTimer = GetTickCount();
					m_gunSound->Play();
					for (int i = 0; i < MAX_DUCKS; i++)
					{
						if (m_position.x > m_ducks[i].pos.x && m_position.x < m_ducks[i].pos.x + 64 && 
							m_position.y > m_ducks[i].pos.y && m_position.y < m_ducks[i].pos.y + 64)
						{
							m_numHits++;
							m_duckSound->Play();
							m_ducks[i].state = DYING;
							StartExplosion(m_position.x, m_position.y);	
							break;
						}
					}
				}
				if (m_numShots >= 10) 
				{
					m_currentGameState = RESULT;
					m_boxRot = 0.0f;
				}

				MoveDucks(dt);
				MoveExplosions(dt);
				}
				break;
			case RESULT:
				if (input->KeyDown(DIK_ESCAPE))
					m_currentGameState = START;
				m_boxRot += 1.0f * dt;
				break;
		}
	}
	
}

void DuckShootGameState::Draw()
{
	g_engine->GetSprite()->Begin(0);
	RECT rect = {0,0,800,600};
	if (!m_displayHelp)
		g_engine->GetSprite()->Draw(m_bkgdTex, &rect, 0, &D3DXVECTOR3(0,0,1), 0xffffffff);
	g_engine->GetSprite()->End();
	if (m_displayHelp == false)
	{
		switch(m_currentGameState)
		{
			case START:
				{
					m_font->Draw(200,200, "Press space bar to start new game", 0xffffffff);
				
				}
				break;
			case PLAY:
			
			{
				g_engine->GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);

				for (int i = 0; i < MAX_DUCKS; i++)
				{
					g_engine->GetSprite()->Draw(m_duckTex, 0, 0, &m_ducks[i].pos, 0xffffffff);
				}

				DrawExplosions();
				g_engine->GetSprite()->Draw(m_crosshairTex, 0, &D3DXVECTOR3(32.0f, 32.0f, 0.0f), &m_position, m_target ? 0xffff0000 : 0xffffffff);
				g_engine->GetSprite()->End();
				char buffer[90];
				sprintf(buffer, "Number of hits = %i, Number of Shots = %i", m_numHits, m_numShots);
				m_font->Draw(0,0,buffer, 0xffffffff);
				
			}break;

			case RESULT:
				if (m_numHits >= 10)
				{
					D3DXMATRIX I;
					D3DXMatrixIdentity(&I);
					
					D3DXMatrixRotationY(&I, m_boxRot);
					g_engine->GetDevice()->SetTexture(0, m_ammoTex);
					g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &I);
					m_boxAmmo->DrawSubset(0);
					m_font->Draw(0,0,"That was freakin awesome!", 0xffffffff);
					m_font->Draw(100, 300, "You win a box of ammo", 0xffffffff);
				}
				else 
				{
					m_font->Draw(0,0,"That wasn't very good.", 0xffffffff);

				}

				char buffer[90];
				sprintf(buffer, "You got %i hits", m_numHits);
				m_font->Draw(0, 40, buffer, 0xffffffff);
				break;
		
		}
	}
	else
	{
		m_font->Draw(0,0, "Instructions\n\
						  Initially you can either press space bar to start game or escape to exit\n\
						  Once you have started a game you need to use the mouse to control the\n \
						  movement of the cross-hair.  The left mouse button will fire the gun.\n\
						  Whenever a duck is under your cross-hair it will change from yellow to red,\n\
						  indicating that you have a target. When you have finished shooting your last\n\
						  bullet, you will get your results.  This is were you can see how many ducks you\n\
						  hit and if you recieved a prize or not.  At this screen you can hit the escape key\n\
						  to go back to the start.\n\n\n\
						  Press 'H' again to go back to the game.", 0xffffffff);
	}
}

void DuckShootGameState::MoveDucks(float dt)
{
	for (int i = 0; i < MAX_DUCKS; i++)
	{
		if (m_ducks[i].state == DEAD)
		{
			m_ducks[i].pos = D3DXVECTOR3(rand()%60 + 800, rand()%(600-64), 0);
			m_ducks[i].speed = (rand()%5000 + 8000) * 0.05f;
			m_ducks[i].state = ALIVE;
		}
		else if (m_ducks[i].state == DYING)
		{
			m_ducks[i].pos.y += (600.0f) * dt;
		}
		else if (m_ducks[i].state == ALIVE)
		{
			m_ducks[i].pos.x -= m_ducks[i].speed * dt;
		}

		if (m_ducks[i].pos.x < -64 || m_ducks[i].pos.y > 664)
		{
			m_ducks[i].state = DEAD;
		}
	}
}

void DuckShootGameState::StartExplosion(float x, float y)
{
	for (int i = 0; i < 10; i++)
	{
		if (!m_explosions[i].active)
		{
			m_explosions[i].life = 6;
			m_explosions[i].active = true;
			for (int j = 0; j < MAX_PARTICLES; j++)
			{
				m_explosions[i].particles[j].x = x;
				m_explosions[i].particles[j].y = y;
				m_explosions[i].particles[j].xv = rand()%2 ? ((rand()%200+1) * 0.5f) : -((rand()%200+1) * 0.5f);
				m_explosions[i].particles[j].yv = rand()%2 ? ((rand()%200+1) * 0.5f) : -((rand()%200+1) * 0.5f);
				m_explosions[i].particles[j].color = rand()%2 ? D3DCOLOR_XRGB(255,255,255) : D3DCOLOR_XRGB(255, 0, 0);
				
			}
			break;
		}
	}
}

void DuckShootGameState::MoveExplosions(float dt)
{
	for (int i = 0; i < 10; i++)
	{
		if (m_explosions[i].active)
		{
			for (int j = 0; j < MAX_PARTICLES; j++)
			{
				m_explosions[i].particles[j].x += m_explosions[i].particles[j].xv * dt;
				m_explosions[i].particles[j].y += m_explosions[i].particles[j].yv * dt;

				m_explosions[i].particles[j].yv += 90.0f * dt;
			}
			m_explosions[i].life -= dt;
			if (m_explosions[i].life <= 0) 
			{
				m_explosions[i].active = false;
				
			}
		}
	}
}

void DuckShootGameState::DrawExplosions()
{
	for (int i = 0; i < 10; i++)
	{
		if (m_explosions[i].active)
		{
			for (int j = 0; j < MAX_PARTICLES; j++)
			{
				D3DXVECTOR3 position(
					m_explosions[i].particles[j].x,
					m_explosions[i].particles[j].y, 0.0f);

				RECT rect = {32, 32, 34, 34};
				g_engine->GetSprite()->Draw(m_duckTex, &rect, 0, &position, 
					m_explosions[i].particles[j].color);
				
			}
			
		}
	}
}