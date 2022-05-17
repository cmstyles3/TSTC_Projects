/*
This is just for testing out ideas and stuff.
*/

#ifndef TEST_H
#define TEST_H

enum
{
	RED,
	BLUE,
	YELLOW,
	GREEN
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
//cards
struct Card
{
	RECT textureRect;
	int color;
	int type;
};

class Test : public State
{
	public:
		Test(DWORD id);

		virtual void Load();
		virtual void Close();
		virtual void OnLostDevice();
		virtual void OnResetDevice();
		virtual void Update(float dt);
		virtual void Draw();

		void CalcCardRects();
		void AddCards();
		void ShuffleCards();
		void DealCards();
		void GetRandomCards(std::vector<Card> &hand);
		void ReplenishDeck();



	private:
		D3DXVECTOR3 m_cardPos;
		LPDIRECT3DTEXTURE9 m_tex;
		RECT m_cardRects[59];
		D3DXVECTOR3 m_discardPilePos;

		Card m_topCard;
		std::vector<Card> m_discardPile;
		std::vector<Card> m_cardDeck;
		std::vector<Card> m_playerHand;
		std::vector<Card> m_aiHand;
		int m_currCard;
		int m_currTurn;
		int m_currColor;

		bool m_skip;
		bool m_hit2;

		LPD3DXLINE m_lines;
		D3DXVECTOR2 m_lineList[5];
		D3DCOLOR m_colors[5];
		

};

#endif