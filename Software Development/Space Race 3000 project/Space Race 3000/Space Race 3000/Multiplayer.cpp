#include "Main.h"

Multiplayer::Multiplayer() : State(MULTIPLAYER_STATE)
{
}

void Multiplayer::Load()
{
	m_camera.SetPosition(D3DXVECTOR3(0.0f,40000.0f, 0.0f));
	m_camera.SetLook(D3DXVECTOR3(0.0f, 0.0f, 1.0f));
	m_camera.SetUp(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	m_camera.SetRight(D3DXVECTOR3(1.0f, 0.0f, 0.0f));

	m_color = g_track[g_currTrack].color;
	SetFog(10.0f, 100000.0f, g_track[g_currTrack].color);

	m_locked = false;
	m_currState = START;
	loaded = true;
	g_currTrack = 0;
}

void Multiplayer::Close()
{
	g_engine->GetNetwork()->Close();
	m_players.clear();
	g_engine->GetSoundSystem()->GetPerformance()->Stop(g_sounds[9]->GetSegment(), NULL, NULL, NULL);
}

void Multiplayer::OnLostDevice()
{
}

void Multiplayer::OnResetDevice()
{
	m_color = g_track[g_currTrack].color;
	SetFog(10.0f, 100000.0f, g_track[g_currTrack].color);
}

void Multiplayer::Update(float dt)
{
	static bool two = false, one = false;
	Input *input = g_engine->GetInput();

	if (g_engine->GetInput()->KeyDown(DIK_ESCAPE))
		g_engine->ChangeState(MAINMENU_STATE);

	if (g_engine->GetNetwork()->GetLocalID() == g_engine->GetNetwork()->GetHostID())
		loaded = true;

	if (loaded == false)
	{
		static DWORD time = timeGetTime();
		if (time < timeGetTime() - 20)
		{
			NetworkMessage msg;
			msg.dpnid = g_engine->GetNetwork()->GetLocalID();
			msg.msgid = MSGID_REQUEST_TRACK;
			g_engine->GetNetwork()->Send(&msg, sizeof(NetworkMessage), g_engine->GetNetwork()->GetHostID());
			time = timeGetTime();
		}
		return;
	}

	Player *player;
	if (m_players.size() > 0)
		player = &m_players[0];
	else return;

	Object3D *playerShip = &m_players[0].object;

	switch(m_currState)
	{
		case START:
		{
			if (g_engine->GetNetwork()->GetLocalID() == g_engine->GetNetwork()->GetHostID())
			{
				if (g_engine->GetInput()->KeyDown(DIK_SPACE))
				{
					
					NetworkMessage msg;
					msg.dpnid = player->dpnid;
					msg.msgid = MSGID_START_COUNTDOWN;
					g_engine->GetNetwork()->Send(&msg, sizeof(NetworkMessage), 0);
				}

				if (g_engine->GetInput()->KeyDown(DIK_SPACE))
				{
					
					NetworkMessage msg;
					msg.dpnid = player->dpnid;
					msg.msgid = MSGID_START_COUNTDOWN;
					g_engine->GetNetwork()->Send(&msg, sizeof(NetworkMessage), 0);
				}
			}

			
			D3DXMATRIX view;
			m_camera.Yaw(0.15f * dt);
			m_camera.GetViewMatrix(&view);
			g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &view);
			
		}break;

		case INITGAME:
		{
			m_countDown = 4.0f;
			m_throttle = 0;
			m_currRing = 1;
			m_raceTimer = 0.0f;
			m_currState = COUNTDOWN;
			m_shipSpeed = 0.0f;
			m_players[0].object.SetPosition(g_track[g_currTrack].rings[0].GetPosition());
			m_players[0].object.SetRight(g_track[g_currTrack].rings[0].GetRight());
			m_players[0].object.SetUp(g_track[g_currTrack].rings[0].GetUp());
			m_players[0].object.SetLook(g_track[g_currTrack].rings[0].GetLook());
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
			}


			D3DXVECTOR3 shipPos = playerShip->GetPosition();
			m_camera.SetLook(playerShip->GetLook());
			m_camera.SetRight(playerShip->GetRight());
			m_camera.SetUp(playerShip->GetUp());

			D3DXVECTOR3 targetPos = (
			shipPos + playerShip->GetUp() * 100.0f - playerShip->GetLook() * 500.0f);
			
			D3DXMATRIX V;
			m_camera.UpdateThirdPerson(targetPos, &V, dt);
			g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &V);
		}break;

		case PLAYGAME:
		{
			m_raceTimer += dt;
			m_camera.Walk(m_shipSpeed * dt);

			

			playerShip->MoveOnZ(m_shipSpeed * dt);

			if (input->KeyDown(DIK_LEFT))
			{
				playerShip->Roll(ROLL_SPEED * dt);
			}
			if (input->KeyDown(DIK_RIGHT))
			{
				playerShip->Roll(-ROLL_SPEED * dt);
			}
			if (input->KeyDown(DIK_UP))
			{
				playerShip->Pitch(PITCH_SPEED * dt);
			}
			if (input->KeyDown(DIK_DOWN))
			{
				playerShip->Pitch(-PITCH_SPEED * dt);
			}
			if (input->KeyDown(DIK_A))
			{
				playerShip->Yaw(-YAW_SPEED * dt);
			}
			if (input->KeyDown(DIK_D))
			{
				playerShip->Yaw(YAW_SPEED * dt);
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
			D3DXVECTOR3 pos = playerShip->GetPosition();
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
			playerShip->SetPosition(pos);

			
			D3DXVECTOR3 shipPos = playerShip->GetPosition();
			m_camera.SetLook(playerShip->GetLook());
			m_camera.SetRight(playerShip->GetRight());
			m_camera.SetUp(playerShip->GetUp());


			if (abs(shipPos.x - g_track[g_currTrack].rings[m_currRing].GetPosition().x) < 450.0f && 
				abs(shipPos.y - g_track[g_currTrack].rings[m_currRing].GetPosition().y) < 450.0f && 
				abs(shipPos.z - g_track[g_currTrack].rings[m_currRing].GetPosition().z) < 450.0f)
			{
				g_sounds[0]->Play();
				m_currRing++;
				//m_shipSpeed += 100000.0f * dt;
				if (m_currRing >= g_track[g_currTrack].rings.size())
				{
					//m_currRing = 0;
					
					FinalTimeMsg finalTimeMsg;
					finalTimeMsg.dpnid = player->dpnid;
					finalTimeMsg.msgid = MSGID_FINAL_TIME;
					finalTimeMsg.time = m_raceTimer;
					g_engine->GetNetwork()->Send(&finalTimeMsg, sizeof(FinalTimeMsg), 0, DPNSEND_NOLOOPBACK);
					player->finished = true;
					player->time = m_raceTimer;
					m_currState = RESULTS;
				}
			}

			// send updated position for yourself
			static DWORD updateMoveTimer = timeGetTime();
			if (updateMoveTimer < timeGetTime() - 100)
			{
				MoveUpdateMsg msg;
				msg.dpnid = player->dpnid;
				msg.msgid = MSGID_MOVE_UPDATE;
				msg.position = player->object.GetPosition();
				msg.right    = player->object.GetRight();
				msg.up       = player->object.GetUp();
				msg.look     = player->object.GetLook();
				g_engine->GetNetwork()->Send(&msg, sizeof(MoveUpdateMsg), 0, DPNSEND_NOLOOPBACK);
			}
			//---------------------------------------------------------------------------

			D3DXVECTOR3 targetPos = (
			shipPos + playerShip->GetUp() * 100.0f - playerShip->GetLook() * 500.0f);
			
			D3DXMATRIX V;
			m_camera.UpdateThirdPerson(targetPos, &V, dt);
			g_engine->GetDevice()->SetTransform(D3DTS_VIEW, &V);
		}break;
	}
	
}

void Multiplayer::Draw()
{
	if (loaded == false)
		return;

	Player *player;
	if (m_players.size() > 0)
		player = &m_players[0];
	else return;

	D3DXMATRIX trans;
	D3DXMatrixIdentity(&trans);
	g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &trans);

	g_terrain[g_track[g_currTrack].terrainId].Draw();

	for (int i = 0; i < m_players.size(); i++)
	{
		g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &m_players[i].object.GetMatrix());
		g_shipMesh[m_players[i].ship].Render();
	}
	
	switch(m_currState)
	{
		case START:
		{
			
			int y;
			for (int i = 0; i < m_players.size(); i++)
			{
				y = i * 32;
				bool isLocal = false;
				if (m_players[i].dpnid == g_engine->GetNetwork()->GetLocalID())
					isLocal = true;
				if (m_players[i].dpnid == g_engine->GetNetwork()->GetHostID())
				{
					g_font.Draw(0, y, isLocal ? 0xffffff00 : 0xffffffff, "%s - %s", m_players[i].name, "hosting");
				}
				else g_font.Draw(0, y, isLocal ? 0xffffff00 : 0xffffffff, m_players[i].name);
		

				if (g_engine->GetNetwork()->IsHost())
				{
					g_font.Draw(180, 560, 0xffffffff, "Press space bar to start race");
				}
				else g_font.Draw(180, 560, 0xffffffff, "Waiting for host to start race");
				
			}
		}break;

		case COUNTDOWN:
		{
			g_font.Draw(384, 284, 0xffffffff, "%i", (int)m_countDown);
		}break;

		case PLAYGAME:
		{
			
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
			object.Reset();
			float x, y, z;
			D3DXVECTOR3 look, up, pos;
			m_camera.GetPosition(x, y, z);
			m_camera.GetLook(look);
			m_camera.GetUp(up);

			look = player->object.GetLook();
			up = player->object.GetUp();
			pos = player->object.GetPosition();
			
			object.SetPosition(
				D3DXVECTOR3(
				x + m_camera.GetRealLook().x * 400.0f + m_camera.GetRealUp().x*130.0f,
				y + m_camera.GetRealLook().y * 400.0f + m_camera.GetRealUp().y*130.0f, 
				z + m_camera.GetRealUp().z*130.0f+m_camera.GetRealLook().z * 400.0f));

			look = g_track[g_currTrack].rings[m_currRing].GetPosition() - player->object.GetPosition();
			D3DXVec3Normalize(&look, &look);
			object.SetLook(look);
			
			if (m_currState == PLAYGAME)
			{
				g_engine->GetDevice()->SetTransform(D3DTS_WORLD, &object.GetMatrix());
				g_arrowMesh.Render();
			}
			g_engine->GetDevice()->SetRenderState(D3DRS_LIGHTING, true);

			for (int i = 0; i < m_players.size(); i++)
			{
				int y = i * 16;
				if (m_players[i].dpnid == g_engine->GetNetwork()->GetHostID())
					g_smallFont.Draw(0, y, 0xffffffff, "%s - %s", m_players[i].name, "Hosting");
				else g_smallFont.Draw(0, y, 0xffffffff, m_players[i].name);
			}
			g_font.Draw(0, 566, 0xffffffff, "%i MPH", (int)(m_shipSpeed/10.0f));

		}break;

		case RESULTS:
		{
			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_players[i].finished)
				{
					int y = 32 * i;
					g_font.Draw(0, y, 0xffffffff, "%s - %.2f seconds", m_players[i].name, m_players[i].time);
				}
			}
		}break;
	
	}
}

void Multiplayer::NetworkMessageHandler(ReceivedMessage *msg)
{
	PlayerInfo *info = g_engine->GetNetwork()->GetPlayer(msg->dpnid);
	switch(msg->msgid)
	{
		case MSGID_CREATE_PLAYER:
		{
			PlayerData *data = (PlayerData*)info->data;

			Player player;
			player.dpnid = msg->dpnid;
			strcpy(player.name,info->name);
			player.ship = data->ship;
			player.finished = false;
			player.time = 0.0f;

			m_players.push_back(player);

			if (player.dpnid == g_engine->GetNetwork()->GetHostID())
			{
				g_engine->GetNetwork()->SetReceiveAllowed(true);	
				
				PlayerInfo *localInfo = g_engine->GetNetwork()->GetPlayer(g_engine->GetNetwork()->GetLocalID());
		
				g_currTrack = data->track;
				((PlayerData*)localInfo->data)->track = data->track;

				m_color = g_track[g_currTrack].color;
				SetFog(10.0f, 100000.0f, g_track[g_currTrack].color);

				NetworkMessage netMsg;
				netMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
				netMsg.msgid = MSGID_REQUEST_STATUS;
				g_engine->GetNetwork()->Send(&netMsg, sizeof(NetworkMessage), msg->dpnid);

				//g_currTrack = 0;
				
				//loaded = true;
			}

		}break;

		case MSGID_DESTROY_PLAYER:
		{
			for (int i = 0; i < m_players.size(); i++)
			{
				if (msg->dpnid == m_players[i].dpnid)
				{
					m_players[i] = m_players[m_players.size()-1];
					m_players.pop_back();
					//break;
				}
			}
		}break;

		case MSGID_HOST_MIGRATE:
		{
			((PlayerData*)info->data)->track = g_currTrack;
		}break;

		case MSGID_START_COUNTDOWN:
		{
			//MessageBox(0,0,0,0);
			m_currState = INITGAME; 
			m_locked = true;
		}break;

		case MSGID_MOVE_UPDATE:
		{
			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_players[i].dpnid == msg->dpnid)
				{
					MoveUpdateMsg *moveUpdateMsg = (MoveUpdateMsg*)msg;
					m_players[i].object.SetPosition(moveUpdateMsg->position);
					m_players[i].object.SetRight(moveUpdateMsg->right);
					m_players[i].object.SetUp(moveUpdateMsg->up);
					m_players[i].object.SetLook(moveUpdateMsg->look);
					break;
				}

			}
		}break;

		case MSGID_FINAL_TIME:
		{
			FinalTimeMsg *finalTimeMsg = (FinalTimeMsg*)msg;
			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_players[i].dpnid == finalTimeMsg->dpnid)
				{
					m_players[i].time = finalTimeMsg->time;
					m_players[i].finished = true;
					
					break;
				}
			}
		}break;

		case MSGID_REQUEST_STATUS:
		{
			if (g_engine->GetNetwork()->GetHostID() == g_engine->GetNetwork()->GetLocalID())
			{
				StatusMsg statusMsg;
				statusMsg.dpnid = g_engine->GetNetwork()->GetHostID();
				statusMsg.msgid = MSGID_STATUS;
				statusMsg.locked = m_locked;
				g_engine->GetNetwork()->Send(&statusMsg, sizeof(StatusMsg), msg->dpnid);

			}

		}break;

		case MSGID_STATUS:
		{
			StatusMsg *statusMsg = (StatusMsg*)msg;
			if (statusMsg->locked)
			{
				g_sounds[3]->Play();
				g_engine->ChangeState(MAINMENU_STATE);
			}
		}break;

		case MSGID_REQUEST_TRACK:
		{
			if (g_engine->GetNetwork()->GetLocalID() == g_engine->GetNetwork()->GetHostID())
			{
				UpdateTrackMsg updateTrackMsg;
				updateTrackMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
				updateTrackMsg.msgid = MSGID_UPDATE_TRACK;
				updateTrackMsg.track = g_currTrack;
				g_engine->GetNetwork()->Send(&updateTrackMsg, sizeof(UpdateTrackMsg), msg->dpnid);
			}
		}break;

		case MSGID_UPDATE_TRACK:
		{
			UpdateTrackMsg *updateTrackMsg = (UpdateTrackMsg*)msg;
			g_currTrack = updateTrackMsg->track;
			loaded = true;
		}break;

		//case MSGID_TRACK_CHANGE:
		//{
		//	TrackChangeMsg *trackChangeMsg = (TrackChangeMsg*)msg;
		//	g_currTrack = trackChangeMsg->track;
		//	//char buffer[30];
		//	if (msg->dpnid == g_engine->GetNetwork()->GetHostID())
		//		((PlayerData*)info->data)->track = g_currTrack;
		//	//sprintf(buffer, "%i", ((PlayerData*)info->data)->track);
		//	
		//	//MessageBox(0, buffer, 0, 0);
		//	//PlayerData *data = (PlayerData*)info->data;
		//	
		//	

		//	
		//}break;
	}
}