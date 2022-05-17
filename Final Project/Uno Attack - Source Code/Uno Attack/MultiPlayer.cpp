// © 2008 STYLESoft. All rights reserved.

// MULTIPLAYER.CPP
// PROGRAMMED BY COLLIN MUNSON


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
	m_dealcount = 0;
	m_currCard = 0;
	m_currTurn = 0;

	m_reverse = false;
	m_skip = false;
	m_hit2 = false;
	m_hostSkip = false;
	m_hostHit2 = false;

	m_deck.BuildDeck();
	m_deck.Shuffle();
	m_discardPile.Clear();

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
	m_cardParticle.Update(dt);

	switch(m_mpState)
	{
		case MP_LOBBY:
		{
			UpdateLobby(dt);
		}break;

		case MP_DEAL:
		{
			UpdateDeal(dt);
		}break;

		case MP_PLAY:
		{
			UpdatePlay(dt);
		}break;

		case MP_RESULTS:
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

	DrawLocalPlayersHand();

	RECT rect = {656, 177, 756, 256};
	
	m_sprite->Draw(g_cardTexture, &rect, &D3DXVECTOR3(50, 128, 0), &D3DXVECTOR3(512, 302, 0), 0xffffffff);
	m_cardParticle.Draw();

	rect.top = 0;
	rect.bottom = 177;
	m_sprite->Draw(g_cardTexture, &rect, &D3DXVECTOR3(50, 128, 0), &D3DXVECTOR3(512, 125, 0), 0xffffffff);

	switch(m_mpState)
	{
		case MP_LOBBY:
		{
			if (g_engine->GetNetwork()->IsHost())
				g_font->Draw(300,600, 0xffffffff, "Press space bar to start game");
			else
				g_font->Draw(300,600, 0xffffffff, "Press space bar when ready");
		}break;

		case MP_DEAL:
		{
			g_font->Draw(400, 300, 0xffffffff, "DEALING CARDS");
			//DrawStart();
		}break;

		case MP_PLAY:
		{
			m_sprite->Draw(g_cardTexture, &g_cardRects[m_topCard.id], &D3DXVECTOR3(41, 64,0), &D3DXVECTOR3(512, 75,0), 0xffffffff);
			//g_font->Draw(800, 0, 0xffffffff, "MP_PLAY");

			if (m_playableCard)
				g_font->Draw(400, 500, 0xffffffff, "playable card");

			if (m_wildCard)
			{
				g_font->Draw(400, 720, 0xffffffff, colorStr[m_currColorSelected]);
			}
			else if (m_tradehandsCard)
			{
				for (int i = 0; i < m_players.size(); i++)
				{
					if (m_currPlayerSelected == m_players[i].id)
						g_font->Draw(400, 720, 0xffffffff, "%i. %s", m_players[i].id+1, m_players[i].name);
				}
			}

			if (m_topCard.type >= WILD)
			{
				char buffer[80];
				switch(m_topCard.color)
				{
					case YELLOW:
						strcpy(buffer, "YELLOW");
						break;
					case RED:
						strcpy(buffer, "RED");
						break;
					case GREEN:
						strcpy(buffer, "GREEN");
						break;
					case BLUE:
						strcpy(buffer, "BLUE");
						break;
					default:
						strcpy(buffer, "NO COLOR");
						break;
				}
				g_font->Draw(560, 100, 0xffffffff,  buffer); 
			}

			RECT rect1 = {756, 0, 820, 128};
			RECT rect2 = {756, 128, 820, 256};

			if (m_players.size() > 2)
			{
				if (m_reverse)
					m_sprite->Draw(g_cardTexture, &rect1, 0, &D3DXVECTOR3(0, 300, 0), 0xffffffff);
				else 
					m_sprite->Draw(g_cardTexture, &rect2, 0, &D3DXVECTOR3(0, 300, 0), 0xffffffff);
			}

			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_players[i].dpnid == g_engine->GetNetwork()->GetLocalID())
				{
					if (m_currTurn == m_players[i].id)
					{
						if (!m_skip)
						{
							g_font->Draw(430, 300, 0xffffffff, "YOUR TURN");
						}
						if (m_hit2)
						{
							g_font->Draw(300, 350, 0xffffffff, "Press space bar to Hit Two");
						}
						else if (m_allHit)
						{
							g_font->Draw(300, 350, 0xffffffff, "Press space bar to All Hit");
						}
						else if (m_hitFire)
						{
							g_font->Draw(300, 350, 0xffffffff, "Press space bar to Hit Fire");
						}
					
					}
				}
			}
			//DrawPlay();
		}break;

		case MP_RESULTS:
		{
			m_sprite->Draw(g_cardTexture, &g_cardRects[m_topCard.id], &D3DXVECTOR3(41, 64,0), &D3DXVECTOR3(512, 75,0), 0xffffffff);
			g_font->Draw(300, 300, 0xffffffff, "%s has won the game", m_winnersName);
			//DrawEnd();
		}break;
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
				
				/*player.id = m_id;
				m_id++;*/
			}
			if (m_mpState == MP_LOBBY || m_mpState == MP_RESULTS) 
			{
				m_players.push_back(player);
				for (int i = 0; i < m_players.size(); i++)
					m_players[i].ready = false;
			}
		}break;

		case MSGID_DESTROY_PLAYER:
		{
			if (m_mpState != MP_LOBBY && m_mpState != MP_RESULTS)
			{
				if (g_engine->GetNetwork()->IsHost())
					g_engine->ChangeState(GS_MENU);
				
			}
			else
			{
			
				//EnterCriticalSection(&m_playerCS);
				for (int i = 0; i < m_players.size(); i++)
				{
					if (m_players[i].dpnid == msg->dpnid)
					{
						m_players[i] = m_players.back();
						m_players.pop_back();
					}
				}
			}
			//LeaveCriticalSection(&m_playerCS);
		}break;

		case MSGID_TERMINATE_SESSION:
		{
			if (m_mpState != MP_RESULTS)
				g_engine->ChangeState(GS_MENU);
		}break;


		case MSGID_ASSIGN_ID:
		{
			AssignIDMsg *assignIDMsg = (AssignIDMsg*)msg;

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
					if (m_players[i].dpnid == g_engine->GetNetwork()->GetLocalID() && addCardsMsg->n == 1)
						g_dingSound->Play();
					for (int j = 0; j < addCardsMsg->n; j++)
					{
						if (g_engine->GetNetwork()->GetLocalID() == addCardsMsg->addDpnid)
							m_cardParticle.AddCard(addCardsMsg->cards[j]);
						m_players[i].hand.AddCard(addCardsMsg->cards[j]);
					}
					break;
				}
			}
		}break;

		case MSGID_SET_TOPCARD:
		{
			SetTopCardMsg *setTopCardMsg = (SetTopCardMsg*)msg;
			m_topCard = setTopCardMsg->card;

			if (g_engine->GetNetwork()->IsHost())
				m_discardPile.AddCard(m_topCard);

			switch(m_topCard.type)
			{
				case SKIP:
					m_hostSkip = true;
					break;
				case REVERSE:
					if (m_players.size() == 2)
						m_hostSkip = true;
					else m_reverse = !m_reverse;
					break;
				case HIT_TWO:
					m_hostHit2 = true;
					break;

				case WILD_ALLHIT:
					m_allHit = true;
					break;
				case WILD_HITFIRE:
					m_hostHitFire = true;
					break;
			}

		}break;

		case MSGID_PLAYCARD:
		{
			PlayCardMsg *playCardMsg = (PlayCardMsg*)msg;
			m_topCard = playCardMsg->card;

			if (g_engine->GetNetwork()->IsHost())
				m_discardPile.AddCard(m_topCard);

			for (int i = 0; i < m_players.size(); i++)
			{
				
				if (m_players[i].dpnid == playCardMsg->dpnid)
				{
					Card card = m_players[i].hand.GetCard(playCardMsg->locOfCard);
					m_players[i].hand.RemoveCard(playCardMsg->locOfCard);

					

					switch(card.type)
					{
						case SKIP:
						{
							m_hostSkip = true;
						}break;

						case HIT_TWO:
						{
							m_hostHit2 = true;
						}break;

						case REVERSE:
						{
							if (m_players.size() == 2)
								m_hostSkip = true;
							else m_reverse = !m_reverse;
						}break;

						case DISCARD_ALL:
						{
							for (int j = 0; j < m_players[i].hand.GetNumCards(); j++)
							{
								if (m_players[i].hand.GetCard(j).color == card.color)
								{
									//m_discardPile.AddCard(m_playersHand.GetCard(i));
									m_players[i].hand.RemoveCard(j);
									j--;
								}
							}
							
						}break;

						case TRADE_HANDS:
						{
							int index1 = 0, index2 = 0;

							//FIND TWO PLAYERS HANDS TO TRADE
							for (int j = 0; j < m_players.size(); j++)
							{
								if (playCardMsg->player == m_players[j].id)
								{
									index1 = j;
								}
								else if (m_players[j].dpnid == playCardMsg->dpnid)
								{
									index2 = j;
								}

							}

							// TRADE HANDS
							CardHand temp = m_players[index1].hand;
							m_players[index1].hand = m_players[index2].hand;
							m_players[index2].hand = temp;

						}break;

						case WILD:
						{
							m_topCard.color = playCardMsg->color;	
						}break;

						case WILD_ALLHIT:
						{
							m_topCard.color = playCardMsg->color;
							if (m_players[i].dpnid != g_engine->GetNetwork()->GetLocalID())
								m_allHit = true;
						}break;

						case WILD_HITFIRE:
						{
							m_topCard.color = playCardMsg->color;
							m_hostHitFire = true;
						}break;
					}

					if (m_players[i].hand.GetNumCards() < 7 && 
						m_players[i].dpnid == g_engine->GetNetwork()->GetLocalID())
						m_currCard = m_players[i].hand.GetNumCards()/2;
					
				}
			}
		}break;

		case MSGID_HITLAUNCHER:
		{
			HitLauncherMsg *hitLauncherMsg = (HitLauncherMsg*)msg;
			if (g_engine->GetNetwork()->IsHost())
			{
				int numOfCards = 0;
				if (hitLauncherMsg->numOfHits > 2)
				{
					while (numOfCards == 0)
					{
						numOfCards = HitLauncher();
					}
				}
				else
				{
					for (int i = 0; i < hitLauncherMsg->numOfHits; i++)
					{
						numOfCards += HitLauncher();
					}
				}

				AddCardsMsg addCardsMsg;
				addCardsMsg.addDpnid = hitLauncherMsg->dpnid;
				addCardsMsg.msgid = MSGID_ADDCARDS;
				addCardsMsg.dpnid = g_engine->GetNetwork()->GetHostID();
				addCardsMsg.n = numOfCards;
				for (int i = 0; i < addCardsMsg.n; i++)
				{
					Card card = m_deck.GetCardFromDeck();
					addCardsMsg.cards[i] = card.id;
				}
				g_engine->GetNetwork()->Send(&addCardsMsg, sizeof(AddCardsMsg));
			}
		}break;

		case MSGID_DONE_WITH_TURN:
		{
			if (g_engine->GetNetwork()->IsHost())
			{
				if (m_reverse)
					m_currTurn--;
				else
					m_currTurn++;

				if (m_currTurn < 0) m_currTurn = m_players.size()-1;
				if (m_currTurn >= m_players.size()) m_currTurn = 0;

				ChangeTurnMsg changeTurnMsg;
				changeTurnMsg.msgid = MSGID_CHANGE_TURN;
				changeTurnMsg.dpnid = g_engine->GetNetwork()->GetHostID();
				changeTurnMsg.turn = m_currTurn;
				g_engine->GetNetwork()->Send(&changeTurnMsg, sizeof(ChangeTurnMsg));
			}
		}break;

		case MSGID_CHANGE_TURN:
		{
			ChangeTurnMsg *changeTurnMsg = (ChangeTurnMsg*)msg;
			m_currTurn = changeTurnMsg->turn;

			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_currTurn == m_players[i].id && 
					g_engine->GetNetwork()->GetLocalID() == m_players[i].dpnid)
				{
					if (m_hostSkip) m_skip = true;
					else if (m_hostHit2) m_hit2 = true;
					else if (m_hostHitFire) m_hitFire = true;
				}
			}
			if (m_hostSkip) m_hostSkip = false;
			if (m_hostHit2) m_hostHit2 = false;
			if (m_hostHitFire) m_hostHitFire = false;
		}break;

		case MSGID_CHANGE_STATE:
		{
			ChangeStateMsg *changeStateMsg = (ChangeStateMsg*)msg;
			m_mpState = changeStateMsg->state;
			if (m_mpState == MP_DEAL) m_currTurn = m_players.size() - 1;
		}break;

		case MSGID_KICK_PLAYER:
		{
			g_engine->ChangeState(GS_MENU);
		}break;

		case MSGID_READY:
		{
			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_players[i].dpnid == msg->dpnid)
				{
					m_players[i].ready = true;
				}
			}
		}break;
	}
}

void MultiPlayer::UpdateLobby(float dt)
{
	if (g_engine->GetNetwork()->IsHost())
	{
		static float updateTimer = 1.0f;
		updateTimer+=dt;
		if (updateTimer > 1.0f)
		{
			updateTimer = 0.0f;
			
			//ASSIGN ID TO PLAYERS
			for (int i = 0; i < m_players.size(); i++)
			{
				AssignIDMsg assignIDMsg;
				assignIDMsg.msgid = MSGID_ASSIGN_ID;
				assignIDMsg.dpnid = g_engine->GetNetwork()->GetHostID();
				assignIDMsg.assignDpnid = m_players[i].dpnid;
				assignIDMsg.id = i;//m_players[i].id;
				g_engine->GetNetwork()->Send(&assignIDMsg, sizeof(AssignIDMsg));
			}
		}


		if (m_input->KeyDown(DIK_SPACE) && m_players.size() >= 2)
		{
			ChangeStateMsg changeStateMsg;
			changeStateMsg.msgid = MSGID_CHANGE_STATE;
			changeStateMsg.state = MP_DEAL;
			g_engine->GetNetwork()->Send(&changeStateMsg, sizeof(ChangeStateMsg));
		}
	}
	else
	{
		if (m_input->KeyDown(DIK_SPACE))
		{
			NetworkMessage netMsg;
			netMsg.msgid = MSGID_READY;
			netMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
			g_engine->GetNetwork()->Send(&netMsg, sizeof(netMsg));
		}
	}
	
}

void MultiPlayer::UpdateDeal(float dt)
{
	m_currCard = m_players[0].hand.GetNumCards()/2;

	static float updateTimer = 1.0f;
	updateTimer += dt;
	// Add TO START
	if (updateTimer >= 1.0f && m_dealcount < 7 && g_engine->GetNetwork()->IsHost())
	{
		updateTimer = 0.0f;
		for (int id = 0; id < m_players.size(); id++)
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
		
		m_dealcount++;
		if (m_dealcount >= 7)
		{
			Card card;
			do
			{
				card = m_deck.GetCardFromDeck();
				m_discardPile.AddCard(card);
			}while(card.type == TRADE_HANDS);

				
			SetTopCardMsg setTopCardMsg;
			setTopCardMsg.msgid = MSGID_SET_TOPCARD;
			setTopCardMsg.dpnid = g_engine->GetNetwork()->GetHostID();
			setTopCardMsg.card = card;
			g_engine->GetNetwork()->Send(&setTopCardMsg, sizeof(SetTopCardMsg));
			if (card.type == REVERSE) m_currTurn = 0;

			ChangeStateMsg changeStateMsg;
			changeStateMsg.msgid = MSGID_CHANGE_STATE;
			changeStateMsg.state = MP_PLAY;
			g_engine->GetNetwork()->Send(&changeStateMsg, sizeof(ChangeStateMsg));

			NetworkMessage netMsg;
			netMsg.msgid = MSGID_DONE_WITH_TURN;
			netMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
			g_engine->GetNetwork()->Send(&netMsg, sizeof(NetworkMessage), g_engine->GetNetwork()->GetHostID());

		}
	}
}

void MultiPlayer::DrawPlayerList()
{
	switch(m_mpState)
	{
		// DRAW LIST FOR LOBBY
		case MP_LOBBY:
		{
			for (int i = 0; i < m_players.size(); i++)
			{
				g_font->Draw(0, m_players[i].id  * 40, 
					0xffffffff, 
					"%i. %s - %s", m_players[i].id+1, m_players[i].name, m_players[i].ready ? "Ready" : "Not Ready");
			
			}
			g_font->Draw(700, 0, 0xffffffff, "%d", m_players.size());
		}break;


		// DRAW LIST FOR DEAL
		case MP_DEAL:
		{
			for (int i = 0; i < m_players.size(); i++)
			{
				g_font->Draw(0, m_players[i].id  * 40, 
					0xffffffff, 
					"%i. %s - %d", m_players[i].id+1, m_players[i].name, m_players[i].hand.GetNumCards());
			
			}
		}break;

		// DRAW LIST FOR PLAY
		case MP_PLAY:
		{
			for (int i = 0; i < m_players.size(); i++)
			{
				if (m_players[i].hand.GetNumCards() == 1)
				{
					g_font->Draw(0, m_players[i].id  * 40, 
						(m_players[i].id == m_currTurn) ? 0xffffffff : 0x66ffffff, 
						"%i. %s - UNO", m_players[i].id+1, m_players[i].name);
				}
					
				else
				{
					g_font->Draw(0, m_players[i].id  * 40, 
						(m_players[i].id == m_currTurn) ? 0xffffffff : 0x66ffffff, 
						"%i. %s - %d", m_players[i].id+1, m_players[i].name, m_players[i].hand.GetNumCards());
				}
			
			}
			g_font->Draw(700, 0, 0xffffffff, "%d", m_players.size());
		}break;
	}
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

void MultiPlayer::UpdatePlay(float dt)
{
	
	for (int i = 0; i < m_players.size(); i++)
	{
		if (m_players[i].dpnid == g_engine->GetNetwork()->GetLocalID())
		{
			if (m_players[i].hand.GetNumCards() > 0)
			{
				if (m_players[i].hand.GetCard(m_currCard).color == m_topCard.color ||
					m_players[i].hand.GetCard(m_currCard).type == m_topCard.type || 
					m_players[i].hand.GetCard(m_currCard).type >= WILD ||
					m_topCard.color == NOCOLOR && 
					!(m_players[i].hand.GetCard(m_currCard).type == DISCARD_ALL &&
					m_players[i].hand.GetNumCards() == 1))
				{
					m_playableCard = true;
				}
				else 
				{
					m_playableCard = false;
				}

				if (m_players[i].hand.GetCard(m_currCard).type >= WILD)
				{
					m_wildCard = true;
				}
				else m_wildCard = false;

				if (m_players[i].hand.GetCard(m_currCard).type == TRADE_HANDS)
				{
					m_tradehandsCard = true;
				}
				else m_tradehandsCard = false;
			}

			if (m_input->KeyDown(DIK_LEFT) &&
				!m_input->PrevKeyDown(DIK_LEFT))
			{
				m_currCard--;
				if (m_currCard < 0) m_currCard = m_players[i].hand.GetNumCards()-1;
			}

			if (m_input->KeyDown(DIK_RIGHT) &&
				!m_input->PrevKeyDown(DIK_RIGHT))
			{
				m_currCard++;
				if (m_currCard >= m_players[i].hand.GetNumCards()) m_currCard = 0;
			}


			if (m_input->KeyDown(DIK_UP) &&
				!m_input->PrevKeyDown(DIK_UP))
			{
				if (m_wildCard)
				{
					m_currColorSelected--;
					if (m_currColorSelected < 0) m_currColorSelected = 3;
				}
				else if (m_tradehandsCard)
				{
					do 
					{
						m_currPlayerSelected--;
						if (m_currPlayerSelected < 0) m_currPlayerSelected = m_players.size()-1;
					}while(m_currPlayerSelected == m_players[i].id);
				}

			}

			if (m_input->KeyDown(DIK_DOWN) &&
				!m_input->PrevKeyDown(DIK_DOWN))
			{
				if (m_wildCard)
				{
					m_currColorSelected++;
					if (m_currColorSelected > 3) m_currColorSelected = 0;
				}
				else if (m_tradehandsCard)
				{
					do
					{
						m_currPlayerSelected++;
						if (m_currPlayerSelected >= m_players.size()) m_currPlayerSelected = 0;
					}while(m_currPlayerSelected == m_players[i].id);
				}
			}

			if (m_players[i].id == m_currTurn)
			{
				if (m_skip)
				{
					m_skip = false;

					NetworkMessage netMsg;
					netMsg.msgid = MSGID_DONE_WITH_TURN;
					netMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
					g_engine->GetNetwork()->Send(&netMsg, sizeof(NetworkMessage), g_engine->GetNetwork()->GetHostID());
				}
				else if (m_hit2)
				{
					if (m_input->KeyDown(DIK_SPACE))
					{
						m_hit2 = false;

					
						HitLauncherMsg hitLauncherMsg;
						hitLauncherMsg.numOfHits = 2;
						hitLauncherMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						hitLauncherMsg.msgid = MSGID_HITLAUNCHER;
						g_engine->GetNetwork()->Send(&hitLauncherMsg, sizeof(HitLauncherMsg));

						/*netMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						netMsg.msgid = MSGID_HITLAUNCHER;
						g_engine->GetNetwork()->Send(&netMsg, sizeof(NetworkMessage));*/

		
						NetworkMessage netMsg;
						netMsg.msgid = MSGID_DONE_WITH_TURN;
						netMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						g_engine->GetNetwork()->Send(&netMsg, sizeof(NetworkMessage), g_engine->GetNetwork()->GetHostID());

						g_dingSound->Play();
					}
				}
				else if (m_hitFire)
				{
					if (m_input->KeyDown(DIK_SPACE))
					{
						m_hitFire = false;

						HitLauncherMsg hitLauncherMsg;
						hitLauncherMsg.numOfHits = 3;
						hitLauncherMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						hitLauncherMsg.msgid = MSGID_HITLAUNCHER;
						g_engine->GetNetwork()->Send(&hitLauncherMsg, sizeof(HitLauncherMsg));

						NetworkMessage netMsg;
						netMsg.msgid = MSGID_DONE_WITH_TURN;
						netMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						g_engine->GetNetwork()->Send(&netMsg, sizeof(NetworkMessage), g_engine->GetNetwork()->GetHostID());

						g_dingSound->Play();
					}
				}
				else if (m_allHit)
				{
					if (m_input->KeyDown(DIK_SPACE))
					{
						m_allHit = false;

						HitLauncherMsg hitLauncherMsg;
						hitLauncherMsg.numOfHits = 1;
						hitLauncherMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						hitLauncherMsg.msgid = MSGID_HITLAUNCHER;
						g_engine->GetNetwork()->Send(&hitLauncherMsg, sizeof(HitLauncherMsg));

						NetworkMessage netMsg;
						netMsg.msgid = MSGID_DONE_WITH_TURN;
						netMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						g_engine->GetNetwork()->Send(&netMsg, sizeof(NetworkMessage), g_engine->GetNetwork()->GetHostID());

						g_dingSound->Play();
					}
				}
				else
				{
					if (m_input->KeyDown(DIK_RETURN) &&
						!m_input->PrevKeyDown(DIK_RETURN) && m_playableCard)
					{
						PlayCardMsg playCardMsg;
						playCardMsg.card = m_players[i].hand.GetCard(m_currCard);
						playCardMsg.locOfCard = m_currCard;
						playCardMsg.color = m_currColorSelected;
						playCardMsg.player = m_currPlayerSelected;
						playCardMsg.msgid = MSGID_PLAYCARD;
						playCardMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						g_engine->GetNetwork()->Send(&playCardMsg, sizeof(PlayCardMsg));

						NetworkMessage netMsg;
						netMsg.msgid = MSGID_DONE_WITH_TURN;
						netMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						g_engine->GetNetwork()->Send(&netMsg, sizeof(NetworkMessage), g_engine->GetNetwork()->GetHostID());
					}

					if (m_input->KeyDown(DIK_SPACE) &&
						!m_input->PrevKeyDown(DIK_SPACE))
					{
						HitLauncherMsg hitLauncherMsg;
						hitLauncherMsg.numOfHits = 1;
						hitLauncherMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						hitLauncherMsg.msgid = MSGID_HITLAUNCHER;
						g_engine->GetNetwork()->Send(&hitLauncherMsg, sizeof(HitLauncherMsg));

						NetworkMessage netMsg;
						netMsg.msgid = MSGID_DONE_WITH_TURN;
						netMsg.dpnid = g_engine->GetNetwork()->GetLocalID();
						g_engine->GetNetwork()->Send(&netMsg, sizeof(NetworkMessage), g_engine->GetNetwork()->GetHostID());

						g_dingSound->Play();
					}
				}
			}
		}
	}

	// REPLENISH DECK 
	if (m_deck.GetNumCards() < 20)
	{
		m_discardPile.Shuffle();
		m_deck.AddCardsToDeck(m_discardPile.GetCards());
		// KEEP THE TOP CARD ON THE DISCARD PILE
		Card card = m_discardPile.GetCards().back();
		m_discardPile.Clear();
		m_discardPile.AddCard(card);
	}

	for (int i = 0; i < m_players.size(); i++)
	{
		if (m_players[i].hand.GetNumCards() <= 0)
		{
			m_mpState = MP_RESULTS;
			strcpy(m_winnersName, m_players[i].name);
		}
	}
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