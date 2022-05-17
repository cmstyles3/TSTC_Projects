#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include "../Engine/Engine.h"
#include "Cards.h"

#define MSGID_REQUEST_ID 0x12005
#define MSGID_ASSIGN_ID 0x12006

#define MSGID_ADDCARDS 0x12007

#define MSGID_KICK_PLAYER 0x12008
#define MSGID_CHANGE_STATE 0x12009

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

};

class MultiPlayer : public State
{
	enum {MP_LOBBY, MP_START, MP_PLAY, MP_END};

	public:
		MultiPlayer(DWORD id);

		virtual void Load();
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

		void UpdateStart(float dt);
		void DrawStart();

		void UpdatePlay(float dt);
		void DrawPlay();

		void UpdateEnd();
		void DrawEnd();


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

		// Host controlled variables
		bool deal;
		CardDeck m_deck;
		int dealNum;
		int m_id;

		// variables used by HitLauncher()
		int m_numZerosInRow;
		bool m_firstHit;
		bool m_secondHit;

		CRITICAL_SECTION m_playerCS;
		
};

#endif