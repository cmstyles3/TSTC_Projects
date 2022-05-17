// © 2008 STYLESoft. All rights reserved.

// SINGLEPLAYER.CPP
// PROGRAMMED BY COLLIN MUNSON

#include "Main.h"


int numCards = 0;
SinglePlayer::SinglePlayer(DWORD id) : State(id)
{
}

void SinglePlayer::Load()
{
	InitGame();
}

void SinglePlayer::Close()
{
}

void SinglePlayer::OnLostDevice()
{
}

void SinglePlayer::OnResetDevice()
{
}

void SinglePlayer::Update(float dt)
{
	m_input = g_engine->GetInput();

	if (m_input->KeyDown(DIK_ESCAPE))
	{
		g_engine->ChangeState(GS_MENU);
	}
	

	switch(m_spState)
	{
		case SP_DEAL:
		{
			UpdateDeal(dt);
		}break;

		case SP_PLAY:
		{
			UpdatePlay(dt);			
		}break;

		case SP_RESULTS:
		{
			if (m_input->KeyDown(DIK_N))
			{
				InitGame();
			}
		}break;

	}
	m_cardParticles.Update(dt);


}

void SinglePlayer::Draw()
{
	m_sprite = g_engine->GetSprite();

	m_sprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_sprite->Draw(g_tableTexture, 0, 0, 0, 0xffffffff); 
	
	switch(m_spState)
	{
		case SP_DEAL:
		{
			
			//sprite->Draw(g_cardTexture, &g_cardRects[m_topCard.id], &D3DXVECTOR3(41, 64,0), &D3DXVECTOR3(512, 75,0), 0xffffffff);
			DrawPlayersHand();
			g_font->Draw(400, 300, 0xffffffff, "DEALING CARDS");
			//DisplayInfo();
		}break;

		case SP_PLAY:
		{
			
			m_sprite->Draw(g_cardTexture, &g_cardRects[m_topCard.id], &D3DXVECTOR3(41, 64,0), &D3DXVECTOR3(512, 75,0), 0xffffffff);
			DrawPlayersHand();
			
			DisplayInfo();
		}break;

		case SP_RESULTS:
		{
			if (m_whowon == PLAYER)
				g_font->Draw(300, 300, 0xffffffff, "You have won");
			else g_font->Draw(300, 300, 0xffffffff, "The Computer has won");

			g_font->Draw(300, 400, 0xffffffff, "Press 'N' to play new game");
		}break;
	}

	
	RECT rect = {656, 177, 756, 256};
	
	m_sprite->Draw(g_cardTexture, &rect, &D3DXVECTOR3(50, 128, 0), &D3DXVECTOR3(512, 302, 0), 0xffffffff);
	m_cardParticles.Draw();

	rect.top = 0;
	rect.bottom = 177;
	m_sprite->Draw(g_cardTexture, &rect, &D3DXVECTOR3(50, 128, 0), &D3DXVECTOR3(512, 125, 0), 0xffffffff);

	if (m_spState != SP_DEAL)
	{
		m_sprite->Draw(g_cardTexture, &g_cardRects[m_topCard.id], &D3DXVECTOR3(41, 64,0), &D3DXVECTOR3(512, 75,0), 0xffffffff);
	}

	m_sprite->End();
	m_sprite->Flush();
}

void SinglePlayer::UpdateDeal(float dt)
{
	m_timer += dt;
	if (m_timer >= 1.0f)
	{
		Card card = m_deck.GetCardFromDeck();
		m_playersHand.AddCard(card);
		m_cardParticles.AddCard(card.id);
		m_computersHand.AddCard(m_deck.GetCardFromDeck());
		m_timer = 0.0f;
		m_dealcount++;
		g_dingSound->Play();
	}

	if (m_dealcount == 7)
	{
		do 
		{
			m_topCard = m_deck.GetCardFromDeck();
			m_discardPile.AddCard(m_topCard);

		}while(m_topCard.type == TRADE_HANDS);

		if (m_topCard.type == HIT_TWO)
			m_hit2 = true;
		else if (m_topCard.type == SKIP || m_topCard.type == REVERSE)
			m_skip = true;
		else if (m_topCard.type == WILD_HITFIRE)
			m_hitFire = true;
		else if (m_topCard.type == WILD_ALLHIT)
			m_allHit = true;

	
		m_spState = SP_PLAY;
	}

	m_currCard = m_playersHand.GetNumCards()/2;
}

void SinglePlayer::UpdatePlay(float dt)
{

	// PLAYERS
	if (m_playersHand.GetNumCards() > 0)
	{
		if (m_topCard.type == m_playersHand.GetCard(m_currCard).type ||
			m_topCard.color == m_playersHand.GetCard(m_currCard).color || 
			m_playersHand.GetCard(m_currCard).type == WILD ||
			m_playersHand.GetCard(m_currCard).type == WILD_ALLHIT ||
			m_playersHand.GetCard(m_currCard).type == WILD_HITFIRE ||
			m_topCard.color == NOCOLOR)
			m_playableCard = true;
		else m_playableCard = false;

		if (m_playersHand.GetCard(m_currCard).type == TRADE_HANDS &&
			m_playersHand.GetNumCards() <= 1)
		{
			m_playableCard = false;
		}
	}

	if (m_input->KeyDown(DIK_LEFT) &&
		!m_input->PrevKeyDown(DIK_LEFT))
	{
		m_currCard--;
		if (m_currCard < 0) m_currCard = m_playersHand.GetNumCards() - 1;
	}
	if (m_input->KeyDown(DIK_RIGHT) &&
		!m_input->PrevKeyDown(DIK_RIGHT))
	{
		m_currCard++;
		if (m_currCard >= m_playersHand.GetNumCards()) m_currCard = 0;
	}

	if (m_input->KeyDown(DIK_UP) &&
		!m_input->PrevKeyDown(DIK_UP))
	{
		m_currColor--;
		if (m_currColor < 0) m_currColor = 3;
	}
	if (m_input->KeyDown(DIK_DOWN) &&
		!m_input->PrevKeyDown(DIK_DOWN))
	{
		m_currColor++;
		if (m_currColor > 3) m_currColor = 0;
	}

	// PLAYERS TURN
	if (m_currTurn == PLAYER)
	{
		if (m_skip)
		{
			m_skip = false;
			m_currTurn = COMPUTER;
		}
		else if (m_hit2)
		{
			if (m_input->KeyDown(DIK_SPACE))
			{
				m_hit2 = false;
				int one = HitLauncher();
				int two = HitLauncher();
				for (int i = 0; i < one+two; i++)
				{
					Card card = m_deck.GetCardFromDeck();
					m_cardParticles.AddCard(card.id);
					m_playersHand.AddCard(card);
				}
				m_currCard = m_playersHand.GetNumCards()/2;
				m_currTurn = COMPUTER;

				g_dingSound->Play();
			}
		}
		else if (m_hitFire)
		{
			if (m_input->KeyDown(DIK_SPACE))
			{
				m_hitFire = false;
				int n = 0;
				while(n == 0)
				{
					n = HitLauncher();
				}
				for (int i = 0; i < n; i++)
				{
					Card card = m_deck.GetCardFromDeck();
					m_cardParticles.AddCard(card.id);
					m_playersHand.AddCard(card);
				}
				m_currTurn = COMPUTER;

				g_dingSound->Play();
			}
		}
		else if (m_allHit)
		{
			if (m_input->KeyDown(DIK_SPACE))
			{
				m_allHit = false;
				int n = HitLauncher();
				for (int i = 0; i < n; i++)
				{
					Card card = m_deck.GetCardFromDeck();
					m_cardParticles.AddCard(card.id);
					m_playersHand.AddCard(card);
				}
				m_currTurn = COMPUTER;

				g_dingSound->Play();
			}
		}
		else
		{
			// PLAY CURRENT SELECTED CARD IF PLAYABLE WHEN PLAYER HITS ENTER
			if (m_input->KeyDown(DIK_RETURN) &&
			!m_input->PrevKeyDown(DIK_RETURN))
			{
				if (m_playersHand.GetNumCards() > 0 && m_playableCard)
				{
					m_topCard = m_playersHand.GetCard(m_currCard);
					m_playersHand.RemoveCard(m_currCard);
					if (m_topCard.type == DISCARD_ALL)
					{
						for (int i = 0; i < m_playersHand.GetNumCards(); i++)
						{
							if (m_playersHand.GetCard(i).color == m_topCard.color)
							{
								m_discardPile.AddCard(m_playersHand.GetCard(i));
								m_playersHand.RemoveCard(i);
								i--;
							}
						}
					}
					if (m_topCard.type == TRADE_HANDS)
					{
						CardHand temp = m_playersHand;
						m_playersHand = m_computersHand;
						m_computersHand = temp;
					}
					if (m_topCard.type == SKIP || m_topCard.type == REVERSE)
					{
						m_skip = true;
					}
					if (m_topCard.type == HIT_TWO)
					{
						m_hit2 = true;
					}


					m_discardPile.AddCard(m_topCard);
					if (m_playersHand.GetNumCards() < 7)
						m_currCard = m_playersHand.GetNumCards()/2;

					if (m_topCard.type == WILD ||
						m_topCard.type == WILD_ALLHIT ||
						m_topCard.type == WILD_HITFIRE)
					{
						if (m_topCard.type == WILD_ALLHIT) m_allHit = true;
						else if (m_topCard.type == WILD_HITFIRE) m_hitFire = true;
						m_topCard.color = m_currColor;
					}

					m_currTurn = COMPUTER;
					m_playableCard = false;
				}
				
			}
			
			// PRESS SPACE BAR TO HIT LAUNCHER
			if (m_input->KeyDown(DIK_SPACE) &&
				!m_input->PrevKeyDown(DIK_SPACE))
			{
				int num = numCards = HitLauncher();
				for (int i = 0; i < num; i++)
				{
					Card temp = m_deck.GetCardFromDeck();
					m_cardParticles.AddCard(temp.id);
					m_playersHand.AddCard(temp);
				}
				if (m_playersHand.GetNumCards() < 7)
					m_currCard = m_playersHand.GetNumCards()/2;

				m_currTurn = COMPUTER;
				m_playableCard = false;

				g_dingSound->Play();
			}
		}

	}
	// COMPUTERS TURN
	else
	{
		m_timer += dt;
		if (m_skip)
		{
			m_skip = false;
			m_currTurn = PLAYER;
		}
		else if (m_hit2)
		{
			m_hit2 = false;
			int one = HitLauncher();
			int two = HitLauncher();
			for (int i = 0; i < one+two; i++)
			{
				Card card = m_deck.GetCardFromDeck();
				//m_cardParticles.AddCard(card.id);
				m_computersHand.AddCard(card);
			}
			m_currTurn = PLAYER;

			g_dingSound->Play();
		}
		else if (m_hitFire)
		{
			
			m_hitFire = false;
			int n = 0;
			while(n == 0)
			{
				n = HitLauncher();
			}
			for (int i = 0; i < n; i++)
			{
				Card card = m_deck.GetCardFromDeck();
				//m_cardParticles.AddCard(card.id);
				m_computersHand.AddCard(card);
			}
			m_currTurn = PLAYER;

			g_dingSound->Play();
		}
		else if (m_allHit)
		{
			m_allHit = false;
			int n = HitLauncher();
			for (int i = 0; i < n; i++)
			{
				Card card = m_deck.GetCardFromDeck();
				//m_cardParticles.AddCard(card.id);
				m_computersHand.AddCard(card);
			}
			m_currTurn = PLAYER;

			g_dingSound->Play();
		}
		else if (m_timer > 2.0)
		{
			std::vector<AIDecision> aiDecisionList;
			int red = 0, green = 0, blue = 0, yellow = 0, wild = 0;
			m_computersHand.GetNumForEachColor(yellow, red, green, blue, wild);

			int color = NOCOLOR;
			if (red > green && red > yellow && red > blue)
			{
				color = RED;
			}
			else if (green > red && green > yellow && green > blue)
			{
				color = GREEN;
			}
			else if (blue > red && blue > yellow && blue > green)
			{
				color = BLUE;
			}
			else if (yellow > red && yellow > green && yellow > blue)
			{
				color = YELLOW;
			}

			std::vector<int> colorList;
			if (yellow) colorList.push_back(YELLOW);
			if (red) colorList.push_back(RED);
			if (green) colorList.push_back(GREEN);
			if (blue) colorList.push_back(BLUE);

			// SCAN CARDS AND MAKE DECISION LIST
			for (int i = 0; i < m_computersHand.GetNumCards(); i++)
			{
				if ((m_topCard.color == m_computersHand.GetCard(i).color ||
					m_topCard.type == m_computersHand.GetCard(i).type || 
					m_computersHand.GetCard(i).type >= WILD ||
					m_topCard.color == NOCOLOR) &&
					!(m_computersHand.GetCard(i).type == TRADE_HANDS &&
					m_computersHand.GetNumCards() == 1))
				{
					AIDecision aiDecision;
					aiDecision.index = i;
					switch(m_computersHand.GetCard(i).type)
					{
						case REVERSE:
						case SKIP:
						{
							aiDecision.value = rand()%10+20;
						}break;

						case HIT_TWO:
						{
							aiDecision.value = rand()%10+20;
						}break;
						case DISCARD_ALL:
						{
							aiDecision.value = rand()%10+20;
						}break;

						case TRADE_HANDS:
						{
							aiDecision.value = rand()%10+20;
						}break;
						
						case WILD:
						{
							aiDecision.value = rand()%20+1;
						}break;

						case WILD_ALLHIT:
						{
							aiDecision.value = rand()%5+25;
						}break;

						case WILD_HITFIRE:
						{
							aiDecision.value = rand()%5+28;
						}break;

						default:
						{
							aiDecision.value = rand()%10 + m_computersHand.GetCard(i).type;
						}break;


					}
					aiDecisionList.push_back(aiDecision);
				}
				
			}

			// IF THERE IS NO PLAY HIT LAUNCHER
			if (aiDecisionList.empty())
			{
				int num = numCards = HitLauncher();
				for (int i = 0; i < num; i++)
				{
					Card temp = m_deck.GetCardFromDeck();
					//m_cardParticles.AddCard(temp.id);
					m_computersHand.AddCard(temp);
				}
				g_dingSound->Play();
			}
			else
			{
				int index = 0;
				int value = -1;
				for (int i = 0; i < aiDecisionList.size(); i++)
				{
					if (aiDecisionList[i].value > value)
					{
						value = aiDecisionList[i].value;
						index = aiDecisionList[i].index;
					}
				}

				m_topCard = m_computersHand.GetCard(index);
				m_discardPile.AddCard(m_topCard);
				m_computersHand.RemoveCard(index);
				switch(m_topCard.type)
				{
					case REVERSE:
					case SKIP:
					{
						m_skip = true;
					}break;

					case HIT_TWO:
					{
						m_hit2 = true;
					}break;

					case DISCARD_ALL:
					{
						for (int i = 0; i < m_computersHand.GetNumCards(); i++)
						{
							if (m_computersHand.GetCard(i).color == m_topCard.color)
							{
								m_discardPile.AddCard(m_computersHand.GetCard(i));
								m_computersHand.RemoveCard(i);
								i--;
							}
						}
					}break;

					case TRADE_HANDS:
					{
						CardHand temp = m_playersHand;
						m_playersHand = m_computersHand;
						m_computersHand = temp;
					}break;

					case WILD:
					{
						if (color != NOCOLOR)
						{
							m_topCard.color = color;
						}
						else
						{
							m_topCard.color = colorList.empty() ? rand()%4 : colorList[rand()%colorList.size()];
						}
					}break;

					case WILD_ALLHIT:
					{
						m_allHit = true;
						if (color != NOCOLOR)
						{
							m_topCard.color = color;
						}
						else
						{
							m_topCard.color = colorList.empty() ? rand()%4 : colorList[rand()%colorList.size()];
						}
					}break;

					case WILD_HITFIRE:
					{
						m_hitFire = true;
						if (color != NOCOLOR)
						{
							m_topCard.color = color;
						}
						else
						{
							if (colorList.empty())
								m_topCard.color = rand()%4;
							else
							{
								m_topCard.color = colorList.empty() ? rand()%4 : colorList[rand()%colorList.size()];
							}
						}
					}break;		
				}
			}
		
			m_currTurn = PLAYER;
			m_timer = 0.0f;	
		}
		
	}

	if (m_playersHand.GetNumCards() <= 0)
	{
		m_whowon = PLAYER;
		m_spState = SP_RESULTS;
	}
	if (m_computersHand.GetNumCards() <= 0)
	{
		m_whowon = COMPUTER;
		m_spState = SP_RESULTS;
	}

	// DECK IS RUNNING LOW ON CARDS. MUST REPLENISH.
	if (m_deck.GetNumCards() < 20)
	{
		m_discardPile.Shuffle();
		m_deck.AddCardsToDeck(m_discardPile.GetCards());
		m_discardPile.Clear();
	}
}

void SinglePlayer::InitGame()
{
	m_currColor = 0;
	
	m_playersHand.Clear();
	m_computersHand.Clear();

	m_deck.Clear();
	m_deck.BuildDeck();
	m_deck.Shuffle();
	m_discardPile.Clear();
	m_cardParticles.Clear();
	
	m_currCard = m_playersHand.GetNumCards()/2;
	m_spState = SP_DEAL;
	m_currTurn = PLAYER;
	m_playableCard = false;

	m_timer = 0.0f;
	m_dealcount = 0;

	m_skip = false;
	m_hit2 = false;
	m_allHit = false;
	m_hitFire = false;

	InitLauncher();
}

void SinglePlayer::DrawPlayersHand()
{

	D3DXMATRIX T;
	LPD3DXSPRITE sprite = g_engine->GetSprite();

	for (int c = -3, curr = m_currCard-3; c < 4; c++, curr++)
	{
		if (curr >= 0 && curr < m_playersHand.GetNumCards() || m_playersHand.GetNumCards() >= 7)
		{
			D3DXMATRIX R;
			D3DXMatrixRotationZ(&R, angle[c+3] * PI/180.0f);
			D3DXMatrixTranslation(&T, c * 40.0f + 512.0f, ((c < 0) ? -c*18 : c*18) + 600.0f, 0.0f);
			sprite->SetTransform(&(R*T));
			sprite->Draw(g_cardTexture, &g_cardRects[m_playersHand.GetCard(curr).id], &D3DXVECTOR3(41, 64, 0), 0, m_currCard == curr ? 0xffffffff : 0x88888888);

			
			/*sprite->Draw(g_cardTexture, &g_cardRects[m_playersHand.GetCard(curr).id], 
			&D3DXVECTOR3(41, 64,0.0f), &D3DXVECTOR3(c * 50.0f + 512.0f, ((c < 0) ? -c*20 : c*20) + 600.0f, 0.0f), 
			m_currCard == curr ? 0xffffffff : 0x88888888);*/
		}
	}
	
	D3DXMatrixIdentity(&T);
	sprite->SetTransform(&T);
}

void SinglePlayer::DisplayInfo()
{
	int yellow = 0, red = 0, green = 0, blue = 0, wild = 0;
	m_playersHand.GetNumForEachColor(yellow, red, green, blue, wild);

	if (m_playableCard)
		g_font->Draw(400, 500, 0xffffffff, "playable Card");

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
	if (m_playersHand.GetNumCards() == 1)
		g_font->Draw(0, 0, (m_currTurn == PLAYER) ? 0xffffffff : 0x77ffffff, "Player - UNO");
	else g_font->Draw(0, 0, (m_currTurn == PLAYER) ? 0xffffffff : 0x77ffffff, "Player - %i cards", m_playersHand.GetNumCards());

	if (m_computersHand.GetNumCards() == 1)
		g_font->Draw(0, 30, (m_currTurn == COMPUTER) ? 0xffffffff : 0x77ffffff, "Computer - UNO");
	else g_font->Draw(0, 30, (m_currTurn == COMPUTER) ? 0xffffffff : 0x77ffffff, "Computer - %i cards", m_computersHand.GetNumCards());

	//g_font->Draw(570, 0, 0xffffffff, "Discard pile has %i card(s)", m_discardPile.GetNumCards());
	/*g_font->Draw(0, 610, 0xffffffff, "Yellow = %i\nRed = %i\nGreen = %i\nBlue = %i\nWild = %i", yellow, red, green, blue, wild);*/

	if (m_playersHand.GetCard(m_currCard).type >= WILD)
	{
		g_font->Draw(400, 720, 0xffffffff, colorStr[m_currColor]);
	}

	if (m_currTurn == PLAYER)
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

void SinglePlayer::InitLauncher()
{
	m_firstHit = true;
	m_secondHit = false;
	m_numZerosInRow = 0;
}

int SinglePlayer::HitLauncher()
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