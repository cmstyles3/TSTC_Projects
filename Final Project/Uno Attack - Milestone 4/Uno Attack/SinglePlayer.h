#ifndef SINGLEPLAYER_H
#define SINGLEPLAYER_H

#include "CardParticle.h"
#include "Cards.h"

class SinglePlayer : public State
{
	enum { SP_START, SP_PLAY, SP_END }; // SINGLE PLAYER GAME STATES

	enum {PLAYER, COMPUTER}; // USED FOR TURN

	public:
		SinglePlayer(DWORD id);

		virtual void Load();
		virtual void Close();
		virtual void OnLostDevice();
		virtual void OnResetDevice();
		virtual void Update(float dt);
		virtual void Draw();

		void InitGame();

		void ProcessPlayersControls();
		void DrawPlayersHand();
		
		void DisplayInfo();

		void InitLauncher();
		int HitLauncher();

	private:
		int m_spState; // KEEPS TRACK OF CURRENT SINGLE PLAYER STATE

		CardDeck m_deck;
		CardDeck m_discardPile;

		CardHand m_playersHand;
		int m_currCard;

		CardHand m_computersHand;

		int m_currTurn; // KEEPS TRACK OF WHOS TURN IT IS
		bool m_playableCard;

		Card m_topCard;

		float m_timer;
		int m_dealcount;

		bool m_skip;
		bool m_hit2;

		int m_whowon;

		int m_currColor;

		CardParticle m_cardParticles;

		//Launcher variables
		bool m_firstHit;
		bool m_secondHit;
		int m_numZerosInRow; // if this exceeds two it is sure to fire

};

#endif