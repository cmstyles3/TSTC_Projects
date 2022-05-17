#include "Main.h"

CardParticle::CardParticle()
{
	timer = 0.0f;
	m_particleList.clear();
}

void CardParticle::Clear()
{
	while(!cardQueue.empty())
		cardQueue.pop();
	m_particleList.clear();
}

void CardParticle::AddCard(int id)
{
	Particle cardParticle;
	cardParticle.id = id;
	cardParticle.x = 512;
	cardParticle.y = 150;
	cardParticle.z = 0;
	cardQueue.push(cardParticle);
	//m_particleList.push_back(cardParticle);
}

void CardParticle::Update(float dt)
{
	timer += dt;

	if (timer >= .1f && !cardQueue.empty())
	{
		timer = 0.0f;
		Particle particle = cardQueue.front();
		m_particleList.push_back(particle);
		cardQueue.pop();
	}
	/*if (m_particleList.size() > 1)
		MessageBox(0,0,0,0);*/
	
	for (int i = 0; i < m_particleList.size(); i++)
	{
		m_particleList[i].y += 1200 * dt;
		if (m_particleList[i].y > 900)
		{
			m_particleList[i] = m_particleList.back();
			m_particleList.pop_back();
		}
	}

}

void CardParticle::Draw()
{
	LPD3DXSPRITE sprite = g_engine->GetSprite();
	for (int i = 0; i < m_particleList.size(); i++)
	{
		D3DXVECTOR3 pos(512, m_particleList[i].y, 0);
		sprite->Draw(g_cardTexture, &g_cardRects[m_particleList[i].id], &D3DXVECTOR3(41,128,0), &pos, 0xffffffff);
	}
}