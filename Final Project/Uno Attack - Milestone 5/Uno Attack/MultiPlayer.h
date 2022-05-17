#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include "../Engine/Engine.h"
#include "CardParticle.h"
#include "Cards.h"

#define MSGID_ASSIGN_ID 0x12005
#define MSGID_ADDCARDS 0x12006
#define MSGID_KICK_PLAYER 0x12007
#define MSGID_CHANGE_STATE 0x12008
#define MSGID_SET_TOPCARD 0x1209
#define MSGID_PLAYCARD 0x12010
#define MSGID_HITLAUNCHER 0x12011
#define MSGID_DONE_WITH_TURN 0x12012
#define MSGID_CHANGE_TURN 0x12013
#define MSGID_READY 0x12014

struct HitLauncherMsg : public NetworkMessage
{
	int numOfHits;
};

struct ChangeTurnMsg : public NetworkMessage
{
	int turn;
};

struct PlayCardMsg : public NetworkMessage
{
	int locOfCard;
	Card card;
	
	int color; // used for wild cards
	int player; // used for trade hands
};

struct SetTopCardMsg : public NetworkMessage
{
	Card card;
};

struct AssignIDMsg : public NetworkMessage
{
	DPNID assignDpnid;
	int id;
};

struct AddCardsMsg : public NetworkMessage
{
	DPNID addDpnid;
	int n;
	char cards[15];
};

struct ChangeStateMsg : public NetworkMessage
{
	int state;
};

struct Player
{
	DPNID dpnid;
	char name[16];

	int id;
	CardHand hand;

	bool ready;

};

class MultiPlayer : public State
{
	enum {MP_LOBBY, MP_DEAL, MP_PLAY, MP_RESULTS};

	public:
		MultiPlayer(DWORD id);

		virtual void Load() sealed;
		virtual void Close();
		virtual void OnLostDevice();
		virtual void OnResetDevice();
		virtual void Update(float dt);
		virtual void Draw();

		void NetworkMessageHandler(ReceivedMessage *msg);


	private:

		void UpdateLobby(float dt);
		void DrawPlayerList();
		//void DrawLobby();

		void UpdateDeal(float dt);
		void DrawDeal();

		void UpdatePlay(float dt);
		void DrawPlay();

		void UpdateResults();
		void DrawResults();


		void DrawLocalPlayersHand();


		int HitLauncher();
		void ResetLauncher();

	
//-------------- VARIABLES ---------------------------
		LPD3DXSPRITE m_sprite;
		Input *m_input;

		int m_mpState;
		std::vector<Player> m_players;
		int m_currCard;
		int m_currTurn;

		CardParticle m_cardParticle;

		Card m_topCard;
		
		// flags
		bool m_hit2;
		bool m_skip;
		bool m_allHit;
		bool m_hitFire;
		bool m_reverse;

		bool m_playableCard;
		bool m_wildCard;
		bool m_tradehandsCard;
		// used for selecting color for wild card
		int m_currColorSelected;
		// used for selecting player to trade hands
		int m_currPlayerSelected;

		char m_winnersName[16];

		// Host controlled variables
		bool deal;
		CardDeck m_deck;
		CardDeck m_discardPile;
		int m_dealcount;
		int m_id;
		bool m_hostSkip;
		bool m_hostHit2;
		bool m_hostHitFire;

		// variables used by HitLauncher()
		int m_numZerosInRow;
		bool m_firstHit;
		bool m_secondHit;

		

		CRITICAL_SECTION m_playerCS;
		
};

#endif