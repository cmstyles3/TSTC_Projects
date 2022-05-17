// © 2008 STYLESoft. All rights reserved.

// CARDS.H
// PROGRAMMED BY COLLIN MUNSON

#ifndef CARDS_H
#define CARDS_H

#include <iostream>
#include <vector>
#include <windows.h>

enum
{	
	YELLOW,
	RED,
	GREEN,
	BLUE,
	NOCOLOR
};

enum 
{
	ONE, 
	TWO, 
	THREE, 
	FOUR, 
	FIVE, 
	SIX, 
	SEVEN, 
	EIGHT, 
	NINE,
	REVERSE,
	SKIP,
	HIT_TWO,  
	DISCARD_ALL,
	TRADE_HANDS,
	WILD,
	WILD_ALLHIT,
	WILD_HITFIRE
};

struct Card
{
	int id;
	//RECT rect;
	int color;
	int type;
};

void CalcCardRects();

extern RECT g_cardRects[59];


class CardDeck
{
	public:
		CardDeck();
		void BuildDeck();
		void AddCard(Card card);
		void AddCardsToDeck(std::vector<Card> cards);
		int GetNumCards() { return m_cards.size(); }
		std::vector<Card> GetCards() { return m_cards; }
		void Shuffle();
		Card GetCard(int i);
		Card GetBackCard();
		Card GetCardFromDeck();
		void Clear();
		

	private:
		//data
		//RECT m_cardRects[59];
		std::vector<Card> m_cards;

};

class CardHand
{
	public:
		void AddCard(int id);
		void AddCard(Card card);
		Card GetCard(int i);
		void RemoveCard(int i);
		void Clear();
		int GetNumCards();

		void GetNumForEachColor(int &yellow, int &red, int &green, int &blue, int &wild);
	private:
		std::vector<Card> m_cards;
		
};

#endif