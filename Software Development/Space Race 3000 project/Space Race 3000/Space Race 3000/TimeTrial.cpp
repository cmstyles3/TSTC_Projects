#include "main.h"

TimeTrial::TimeTrial() : State(TIMETRIAL_STATE)
{
}

void TimeTrial::Load()
{
	m_camera.SetPosition(0.0f, 40000.0f, 0.0f);
	m_camera.SetLook(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	m_camera.SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_camera.SetRight(D3DXVECTOR3(1.0f, 0.0f, 0.0f));

	SetFog(10.0f, 100000.0f, g_track[g_currTrack].color);
	m_color = g_track[g_currTrack].color;

	m_currState = START;
	
}

void TimeTrial::Close()
{
	g_engine->GetSoundSystem()->GetPerformance()->Stop(g_sounds[9]->GetSegment(), NULL, NULL, NULL);
}

void TimeTrial::OnLostDevice()
{
}

void TimeTrial::OnResetDevice()
{
	SetFog(10.0f, 100000.0f, g_track[g_currTrack].color);
}

void TimeTrial::Update(float dt)
{
	static bool two = false, one = false;

	Input *input = g_engine->GetInput();
	if (input->KeyDown(DIK_ESCAPE))
				g_engine->ChangeState(MAINMENU_STATE);
	switch(m_currState)
	{
		case START:
		{
			
			if (input->KeyDown(DIK_SPACE))
				m_currState = INITGAME;
			m_camera.Yaw(0.05f*dt);
			D3DXMATRIX V;
			m_camera.GetViewMatrix(&V);
			g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &V);

		}break;

		case INITGAME:
		{
			m_playerShip.SetPosition(g_track[g_currTrack].rings[0].GetPosition());
			m_playerShip.SetRight   (g_track[g_currTrack].rings[0].GetRight());
			m_playerShip.SetUp      (g_track[g_currTrack].rings[0].GetUp());
			m_playerShip.SetLook    (g_track[g_currTrack].rings[0].GetLook());

			m_shipSpeed = 0.0f;
			m_throttle = CRUISE_THROTTLE;
			m_raceTimer = 0.0f;
			m_countDown = 4.0f;
			m_currState = COUNTDOWN;
			m_currRing = 1;
			two = one = false;
			g_sounds[5]->Play();
		}break;

		case COUNTDOWN:
		{
	
			m_countDown -= dt;
			
			if (m_countDown <= 3.0f && !two)
			{
				g_sounds[6]->Play();
				two = true;
			}
			if (m_countDown <= 2.0f && !one)
			{
				g_sounds[7]->Play();
				one = true;
			}
			if (m_countDown < 1.0f)
			{
				m_currState = PLAYGAME;
				g_sounds[8]->Play();
				g_sounds[9]->Play(true);
				//m_shipSpeed = 0.0f;
			}
			D3DXVECTOR3 shipPos = m_playerShip.GetPosition();
			m_camera.SetLook(m_playerShip.GetLook());
			m_camera.SetRight(m_playerShip.GetRight());
			m_camera.SetUp(m_playerShip.GetUp());


			D3DXVECTOR3 targetPos = (
			shipPos + m_playerShip.GetUp() * 100.0f - m_playerShip.GetLook() * 500.0f);
			
			D3DXMATRIX V;
			m_camera.UpdateThirdPerson(targetPos, &V, dt);
			g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &V);

				
		}break;

		case PLAYGAME:
		{
			m_raceTimer += dt;
			m_camera.Walk(m_shipSpeed * dt);
			m_playerShip.MoveOnZ(m_shipSpeed * dt);

			if (input->KeyDown(DIK_LEFT))
			{
				m_playerShip.Roll(ROLL_SPEED * dt);
			}
			if (input->KeyDown(DIK_RIGHT))
			{
				m_playerShip.Roll(-ROLL_SPEED * dt);
			}
			if (input->KeyDown(DIK_UP))
			{
				m_playerShip.Pitch(PITCH_SPEED * dt);
			}
			if (input->KeyDown(DIK_DOWN))
			{
				m_playerShip.Pitch(-PITCH_SPEED * dt);
			}
			if (input->KeyDown(DIK_A))
			{
				m_playerShip.Yaw(-YAW_SPEED * dt);
			}
			if (input->KeyDown(DIK_D))
			{
				m_playerShip.Yaw(YAW_SPEED * dt);
			}
			
			//if (input->KeyDown(DIK_W))
			//{
			//	m_players[0].object.MoveOnZ(SHIP_SPEED * dt);
			//	m_camera.Walk(SHIP_SPEED * dt);
			//}
			//if (input->KeyDown(DIK_S))
			//{
			//	m_players[0].object.MoveOnZ(-SHIP_SPEED * dt);
			//	m_camera.Walk(-SHIP_SPEED * dt);
			//}
		////////////////////////////////////////////////////////////////////////////////////////////////
		// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////
		// PROGRAMMED BY JUSTIN KNOX /////////////////// B E G I N N I N G /////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////
			if (m_shipSpeed > CRUISE_SHIP_SPEED && m_throttle == CRUISE_THROTTLE)
			{
				m_shipSpeed -= CRUISE_ACCEL_DECEL * dt;
				if (m_shipSpeed < CRUISE_SHIP_SPEED) 
					m_shipSpeed = CRUISE_SHIP_SPEED;
			}
			if (m_shipSpeed < CRUISE_SHIP_SPEED && m_throttle == CRUISE_THROTTLE)
			{
				m_shipSpeed += CRUISE_ACCEL_DECEL * dt;
				if (m_shipSpeed > CRUISE_SHIP_SPEED) 
					m_shipSpeed = CRUISE_SHIP_SPEED;
			}
			if (input->KeyDown(DIK_W) && m_throttle != SLOWDOWN_THROTTLE)
			{
				if (m_shipSpeed > MAX_SHIP_SPEED)
				{
					m_throttle = CRUISE_THROTTLE;
				}
				if (m_shipSpeed < MAX_SHIP_SPEED)
				{
					m_shipSpeed += SHIP_ACCEL * dt;
					if (m_shipSpeed > MAX_SHIP_SPEED)
						m_shipSpeed = MAX_SHIP_SPEED;

					m_throttle = SPEEDUP_THROTTLE;
				}
				
			}
			if (input->KeyDown(DIK_S) && m_throttle != SPEEDUP_THROTTLE)
			{
				if (m_shipSpeed < MIN_SHIP_SPEED)
				{
					m_throttle = CRUISE_THROTTLE;
				}
				if (m_shipSpeed > MIN_SHIP_SPEED)
				{
					m_shipSpeed -= SHIP_DECEL * dt;
					if (m_shipSpeed < MIN_SHIP_SPEED)
						m_shipSpeed = MIN_SHIP_SPEED;
					m_throttle = SLOWDOWN_THROTTLE;
				}
				
				
			}
			m_throttle = CRUISE_THROTTLE;

			// COLLISION DETECTION ----------------------------------
			D3DXVECTOR3 pos = m_playerShip.GetPosition();
			float width     = g_terrain[g_track[g_currTrack].terrainId].GetWidth();
			float depth     = g_terrain[g_track[g_currTrack].terrainId].GetDepth();
			float maxHeight = g_terrain[g_track[g_currTrack].terrainId].GetMaxHeight();

			if (pos.x < -width/2.0f)
				pos.x = -width/2.0f;
			if (pos.x > width/2.0f)
				pos.x = width/2.0f;
			if (pos.z < -depth/2.0f)
				pos.z = -depth/2.0f;
			if (pos.z > depth/2.0f)
				pos.z = depth/2.0f;
			if (pos.y > maxHeight*1.3f)
				pos.y = maxHeight*1.3f;
	
			float terrainHeight = g_terrain[g_track[g_currTrack].terrainId].GetHeight(pos.x, pos.z);
			float x, y, z;
			m_camera.GetPosition(x, y, z);
			if (pos.y < terrainHeight && terrainHeight != -1)
			{
				m_shipSpeed = 1000.0f;
				pos.y = y  = terrainHeight;
				m_camera.SetPosition(x, pos.y, z);
			}
			m_playerShip.SetPosition(pos);

			
			D3DXVECTOR3 shipPos = m_playerShip.GetPosition();
			m_camera.SetLook(m_playerShip.GetLook());
			m_camera.SetRight(m_playerShip.GetRight());
			m_camera.SetUp(m_playerShip.GetUp());


			if (abs(shipPos.x - g_track[g_currTrack].rings[m_currRing].GetPosition().x) < 450.0f && 
				abs(shipPos.y - g_track[g_currTrack].rings[m_currRing].GetPosition().y) < 450.0f && 
				abs(shipPos.z - g_track[g_currTrack].rings[m_currRing].GetPosition().z) < 450.0f)
			{
				g_sounds[0]->Play();
				m_currRing++;
				if (m_currRing >= g_track[g_currTrack].rings.size())
				{
					m_currRing = 0;
					m_currState = RESULTS;
				}
			}
			//---------------------------------------------------------------------------

			D3DXVECTOR3 targetPos = (
			shipPos + m_playerShip.GetUp() * 100.0f - m_playerShip.GetLook() * 500.0f);
			
			D3DXMATRIX V;
			m_camera.UpdateThirdPerson(targetPos, &V, dt);
			g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &V);
			
		}break;

		case RESULTS:
		{
		}break;
	}
	

	
}

void TimeTrial::Draw()
{
	D3DXMATRIX T;
	D3DXMatrixIdentity(&T);
	g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &T);
	g_terrain[g_track[g_currTrack].terrainId].Draw();

	switch(m_currState)
	{
		case START:
		{
			
			for (int i = 0; i < g_track[g_currTrack].rings.size(); i++)
			{
				g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &g_track[g_currTrack].rings[i].GetMatrix());
				g_ringMesh.Render();//DrawSubset(0);
			}
			g_font.Draw(0,0,0xffffffff, "this is timetrial mode");
			g_font.Draw(180, 560, 0xffffffff, "Press space bar to start race");
		}break;

		case COUNTDOWN:
		case PLAYGAME:
		{
			
			g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &m_playerShip.GetMatrix());
			g_shipMesh[g_currShip].Render();

			if (m_currState == COUNTDOWN)
			{
				g_font.Draw(384, 384, 0xffffffff, "%i", (int)m_countDown);
			}
			else
			{
				g_font.Draw(0,0,0xffffffff,"%i", (int)m_raceTimer);
				g_font.Draw(0, 560, 0xffffffff, "Speed %i MPH", (int)(m_shipSpeed/10.0f));
			}

			D3DXMATRIX S;
			D3DXMatrixScaling(&S, 1.5f, 1.5f, 1.5f);
			g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &(S * g_track[g_currTrack].rings[m_currRing].GetMatrix()));
			g_ringMesh.Render();
			if (m_currRing+1 < g_track[g_currTrack].rings.size())
			{
				g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &g_track[g_currTrack].rings[m_currRing+1].GetMatrix());
				g_ringMesh.Render();
			}


			g_engine->GetDevice()->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
			g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, false);
			static Object3D object;
			float x, y, z;
			D3DXVECTOR3 look, up, pos;
			m_camera.GetPosition(x, y, z);
			m_camera.GetLook(look);
			m_camera.GetUp(up);

			look = m_playerShip.GetLook();
			up = m_playerShip.GetUp();
			pos = m_playerShip.GetPosition();
			
			object.SetPosition(
				D3DXVECTOR3(
				x + m_camera.GetRealLook().x * 400.0f + m_camera.GetRealUp().x*130.0f,
				y + m_camera.GetRealLook().y * 400.0f + m_camera.GetRealUp().y*130.0f, 
				z + m_camera.GetRealUp().z*130.0f+m_camera.GetRealLook().z * 400.0f));

			look = g_track[g_currTrack].rings[m_currRing].GetPosition() - m_playerShip.GetPosition();
			D3DXVec3Normalize(&look, &look);
			object.SetLook(look);
			
			if (m_currState == PLAYGAME)
			{
				g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &object.GetMatrix());
				g_arrowMesh.Render();
			}
			g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, true);
		}break;

		case RESULTS:
		{
			g_font.Draw(0, 0, 0xffffffff, "You finished the race in %.3f seconds", m_raceTimer);
		}break;
	}
}