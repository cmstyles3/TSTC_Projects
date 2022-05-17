// © 2008 STYLESoft. All rights reserved.

// CARDS.CPP
// PROGRAMMED BY COLLIN MUNSON

#include "Cards.h"

RECT g_cardRects[59];

CardDeck::CardDeck()
{
	m_cards.clear();
	//BuildDeck();
}

void CalcCardRects()
{
	//CALCULATE RECTS
	for (int i = 0; i < 59; i++)
	{
		int row = i / 8;
		int col = i % 8;
		int x = col * 82;
		int y = row * 128;

		g_cardRects[i].left   = x;
		g_cardRects[i].top    = y;
		g_cardRects[i].right  = x + 82;
		g_cardRects[i].bottom = y + 128;
	}
}

void CardDeck::BuildDeck()
{
	m_cards.clear();
	//CalcCardRects();
	// ADD CARDS
	for (int c = 0; c < 4; c++)
	{
		for (int t = 0; t < 14; t++)
		{
			int i = c * 14 + t;
			Card card;
			card.id = i;
			card.color = c;
			card.type = t;
			//card.rect = m_cardRects[i];
			m_cards.push_back(card);

			if (card.type != DISCARD_ALL && 
				card.type != TRADE_HANDS)
				m_cards.push_back(card);
		}
	}
	//add wild cards
	for (int i = 0; i < 2; i++)
	{
		Card card;
		card.id = 56;
		card.color = 4;
		card.type = WILD;
		//card.rect = m_cardRects[56];
		m_cards.push_back(card);
		m_cards.push_back(card);
		
		card.type = WILD_ALLHIT;
		card.id = 58;
		m_cards.push_back(card);

		card.type = WILD_HITFIRE;
		card.id = 57;
		m_cards.push_back(card);
	}
}

void CardDeck::AddCard(Card card)
{
	m_cards.push_back(card);
}

void CardDeck::AddCardsToDeck(std::vector<Card> cards)
{
	if (cards.empty())
		return;
	
	for (int i = 0; i < m_cards.size(); i++)
		cards.push_back(m_cards[i]);

	m_cards = cards;
}

void CardDeck::Shuffle()
{
	//srand(2);
	for (int i = m_cards.size()-1; i > 0; i--)
	{
		int randcard = rand()%i;
		Card temp = m_cards[randcard];
		m_cards[randcard] = m_cards[i];
		m_cards[i] = temp;
	}
}

Card CardDeck::GetCard(int i)
{
	return m_cards[i];
}

Card CardDeck::GetBackCard()
{
	return m_cards.back();
}

Card CardDeck::GetCardFromDeck()
{
	if (m_cards.empty())
	{
		BuildDeck();
		Shuffle();
	}
	Card card = m_cards.back();
	m_cards.pop_back();
	return card;
}

void CardDeck::Clear()
{
	m_cards.clear();
}


//CardHand Functions

void CardHand::AddCard(int id)
{
	Card card;
	card.id = id;
	if (id == 56)
	{
		card.type = WILD;
		card.color = NOCOLOR;
	}
	else if (id == 57)
	{
		card.type = WILD_HITFIRE;
		card.color = NOCOLOR;
	}
	else if (id == 58)
	{
		card.type = WILD_ALLHIT;
		card.color = NOCOLOR;
	}
	else
	{	
		card.id = id;
		card.color = id / 14;
		card.type = id % 14;
	}
	AddCard(card);
}

// -FIX -----------
void CardHand::AddCard(Card card)
{
	std::vector<Card> temp;

	if (m_cards.empty())
		temp.push_back(card);

	else
	{
		bool foundspot = false;
		for (int i = 0; i < m_cards.size(); i++)
		{
			
			if (card.id <= m_cards[i].id && foundspot == false)
				
			{
				foundspot = true;
				temp.push_back(card);
			}
			temp.push_back(m_cards[i]);
			 // <--------
			
		}
		if (foundspot == false)
			temp.push_back(card);

		//m_cards.clear();
		
	}
	m_cards = temp;
}

void CardHand::RemoveCard(int i)
{
	std::vector<Card> temp;
	for (int j = 0; j < m_cards.size(); j++)
	{
		if (j != i)
		{
			temp.push_back(m_cards[j]);
		}
	}
	m_cards = temp;
}

Card CardHand::GetCard(int i)
{
	while (i < 0) 
	{
		i += m_cards.size();

	}
	while (i >= m_cards.size())
	{
		i -= m_cards.size();
	}
	return m_cards[i];
}

void CardHand::Clear()
{
	m_cards.clear();
}

int CardHand::GetNumCards()
{
	return m_cards.size();
}

void CardHand::GetNumForEachColor(int &yellow, int &red, int &green, int &blue, int &wild)
{
	for (int i = 0; i < m_cards.size(); i++)
	{
		switch(m_cards[i].color)
		{
			case YELLOW:
				yellow++;
				break;
			case RED:
				red++;
				break;
			case GREEN:
				green++;
				break;
			case BLUE:
				blue++;
				break;
			case NOCOLOR:
				wild++;
				break;
		}
	}
}