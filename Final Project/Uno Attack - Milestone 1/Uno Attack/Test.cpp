#include "Main.h"

Test::Test(DWORD id) : State(id)
{
	m_color = D3DCOLOR_XRGB(255, 0, 0);
}

void Test::Load()
{
	m_color = D3DCOLOR_XRGB(30, 30, 30);
		
	D3DXCreateTextureFromFileEx(g_engine->GetDevice(), "Assets/Textures/cards.dds", 
	D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
	D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_tex);


	m_cardPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	CalcCardRects();
	AddCards();
	ShuffleCards();
	m_currCard = 0;
	m_currTurn = 0;
	m_skip = false;
	m_hit2 = false;
	DealCards();
	
	
	D3DXCreateLine(g_engine->GetDevice(), &m_lines);
	m_lines->SetWidth(4);
	m_lineList[0] = D3DXVECTOR2(471.0f-10, 250.0f-10);
	m_lineList[1] = D3DXVECTOR2(471.0f + 82+10, 250.0f-10);
	m_lineList[2] = D3DXVECTOR2(471.0f + 82+10, 250.0f + 128+10);
	m_lineList[3] = D3DXVECTOR2(471.0f-10, 250.0f + 128+10);
	m_lineList[4] = D3DXVECTOR2(471.0f-10, 250.0f-10);

	m_colors[0] = D3DCOLOR_XRGB(255, 255, 0);
	m_colors[1] = D3DCOLOR_XRGB(255,   0, 0);
	m_colors[2] = D3DCOLOR_XRGB(  0, 255, 0);
	m_colors[3] = D3DCOLOR_XRGB(  0,   0,255);
	m_colors[4] = D3DCOLOR_XRGB(255, 255, 255);

	

}

void Test::Close()
{
	SAFE_RELEASE(m_lines);
	SAFE_RELEASE(m_tex);
}

void Test::OnLostDevice()
{
}

void Test::OnResetDevice()
{
}

void Test::Update(float dt)
{

	if (m_playerHand.size() <= 0) 
	{
		MessageBox(0, "you wins", "Victory", 0);
		PostQuitMessage(0);
	}
	if (m_aiHand.size() <= 0) 
	{
		MessageBox(0, "computer wins", "Defeat", 0);
		PostQuitMessage(0);
	}


	Input *input = g_engine->GetInput();
	if (input->KeyDown(DIK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	if (input->KeyDown(DIK_LEFT) &&
		!input->PrevKeyDown(DIK_LEFT))
	{
		m_currCard--;
	}

	if (input->KeyDown(DIK_RIGHT) &&
		!input->PrevKeyDown(DIK_RIGHT))
	{
		m_currCard++;
	}

	if (input->KeyDown(DIK_UP) &&
		!input->PrevKeyDown(DIK_UP))
	{
		m_currColor--;
	}
	if (input->KeyDown(DIK_DOWN) &&
		!input->PrevKeyDown(DIK_DOWN))
	{
		m_currColor++;
	}

	if (m_currCard < 0)
		m_currCard = m_playerHand.size()-1;
	if (m_currCard > m_playerHand.size()-1)
			m_currCard = 0;

	if (m_currColor < 0)
		m_currColor = 3;
	else if (m_currColor > 3)
		m_currColor = 0;

	if (m_currTurn == 0)
	{
		if (m_skip)
		{
			m_skip = false;
			m_currTurn = 1;
		}
		else if (m_hit2)
		{
			m_hit2 = false;
			GetRandomCards(m_playerHand);
			GetRandomCards(m_playerHand);
			m_currTurn = 1;
		}
		else
		{
			if (input->KeyDown(DIK_RETURN) && 
				!input->PrevKeyDown(DIK_RETURN) && !m_playerHand.empty())
			{
				if ((m_playerHand[m_currCard].color == m_topCard.color ||
					m_playerHand[m_currCard].type == m_topCard.type || m_topCard.color == 4) &&
					m_playerHand[m_currCard].type != WILD)
				{
					m_topCard = m_playerHand[m_currCard];

					m_discardPile.push_back(m_topCard);

					m_playerHand[m_currCard] = m_playerHand.back();
					m_playerHand.pop_back();
					m_currTurn = 1;
					if (m_topCard.type == DISCARD_ALL)
					{
						for (int i = 0; i < m_playerHand.size(); i++)
						{
							if (m_playerHand[i].color == m_topCard.color)
							{
								m_playerHand[i] = m_playerHand.back();
								m_playerHand.pop_back();
								i--;
							}
						}
					}
					if (m_topCard.type == TRADE_HANDS)
					{
						std::vector<Card> temp;
						temp = m_playerHand;
						m_playerHand = m_aiHand;
						m_aiHand = temp;
					}
					if (m_topCard.type == SKIP || m_topCard.type == REVERSE)
					{
						m_skip = true;
					}
					if (m_topCard.type == HIT_TWO)
					{
						m_hit2 = true;
					}
				}
				else if (m_playerHand[m_currCard].type == WILD)
				{
					m_currTurn = 1;
					m_playerHand[m_currCard].color = m_currColor;
					m_topCard = m_playerHand[m_currCard];
					m_discardPile.push_back(m_topCard);
					m_playerHand[m_currCard] = m_playerHand.back();
					m_playerHand.pop_back();

				}
			}
		}
		
		
		
		
		if (input->KeyDown(DIK_SPACE) && !input->PrevKeyDown(DIK_SPACE))
		{
			GetRandomCards(m_playerHand);
			m_currTurn = 1;
			
		}
	}
	else
	{
		if (m_skip) 
		{
			m_skip = false;
		}
		else if (m_hit2)
		{
			m_hit2 = false;
			GetRandomCards(m_aiHand);
			GetRandomCards(m_aiHand);
		}
		else
		{

			bool noplay = true;
			Sleep(1000);
			for (int i = 0; i < m_aiHand.size(); i++)
			{
				
				if (m_topCard.color == m_aiHand[i].color ||
					m_topCard.type == m_aiHand[i].type || m_aiHand[i].color == 4)
				{
					m_topCard = m_aiHand[i];

					m_aiHand[i] = m_aiHand.back();
					m_aiHand.pop_back();

					if (m_topCard.type == DISCARD_ALL)
					{
						for (int i = 0; i < m_aiHand.size(); i++)
						{
							if (m_aiHand[i].color == m_topCard.color)
							{
								m_aiHand[i] = m_aiHand.back();
								m_aiHand.pop_back();
								i--;
							}
						}
					}
					if (m_topCard.type == TRADE_HANDS)
					{
						std::vector<Card> temp;
						temp = m_playerHand;
						m_playerHand = m_aiHand;
						m_aiHand = temp;
					}
					if (m_topCard.type == WILD || m_topCard.type == WILD_ALLHIT || m_topCard.type == WILD_HITFIRE)
					{
						m_topCard.color = rand()%4;
						int reds = 0, blues = 0, yellows = 0, greens = 0;
						for (int j = 0; j < m_aiHand.size(); j++)
						{
							switch(m_aiHand[j].color)
							{
								case 0: yellows++; break;
								case 1: reds++; break;
								case 2: greens++; break;
								case 3: blues++; break;
							}
						}
						if (yellows > reds && yellows > greens && yellows > blues)
							m_topCard.color = 0;
						else if (reds > yellows && reds > greens && reds > blues)
							m_topCard.color = 1;
						else if (greens > yellows && greens > reds && greens > blues)
							m_topCard.color = 2;
						else if (blues > yellows && blues > reds && blues > greens)
							m_topCard.color = 3;
						
					}
					if (m_topCard.type == SKIP || m_topCard.type == REVERSE)
					{
						m_skip = true;
					}
					if (m_topCard.type == HIT_TWO)
					{
						m_hit2 = true;
					}

					noplay = false;

					break;
				}

				
			}
			if (noplay)
				GetRandomCards(m_aiHand);
			else m_discardPile.push_back(m_topCard);

			
		}

		m_currTurn = 0;			//MessageBox(0, "can't play", 0, 0);
			
	}

	if (m_cardDeck.size() < 10)
	{
		ReplenishDeck();
		ShuffleCards();
	}

	
	
}

void Test::Draw()
{
	g_engine->GetSprite()->Begin(D3DXSPRITE_ALPHABLEND);

	if (!m_cardDeck.empty())
		g_engine->GetSprite()->Draw(m_tex, &m_cardDeck.back().textureRect, 0, &D3DXVECTOR3(50, 50,0), D3DCOLOR_ARGB(255, 255, 255, 255));

	g_engine->GetSprite()->Draw(m_tex, &m_topCard.textureRect, 0, &D3DXVECTOR3(471, 250, 0), 0xffffffff);
	

	for (int i = m_currCard - 2, j = 0; i <= m_currCard + 2; i++, j++)
	{
		if (i >= 0 && i < m_playerHand.size())
		{
			D3DXVECTOR3 pos(100 * j + 10, 580, 0);
			//if (m_currCard == i)
			g_engine->GetSprite()->Draw(m_tex, &m_playerHand[i].textureRect, 0, &pos, 
				(m_currCard == i) ? D3DCOLOR_ARGB(255, 255, 255, 255) : D3DCOLOR_ARGB(50, 255, 255, 255));
		}
	}
	g_engine->GetSprite()->End();

	// SHOW COLOR
	m_lines->Begin();
	m_lines->Draw(m_lineList, 5, m_colors[m_topCard.color]);
	m_lines->End();

	g_font->Draw(0,0, 0xffffffff, "Number of cards left in the deck %i", m_cardDeck.size());
	g_font->Draw(0,178, 0xffffffff, "Top Card in Deck");

	g_font->Draw(400, 390, 0xffffffff, "DISCARD PILE");

	g_font->Draw(0, 500, m_currTurn == 0 ? 0xffffff00 : 0xffffffff, "player hand has %i cards", m_playerHand.size());
	switch(m_currColor)
	{
		case 0: g_font->Draw(0, 720, 0xffffff00, "current color = yellow"); break;
		case 1: g_font->Draw(0, 720, 0xffff0000, "current color = red"); break;
		case 2: g_font->Draw(0, 720, 0xff00ff00, "current color = green"); break;
		case 3: g_font->Draw(0, 720, 0xff0000ff, "current color = blue"); break;
	}
	

	g_font->Draw(650, 500, m_currTurn == 1 ? 0xffffff00 : 0xffffffff, "ai hand has %i cards", m_aiHand.size());
	/*g_engine->GetSprite()->Draw(m_tex, &m_playerHand[m_currCard].textureRect, 0, &D3DXVECTOR3(0,480,0), 
			D3DCOLOR_ARGB(255, 255, 255, 255));*/
	
	
}

void Test::CalcCardRects()
{
	//CALCULATE RECTS
	for (int i = 0; i < 59; i++)
	{
		int row = i / 8;
		int col = i % 8;
		int x = col * 82;
		int y = row * 128;

		m_cardRects[i].left   = x;
		m_cardRects[i].top    = y;
		m_cardRects[i].right  = x + 82;
		m_cardRects[i].bottom = y + 128;
	}
}

void Test::AddCards()
{
	

	// ADD CARDS
	for (int c = 0; c < 4; c++)
	{
		for (int t = 0; t < 14; t++)
		{
			int i = c * 14 + t;
			Card card;
			card.color = c;
			card.type = t;
			card.textureRect = m_cardRects[i];
			m_cardDeck.push_back(card);
			m_cardDeck.push_back(card);
		}
	}
	//add wild cards
	for (int i = 0; i < 8; i++)
	{
		Card card;
		card.color = 4;
		card.type = WILD;
		card.textureRect = m_cardRects[56];
		m_cardDeck.push_back(card);
		
		/*card.type = WILD_ALLHIT;
		card.textureRect = m_cardRects[57];
		m_cardDeck.push_back(card);

		card.type = WILD_HITFIRE;
		card.textureRect = m_cardRects[58];
		m_cardDeck.push_back(card);*/
	}
	
}

void Test::ShuffleCards()
{
	for (int i = 0; i < m_cardDeck.size(); i++)
	{
		int randcard = rand() % m_cardDeck.size();
		while (randcard == i)
			randcard = rand() % m_cardDeck.size();

		Card tempCard = m_cardDeck[randcard];
		m_cardDeck[randcard] = m_cardDeck[i];
		m_cardDeck[i] = tempCard;
	}
}

void Test::DealCards()
{
	
	m_playerHand.clear();
	m_aiHand.clear();
	for (int i = 0; i < 7; i++)
	{
		if (!m_cardDeck.empty())
		{
			m_playerHand.push_back(m_cardDeck.back());
			m_cardDeck.pop_back();

			m_aiHand.push_back(m_cardDeck.back());
			m_cardDeck.pop_back();
		}
	}
	m_topCard = m_cardDeck.back();
	m_cardDeck.pop_back();
}

void Test::GetRandomCards(std::vector<Card> &hand)
{
	int r = rand()%1000;
	int numCards = 0;
	if (r == 0)
	{
		numCards = rand()%20 + 6;
	}
	else if (r >= 200 && r < 500)
	{
		numCards = 2;
	}
	else if (r >= 500 && r < 700)
	{
		numCards = 3;
	}
	else if (r >= 700)
	{
		numCards = 4;
	}

	for (int i = 0; i < numCards; i++)
	{
		if (!m_cardDeck.empty())
		{
			Card card = m_cardDeck.back();
			m_cardDeck.pop_back();
			hand.push_back(card);
		}
	}
}

void Test::ReplenishDeck()
{
	std::vector<Card> topCard;
	topCard.push_back(m_discardPile.back());
	m_discardPile.pop_back();
	for (int i = 0; i < m_discardPile.size(); i++)
	{
		m_cardDeck.push_back(m_discardPile[i]);
	}
	m_discardPile.clear();
	m_discardPile = topCard;
}