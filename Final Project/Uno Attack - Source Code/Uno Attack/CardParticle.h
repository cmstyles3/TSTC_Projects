// © 2008 STYLESoft. All rights reserved.

// CARDPARTICLE.H
// PROGRAMMED BY COLLIN MUNSON

#ifndef CARDPARTICLE_H
#define CARDPARTICLE_H

#include "../Engine/Engine.h"

class CardParticle : public ParticleSystem
{
	public:
		CardParticle();

		void Clear();

		void Update(float dt);
		void Draw();

		void AddCard(int id);

	private:
		float timer;
		std::queue<Particle> cardQueue;
		std::vector<Particle> m_particleList;
		Particle cardList[60];


};

#endif

