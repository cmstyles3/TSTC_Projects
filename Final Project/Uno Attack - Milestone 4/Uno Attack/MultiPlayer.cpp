#include "Main.h"

MultiPlayer::MultiPlayer(DWORD id) : State(id)
{
}

void MultiPlayer::Load()
{
	InitializeCriticalSection(&m_playerCS);

	deal = false;
	m_mpState = MP_LOBBY;
	m_id = 0;
	dealNum = 0;
	m_currCard = 0;

	ResetLauncher();
}

void MultiPlayer::Close()
{
	g_engine->GetNetwork()->Close();
	m_players.clear();
	DeleteCriticalSection(&m_playerCS);
}

void MultiPlayer::OnLostDevice()
{
}

void MultiPlayer::OnResetDevice()
{
}

void MultiPlayer::Update(float dt)
{
	m_input = g_engine->GetInput();

	switch(m_mpState)
	{
		case MP_LOBBY:
		{
			UpdateLobby(dt);
		}break;

		case MP_START:
		{
			UpdateStart(dt);
		}break;

		case MP_PLAY:
		{
			//UpdatePlay(dt);
		}break;

		case MP_END:
		{
			//UpdateEnd(dt);
		}break;
	}

	
	if (m_input->KeyDown(DIK_ESCAPE) &&
		!m_input->PrevKeyDown(DIK_ESCAPE))
		g_engine->ChangeState(GS_MENU);
	if (m_input->KeyDown(DIK_SPACE))
		deal = true;
}

void MultiPlayer::Draw()
{
	m_sprite = g_engine->GetSprite();
	m_sprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_sprite->Draw(g_tableTexture, 0, 0, 0, 0xffffffff);

	RECT rect = {656, 177, 756, 256};
	
	m_sprite->Draw(g_cardTexture, &rect, &D3DXVECTOR3(50, 128, 0), &D3DXVECTOR3(512, 302, 0), 0xffffffff);
	//m_cardParticles.Draw();

	rect.top = 0;
	rect.bottom = 177;
	m_sprite->Draw(g_cardTexture, &rect, &D3DXVECTOR3(50, 128, 0), &D3DXVECTOR3(512, 125, 0), 0xffffffff);

	switch(m_mpState)
	{

		case MP_START:
		{
			g_font->Draw(800, 0, 0xffffffff, "MP_START");
			//DrawStart();
		}break;

		case MP_PLAY:
		{
			g_font->Draw(800, 0, 0xffffffff, "MP_PLAY");
			//DrawPlay();
		}break;

		case MP_END:
		{
			//DrawEnd();
		}break;
	}

	DrawLocalPlayersHand();
	for (int i = 0; i < m_players.size(); i++)
	{
		if (m_players[i].dpnid == g_engine->GetNetwork()->GetLocalID())
		{
			for (int j = 0; j < m_players[i].hand.GetNumCards(); j++)
			{
				Card card = m_players[i].hand.GetCard(j);
				m_sprite->Draw(g_cardTexture, &g_cardRects[card.id], 0, &D3DXVECTOR3(j * 60, 300, 0), 0xffffffff); 
			}
		}
		else
		{
			for (int j = 0; j < m_players[i].hand.GetNumCards(); j++)
			{
				Card card = m_players[i].hand.GetCard(j);
				m_sprite->Draw(g_cardTexture, &g_cardRects[card.id], 0, &D3DXVECTOR3(j * 60, 556, 0), 0xffffffff); 
			}

		}
	}

	DrawPlayerList();
	
	m_sprite->End();
}

void MultiPlayer::NetworkMessageHandler(ReceivedMessage *msg)
{
	PlayerInfo *info = g_engine->GetNetwork()->GetPlayer(msg->dpnid);
	switch(msg->msgid)
	{
		case MSGID_CREATE_PLAYER:
		{
			Player player;
			player.dpnid = msg->dpnid;
			player.id = 0;
			strcpy(player.name, info->name);

			if (player.dpnid == g_engine->GetNetwork()->GetHostID())
				g_engine->GetNetwork()->SetReceiveAllowed(true);

			if (g_engine->GetNetwork()->IsHost())
			{
				if (m_mpState != MP_LOBBY)
				{
					NetworkMessage networkMsg;
					networkMsg.dpnid = g_engine->GetNetwork()->GetHostID();
					networkMsg.msgid = MSGID_KICK_PLAYER;
					g_engine->GetNetwork()->Send(&networkMsg, sizeof(NetworkMessage), msg->dpnid);
				}
				m_deck.BuildDeck();
				m_deck.Shuffle();
				player.id = m_id;
				m_id++;
			}
			/*else
			{
				RequestIDMsg msg;
				msg.dpnid = g_engine->GetNetwork()->GetLocalID();
				msg.msgid = MSGID_REQUEST_ID;
				msg.rqstDpnid = player.dpnid;
				g_engine->GetNetwork()->Send(&msg, sizeof(NetworkMessage), g_engine->GetNetwork()->GetHostID());
			}*/

			if (m_mpState == MP_LOBBY) m_players.push_back(player);
		}break;

		case MSGID_DESTROY_PLAYER:
		{
			EnterCriticalSection(&m_playerCS);
			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_players[i].dpnid == msg->dpnid)
				{
					m_players[i] = m_players.back();
					m_players.pop_back();
				}
			}
			LeaveCriticalSection(&m_playerCS);
		}break;

		case MSGID_TERMINATE_SESSION:
		{
			g_engine->ChangeState(GS_MENU);
		}break;

		/*case MSGID_REQUEST_ID:
		{
			RequestIDMsg *requestIDMsg = ((RequestIDMsg*)(msg->data));
			if (g_engine->GetNetwork()->IsHost())
			{
				for (int i = 0; i < m_players.size(); i++)
				{
					if (m_players[i].dpnid == requestIDMsg->rqstDpnid)
					{
						SendIDMsg sendIDMsg;
						sendIDMsg.sendDpnid = requestIDMsg->rqstDpnid;
						sendIDMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						sendIDMsg.msgid = MSGID_SEND_ID;
						sendIDMsg.id = m_players[i].id;

						g_engine->GetNetwork()->Send(&sendIDMsg, sizeof(SendIDMsg), requestIDMsg->dpnid);


						break;
					}
				}
			}
		}break;*/

		case MSGID_ASSIGN_ID:
		{
			AssignIDMsg *assignIDMsg = (AssignIDMsg*)msg;

			/*char testbuffer[100];
			sprintf(testbuffer, "%d, %d", sendIDMsg->sendDpnid, sendIDMsg->id);*/
			//MessageBox(0,testbuffer,0,0);

			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_players[i].dpnid == assignIDMsg->assignDpnid)
				{
					m_players[i].id = assignIDMsg->id;
					break;
				}
			}
		}break;

		case MSGID_ADDCARDS:
		{
			AddCardsMsg *addCardsMsg = (AddCardsMsg*)msg;

			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_players[i].dpnid == addCardsMsg->addDpnid)
				{
					m_players[i].hand.AddCard(addCardsMsg->cards[0]);
					break;
				}
			}
		}break;

		case MSGID_CHANGE_STATE:
		{
			ChangeStateMsg *changeStateMsg = (ChangeStateMsg*)msg;
			m_mpState = changeStateMsg->state;
		}break;

		case MSGID_KICK_PLAYER:
		{
			g_engine->ChangeState(GS_MENU);
		}break;
	}
}

void MultiPlayer::UpdateLobby(float dt)
{
	static float updateTimer = 1.0f;
	updateTimer+=dt;
	if (updateTimer > 1.0f)
	{
		updateTimer = 0.0f;
		if (g_engine->GetNetwork()->IsHost())
		{
			//ASSIGN ID TO PLAYERS
			for (int i = 0; i < m_players.size(); i++)
			{
				AssignIDMsg assignIDMsg;
				assignIDMsg.msgid = MSGID_ASSIGN_ID;
				assignIDMsg.dpnid = g_engine->GetNetwork()->GetHostID();
				assignIDMsg.assignDpnid = m_players[i].dpnid;
				assignIDMsg.id = i;//m_players[i].id;
				g_engine->GetNetwork()->Send(&assignIDMsg, sizeof(AssignIDMsg), 0);
			}
		}
	}


	if (g_engine->GetNetwork()->IsHost() &&
		m_input->KeyDown(DIK_SPACE) && m_players.size() >= 2)
	{
		ChangeStateMsg changeStateMsg;
		changeStateMsg.msgid = MSGID_CHANGE_STATE;
		changeStateMsg.state = MP_START;
		g_engine->GetNetwork()->Send(&changeStateMsg, sizeof(ChangeStateMsg));
	}
	
}

void MultiPlayer::UpdateStart(float dt)
{
	m_currCard = m_players[0].hand.GetNumCards()/2;

	static float updateTimer = 1.0f;
	updateTimer += dt;
	// Add TO START
	if (updateTimer > 2.0f && dealNum < 7 && g_engine->GetNetwork()->IsHost())
	{
		updateTimer = 0.0f;
		for (int id = 0; id < m_id; id++)
		{
			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_players[i].id == id)
				{
					Card card = m_deck.GetCardFromDeck();
					AddCardsMsg addCardsMsg;
					addCardsMsg.msgid = MSGID_ADDCARDS;
					addCardsMsg.dpnid = g_engine->GetNetwork()->GetHostID();
					addCardsMsg.addDpnid = m_players[i].dpnid;
					addCardsMsg.n = 1;
					addCardsMsg.cards[0] = card.id;
					g_engine->GetNetwork()->Send(&addCardsMsg, sizeof(AddCardsMsg));
					break;
				}
			}
		}
		
		dealNum++;
		if (dealNum >= 7)
		{
			ChangeStateMsg changeStateMsg;
			changeStateMsg.msgid = MSGID_CHANGE_STATE;
			changeStateMsg.state = MP_PLAY;
			g_engine->GetNetwork()->Send(&changeStateMsg, sizeof(ChangeStateMsg));
		}
	}
}

void MultiPlayer::DrawPlayerList()
{
	for (int i = 0; i < m_players.size(); i++)
	{
		g_font->Draw(0, m_players[i].id  * 40, 0xffffffff, "%s - %d", m_players[i].name, m_players[i].hand.GetNumCards());
	}
	g_font->Draw(700, 0, 0xffffffff, "%d", m_players.size());
}

void MultiPlayer::DrawLocalPlayersHand()
{
	D3DXMATRIX T;
	for (int i = 0; i < m_players.size(); i++)
	{
		if (m_players[i].dpnid == g_engine->GetNetwork()->GetLocalID())
		{
			for (int c = -3, curr = m_currCard-3; c < 4; c++, curr++)
			{
				if (curr >= 0 && curr < m_players[i].hand.GetNumCards() || m_players[i].hand.GetNumCards() >= 7)
				{
					D3DXMATRIX R;
					D3DXMatrixRotationZ(&R, angle[c+3] * PI/180.0f);
					D3DXMatrixTranslation(&T, c * 40.0f + 512.0f, ((c < 0) ? -c*18 : c*18) + 600.0f, 0.0f);
					m_sprite->SetTransform(&(R*T));
					m_sprite->Draw(g_cardTexture, &g_cardRects[m_players[i].hand.GetCard(curr).id], &D3DXVECTOR3(41, 64, 0), 0, m_currCard == curr ? 0xffffffff : 0x88888888);
				}
			}
			break;
		}
	}

	D3DXMatrixIdentity(&T);
	m_sprite->SetTransform(&T);
}


int MultiPlayer::HitLauncher()
{
	int numCards = 0;
	if (m_firstHit)
	{
		m_firstHit = false;
		m_secondHit = true;
	}
	else if (m_secondHit)
	{
		m_secondHit = false;
		numCards = 2;
	}
	else
	{
		switch(m_numZerosInRow)
		{
			case 0:
			{
				int num = rand()%100;
				if (num < 10) // 10% chance
					numCards = rand()%6 + 2; // 2 - 7 cards
			}break;

			case 1:
			{
				int num = rand()%100;
				if (num < 25) // 20% chance
					numCards = rand()%7 + 2; // 2 - 8 cards
			}break;

			case 2:
			{
				int num = rand()%100;
				if (num < 50) // 50% chance
					numCards = rand()%8 + 2; // 2 - 9 cards
			}break;

			case 3:
			{
				int num = rand()%100;
				if (num < 95) // 95% chance
					numCards = rand()%6 + 4; // 4 - 9 cards
			}break;

			case 4:
			{
				int num = rand()%100;
				if (num < 95) // 95% chance
					numCards = rand()%7 + 4; // 4 - 10 cards
				// 5% chance
				else numCards = rand()%3 + 2; // 2 - 4 cards
			}break;
		}
	}
	
	if (numCards == 0)
		m_numZerosInRow++;
	else m_numZerosInRow = 0;

	return numCards;
}

void MultiPlayer::ResetLauncher()
{
	m_numZerosInRow = 0;
	m_firstHit = true;
	m_secondHit = false;
}