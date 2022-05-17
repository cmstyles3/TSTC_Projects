#include "DartThrowGame.h"
#include "main.h"

DartThrowGameState::DartThrowGameState() : State(DARTTHROWGAME_STATE)
{
}

void DartThrowGameState::Load()
{
	m_currState = START;
	m_camera.SetPosition(0.0f, 0.0f, -100.0f);
	m_camera.SetLook(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	m_camera.SetRight(D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	m_camera.SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
	g_engine->GetDevice()->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	D3DXMatrixPerspectiveFovLH(&m_proj, D3DX_PI/4, 800.0f/600.0f, 1.0f, 1000.0f);
	g_engine->GetDevice()->SetTransform(D3DTS_PROJECTION, &m_proj);

	D3DXCreateTextureFromFile(g_engine->GetDevice(), "corkboard.bmp", &m_corkboardTex);

	D3DXCreateBox(g_engine->GetDevice(), 50, 50, 10, &m_corkboardMesh, 0);
	m_corkboardMesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_PT, g_engine->GetDevice(), &m_corkboardMesh);

	VertexPT *v;
	m_corkboardMesh->LockVertexBuffer(0, (void**)&v);
	for (int i = 0; i < m_corkboardMesh->GetNumVertices(); i+=4)
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
	m_corkboardMesh->UnlockVertexBuffer();

	D3DXCreateSphere(g_engine->GetDevice(), BALLOON_RADIUS, 10, 10, &m_balloonMesh, 0);
	m_balloonMesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_PD, g_engine->GetDevice(), &m_balloonMesh);

	VertexPD *vpd;
	D3DXCreateSphere(g_engine->GetDevice(), .1, 10, 10, &m_dartMesh, 0);
	m_dartMesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_PD, g_engine->GetDevice(), &m_dartMesh);
	m_dartMesh->LockVertexBuffer(0, (void**)&vpd);
	for (int i =0; i < m_dartMesh->GetNumVertices(); i++)
	{
		vpd[i].color = D3DCOLOR_XRGB(255,255,255);
	}
	m_dartMesh->UnlockVertexBuffer();

	m_yaw = m_pitch = 0;

	m_balloonPop = new Sound("balloonpop.wav");

	m_font = new Font();

	D3DXCreateTextureFromFile(g_engine->GetDevice(), "countyfair.bmp", &m_bkgdTex);

	D3DXCreateSphere(g_engine->GetDevice(), 10.0f, 20, 20, &m_prizeMesh, 0);
	m_prizeMesh->CloneMeshFVF(0,D3DFVF_PNT,g_engine->GetDevice(),&m_prizeMesh);
		
	VertexPNT *vpnt;
	m_prizeMesh->LockVertexBuffer(0, (void**)&vpnt);
	
	for(int i = 0; i < m_prizeMesh->GetNumVertices(); i++)
	{
		vpnt[i].v = -vpnt[i].ny / 2 + 0.5f;
		vpnt[i].u = vpnt[i].nx / 2 + 0.5f;
	}
	m_prizeMesh->UnlockVertexBuffer();
	m_prizeMesh->CloneMeshFVF(D3DXMESH_MANAGED, D3DFVF_PT, g_engine->GetDevice(), &m_prizeMesh);
	D3DXCreateTextureFromFile(g_engine->GetDevice(), "face.bmp", &m_prizeTex);

	m_displayHelp = false;
}

void DartThrowGameState::Close()
{
	SAFE_RELEASE(m_prizeTex);
	SAFE_RELEASE(m_prizeMesh);
	SAFE_RELEASE(m_bkgdTex);
	SAFE_DELETE(m_font);
	SAFE_DELETE(m_balloonPop);
	SAFE_RELEASE(m_dartMesh);
	SAFE_RELEASE(m_corkboardMesh);
	SAFE_RELEASE(m_balloonMesh);
	SAFE_RELEASE(m_corkboardTex);
}

void DartThrowGameState::OnResetDevice()
{
	g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
	g_engine->GetDevice()->SetTransform(D3DTS_PROJECTION, &m_proj);
}

void DartThrowGameState::Update(float dt)
{
	Input *input = g_engine->GetInput();

	if (input->KeyDown(DIK_H) && !input->PrevKeyDown(DIK_H))
		m_displayHelp = !m_displayHelp;
	if (!m_displayHelp)
	{
		switch(m_currState)
		{
			case START:
				if ( input->KeyDown(DIK_ESCAPE) && 
					!input->PrevKeyDown(DIK_ESCAPE))
					g_engine->ChangeState(MENU_STATE);

				if ( input->KeyDown(DIK_SPACE))
				{
					m_currState = PLAY;
					InitBalloons();
					m_canThrowDart = true;
					m_throws = 3;
					m_dart.pos = D3DXVECTOR3(0.0f, 0.0f, 100.0f);
					m_numBalloonsPopped = 0;
					//RESET STUFF HERE
				}

				break;
			case PLAY:
				{
				if ( input->KeyDown(DIK_ESCAPE) &&
					!input->PrevKeyDown(DIK_ESCAPE))
					m_currState = START;
				m_pitch += input->MouseDY() * 0.0025f;
				m_yaw += input->MouseDX() * 0.0025f;

				if (m_pitch < D3DX_PI/8 && m_pitch > -D3DX_PI/8)
				{
					
					m_camera.Pitch(input->MouseDY() * 0.0025f);
				}
				if (m_yaw < D3DX_PI/8 && m_yaw > -D3DX_PI/8)
					m_camera.Yaw(input->MouseDX() * 0.0025f);

				if ( input->MouseButtonDown(0) && m_canThrowDart)
				{
					m_canThrowDart = false;
					m_throws--;
					D3DXVECTOR3 look;
					m_camera.GetLook(look);
					StartDart(look.x,look.y, look.z);

				}
				
				MoveDart(dt);

				D3DXMATRIX V;
				m_camera.GetViewMatrix(&V);
				g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &V);
				}
				break;
			case RESULT:
				{
				if (input->KeyDown(DIK_ESCAPE))
				{
					m_currState = START;
				}

				m_rotPrize += 2.0f * dt;

				D3DXMATRIX V;
				D3DXMatrixLookAtLH(&V, &D3DXVECTOR3(0,0,-100), &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,1,0));
				g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &V);

				}
				break;
		}
	}
	
}

void DartThrowGameState::Draw()
{
	if (!m_displayHelp)
	{
		RECT rect = { 0, 0, 800, 600 };
		g_engine->GetSprite()->Begin(0);
		g_engine->GetSprite()->Draw(m_bkgdTex, &rect, 0, &D3DXVECTOR3(0,0,1), 0xffffffff);
		g_engine->GetSprite()->End();
		switch(m_currState)
		{
			case START:
				m_font->Draw(0,0,"Press the space bar to start a new game", 0xffffffff);
				break;
			case PLAY:
			
				{
				D3DXMATRIX I;
				D3DXMatrixIdentity(&I);
				g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &I);
				g_engine->GetDevice()->SetTexture(0, m_corkboardTex);
				m_corkboardMesh->DrawSubset(0);
				DrawBalloons();
				DrawDart();
				if (m_currState == RESULT)
				{
					
				}
				}break;
			case RESULT:
				{
					if (m_numBalloonsPopped >= 3)
					{
						m_font->Draw(0,0,"Good Job, you popped three balloons", 0xffffffff);
						m_font->Draw(0, 220, "You've won a balloon with a face on it", 0xffffffff);
						D3DXMATRIX I;
						D3DXMatrixIdentity(&I);
						D3DXMatrixRotationY(&I, m_rotPrize);
						g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &I);
						g_engine->GetDevice()->SetTexture(0, m_prizeTex);
						m_prizeMesh->DrawSubset(0);
					}
					else 
					{
						char buffer[80];
						sprintf(buffer, "Wow, you only popped %i. I've seen much better.", m_numBalloonsPopped);
		
						m_font->Draw(0,0,buffer, 0xffffffff);
					}

					m_font->Draw(0,560, "Press the escape key to continue", 0xffffffff);

					
					
				}break;
			
		}
	}
	else
	{
		m_font->Draw(0,0,"Instructions\n\
						 Initially you can press space bar to start new game or\n\
						 escape to exit back to menu.  Once you have started a game\n\
						 you can use the mouse to rotate camera and use the left mouse\n\
						 button to throw a dart, which will be thrown right from the center\n\
						 of the screen. Also, if you would like to exit out of the current\n\
						 game you can press escape. Once you have thrown 3 darts, you will\n\
						 see your results and prize if you popped 3 balloons.\n\n\
						 Press 'H' to go back to the game", 0xffffffff);
	}
}

void DartThrowGameState::InitBalloons()
{
	int startx = -10;
	int starty = 10;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			int index = i * 5 + j;

			m_balloons[index].color = D3DCOLOR_XRGB(rand()%255, rand()%255, rand()%255);
			m_balloons[index].popped = false;

	
			m_balloons[index].pos.x = i * 10 - (2*10);
			m_balloons[index].pos.y = j * 10 - (2*10);
			m_balloons[index].pos.z = -10;


		}
		
	}
}

void DartThrowGameState::DrawBalloons()
{
	VertexPD *v;
	for (int i = 0; i < MAX_BALLOONS; i++)
	{	
		if (!m_balloons[i].popped)
		{
			m_balloonMesh->LockVertexBuffer(0,(void**)&v);
			for (int j = 0; j < m_balloonMesh->GetNumVertices(); j++)
			{
				v[j].color = m_balloons[i].color;
			}
			m_balloonMesh->UnlockVertexBuffer();

			D3DXMATRIX T;
			D3DXMatrixTranslation(&T, m_balloons[i].pos.x, m_balloons[i].pos.y, m_balloons[i].pos.z );
			g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &T);

			m_balloonMesh->DrawSubset(0);
		}
	}
}


void DartThrowGameState::StartDart(float dirx, float diry, float dirz)
{
	m_dart.pos = D3DXVECTOR3(0,0, -100.0f);
	m_dart.vel = D3DXVECTOR3(dirx, diry, dirz) * 300.0f;
}

void DartThrowGameState::MoveDart(float dt)
{
	m_dart.pos.x += m_dart.vel.x * dt;
	m_dart.pos.y += m_dart.vel.y * dt;
	m_dart.pos.z += m_dart.vel.z * dt;

	if (m_dart.pos.z > 200.0f) 
	{
		m_canThrowDart = true;
		if (m_throws <= 0) 
		{
			m_currState = RESULT;
			
		}
	}
	for (int i = 0; i < MAX_BALLOONS; i++)
	{
		if (m_balloons[i].popped == false)
		{
			float x = m_dart.pos.x - m_balloons[i].pos.x;
			float y = m_dart.pos.y - m_balloons[i].pos.y;
			float z = m_dart.pos.z - m_balloons[i].pos.z;

			float dist = sqrt(x*x + y*y + z*z);

			if (dist < BALLOON_RADIUS)
			{
				m_balloonPop->Play();
				m_numBalloonsPopped++;
				m_balloons[i].popped = true;
				if (m_throws <= 0) 
				{
					m_currState = RESULT;
					m_rotPrize = 0;
				}
			}
		}
	}
}

void DartThrowGameState::DrawDart()
{
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, m_dart.pos.x, m_dart.pos.y, m_dart.pos.z);
	g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &T);
	m_dartMesh->DrawSubset(0);
}