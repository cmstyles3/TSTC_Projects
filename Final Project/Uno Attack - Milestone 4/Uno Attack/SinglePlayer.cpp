#include "Main.h"



int numCards = 0;
SinglePlayer::SinglePlayer(DWORD id) : State(id)
{
}

void SinglePlayer::Load()
{
	m_deck.BuildDeck();
	m_playersHand.Clear();
	m_computersHand.Clear();
	m_deck.Shuffle();
	m_discardPile.Clear();
	m_cardParticles.Clear();
	/*for (int i = 0; i < 7; i++)
	{
		m_playersHand.AddCard(m_deck.GetCardFromDeck());
		m_computersHand.AddCard(m_deck.GetCardFromDeck());

	}*/
	
	m_currCard = m_playersHand.GetNumCards()/2;
	m_spState = SP_START;
	m_currTurn = PLAYER;
	m_playableCard = false;

	m_timer = 0.0f;
	m_dealcount = 0;

	m_skip = false;
	m_hit2 = false;

	InitLauncher();
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
	if (g_engine->GetInput()->KeyDown(DIK_ESCAPE))
	{
		g_engine->ChangeState(GS_MENU);
	}
	

	switch(m_spState)
	{
		case SP_START:
		{
			m_timer += dt;
			if (m_timer >= 0.5f)
			{
				m_playersHand.AddCard(m_deck.GetCardFromDeck());
				m_computersHand.AddCard(m_deck.GetCardFromDeck());
				m_timer = 0.0f;
				m_dealcount++;
			}

			if (m_dealcount == 7)
			{
				m_topCard = m_deck.GetCardFromDeck();
				while (m_topCard.id == TRADE_HANDS)
				{
					m_deck.AddCard(m_topCard);
					m_deck.Shuffle();
					m_topCard = m_deck.GetCardFromDeck();
				}
				if (m_topCard.type == HIT_TWO)
					m_hit2 = true;
				else if (m_topCard.type == SKIP)
					m_skip = true;
				m_discardPile.AddCard(m_topCard);
			
				m_spState = SP_PLAY;
			}

			m_currCard = m_playersHand.GetNumCards()/2;


		}break;

		case SP_PLAY:
		{

			if (g_engine->GetInput()->KeyDown(DIK_LEFT) &&
				!g_engine->GetInput()->PrevKeyDown(DIK_LEFT))
			{
				m_currCard--;
				if (m_currCard < 0) m_currCard = m_playersHand.GetNumCards() - 1;
			}
			if (g_engine->GetInput()->KeyDown(DIK_RIGHT) &&
				!g_engine->GetInput()->PrevKeyDown(DIK_RIGHT))
			{
				m_currCard++;
				if (m_currCard >= m_playersHand.GetNumCards()) m_currCard = 0;
			}

			if (g_engine->GetInput()->KeyDown(DIK_UP) &&
				!g_engine->GetInput()->PrevKeyDown(DIK_UP))
			{
				m_currColor--;
				if (m_currColor < 0) m_currColor = 3;
			}
			if (g_engine->GetInput()->KeyDown(DIK_DOWN) &&
				!g_engine->GetInput()->PrevKeyDown(DIK_DOWN))
			{
				m_currColor++;
				if (m_currColor > 3) m_currColor = 0;
			}

			if (m_currTurn == PLAYER)
			{
				if (m_skip)
				{
					m_skip = false;
					m_currTurn = COMPUTER;
				}
				else if (m_hit2)
				{
					m_hit2 = false;
					Card temp = m_deck.GetCardFromDeck();
					m_cardParticles.AddCard(temp.id);
					m_playersHand.AddCard(temp);
					m_currCard = m_playersHand.GetNumCards()/2;
					m_currTurn = COMPUTER;
				}
				else
				{

					if (m_playersHand.GetNumCards() > 0)
					{
						if (m_topCard.type == m_playersHand.GetCard(m_currCard).type ||
							m_topCard.color == m_playersHand.GetCard(m_currCard).color || 
							m_playersHand.GetCard(m_currCard).type == WILD)
							m_playableCard = true;
						else m_playableCard = false;
					}
					if (g_engine->GetInput()->KeyDown(DIK_SPACE) &&
					!g_engine->GetInput()->PrevKeyDown(DIK_SPACE))
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

							if (m_topCard.type == WILD)
							{
								m_topCard.color = m_currColor;
							}

							m_currTurn = COMPUTER;
							m_playableCard = false;
						}
						
					}
					if (g_engine->GetInput()->KeyDown(DIK_RETURN) &&
						!g_engine->GetInput()->PrevKeyDown(DIK_RETURN))
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
					}
				}

			}
			
			else
			{
				m_timer += dt;
				if (m_timer > 2.0)
				{
					if (m_skip)
					{
						m_skip = false;
						m_currTurn = PLAYER;
					}
					else if (m_hit2)
					{
						m_hit2 = false;
						m_computersHand.AddCard(m_deck.GetCardFromDeck());
						m_currTurn = PLAYER;
					}
					else
					{
						bool played = false;
						for (int i = 0; i < m_computersHand.GetNumCards(); i++)
						{
							if (m_topCard.color == m_computersHand.GetCard(i).color ||
								m_topCard.type == m_computersHand.GetCard(i).type)
							{
								m_topCard = m_computersHand.GetCard(i);
								m_computersHand.RemoveCard(i);

								if (m_topCard.type == DISCARD_ALL)
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
								played = true;


								break;
								
							}
							
						}
						

						if (!played)
						{
							for (int i = 0; i < m_computersHand.GetNumCards(); i++)
							{
								if (m_computersHand.GetCard(i).type == WILD)
								{
									m_topCard = m_computersHand.GetCard(i);
									m_discardPile.AddCard(m_topCard);
									m_computersHand.RemoveCard(i);
									m_topCard.color = rand()%4;
									played = true;
									break;
								}
							}
							if (!played)
							{
								int num = numCards = HitLauncher();
								for (int i = 0; i < num; i++)
								{
									Card temp = m_deck.GetCardFromDeck();
									m_cardParticles.AddCard(temp.id);
									m_computersHand.AddCard(temp);
								}
							}
							//	m_computersHand.AddCard(m_deck.GetCardFromDeck());
						}
					}

				
					m_currTurn = PLAYER;
					m_timer = 0.0f;
				}
			}

			if (m_playersHand.GetNumCards() <= 0)
			{
				m_whowon = PLAYER;
				m_spState = SP_END;
			}
			if (m_computersHand.GetNumCards() <= 0)
			{
				m_whowon = COMPUTER;
				m_spState = SP_END;
			}
			

		}break;

		case SP_END:
		{

		}break;

	}
	m_cardParticles.Update(dt);


}

void SinglePlayer::InitGame()
{
	m_deck.BuildDeck();
	
	m_deck.Shuffle();
}

void SinglePlayer::Draw()
{
	LPD3DXSPRITE sprite = g_engine->GetSprite();

	sprite->Begin(D3DXSPRITE_ALPHABLEND);

	sprite->Draw(g_tableTexture, 0, 0, 0, 0xffffffff); 
	
	RECT rect = {656, 177, 756, 256};
	
	sprite->Draw(g_cardTexture, &rect, &D3DXVECTOR3(50, 128, 0), &D3DXVECTOR3(512, 302, 0), 0xffffffff);
	m_cardParticles.Draw();

	rect.top = 0;
	rect.bottom = 177;
	sprite->Draw(g_cardTexture, &rect, &D3DXVECTOR3(50, 128, 0), &D3DXVECTOR3(512, 125, 0), 0xffffffff);
		
	
	
	g_font->Draw(700, 600, 0xffffffff, "numCards = %i", numCards);
	switch(m_spState)
	{
		case SP_START:
		{
			
			//sprite->Draw(g_cardTexture, &g_cardRects[m_topCard.id], &D3DXVECTOR3(41, 64,0), &D3DXVECTOR3(512, 75,0), 0xffffffff);
			DrawPlayersHand();
			g_font->Draw(400, 300, 0xffffffff, "DEALING CARDS");
			//DisplayInfo();
		}break;

		case SP_PLAY:
		{
			
			sprite->Draw(g_cardTexture, &g_cardRects[m_topCard.id], &D3DXVECTOR3(41, 64,0), &D3DXVECTOR3(512, 75,0), 0xffffffff);
			DrawPlayersHand();
			
			DisplayInfo();
		}break;

		case SP_END:
		{
			if (m_whowon == PLAYER)
				g_font->Draw(0, 0, 0xffffffff, "You have won");
			else g_font->Draw(0, 0, 0xffffffff, "The Computer has won");
		}break;
	}

	
	
	

	sprite->End();
	sprite->Flush();
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
	

	D3DXMATRIX S;
	D3DXMatrixScaling(&S, 0.5, 0.5, 1.0);
	for (int i = 0; i < m_computersHand.GetNumCards(); i++)
	{
		D3DXMatrixTranslation(&T, 0 + i*25, 325, 0);
		sprite->SetTransform(&(S*T));
		sprite->Draw(g_cardTexture, &g_cardRects[m_computersHand.GetCard(i).id], 0, 0, 0XFFFFFFFF);
	}

	//for (int i = m_discardPile.GetNumCards() - 1,c = 0; i >= 0 || c > 8; i--, c++)
	//{
	//	//if (i < 0)
	//	{
	//	D3DXMatrixTranslation(&T, 580 + c*51, 50, 0);
	//	sprite->SetTransform(&(S*T));
	//	sprite->Draw(g_cardTexture, &g_cardRects[m_discardPile.GetCard(i).id], 0, 0, 0XFFFFFFFF);
	//	}
	//}
	

	D3DXMatrixIdentity(&T);
	sprite->SetTransform(&T);
}

void SinglePlayer::DisplayInfo()
{
	int yellow = 0, red = 0, green = 0, blue = 0, wild = 0;
	m_playersHand.GetNumForEachColor(yellow, red, green, blue, wild);

	if (m_playableCard)
		g_font->Draw(400, 500, 0xffffffff, "playable Card");
	g_font->Draw(0, 0, (m_currTurn == PLAYER) ? 0xffffffff : 0x77ffffff, "Player - %i card(s)", m_playersHand.GetNumCards());
	g_font->Draw(0, 30, (m_currTurn == COMPUTER) ? 0xffffffff : 0x77ffffff, "Computer - %i card(s)", m_computersHand.GetNumCards());
	g_font->Draw(570, 0, 0xffffffff, "Discard pile has %i card(s)", m_discardPile.GetNumCards());
	g_font->Draw(0, 610, 0xffffffff, "Yellow = %i\nRed = %i\nGreen = %i\nBlue = %i\nWild = %i", yellow, red, green, blue, wild);

	if (m_playersHand.GetCard(m_currCard).type == WILD)
	{
		switch(m_currColor)
		{
			case YELLOW: g_font->Draw(930, 730, D3DCOLOR_XRGB(255,255,0), "yellow"); break;
			case RED: g_font->Draw(930, 730, D3DCOLOR_XRGB(255,0,0), "red"); break;
			case GREEN: g_font->Draw(930, 730, D3DCOLOR_XRGB(0,255,0), "green"); break;
			case BLUE: g_font->Draw(930, 730, D3DCOLOR_XRGB(0,0,255), "blue"); break;
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