#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>

struct Particle
{
	int active;
	int x, y, z;
	int id;

};

class ParticleSystem
{
	public:

		//ParticleSystem();

		virtual void Update(float dt) = 0;
		virtual void Draw() = 0;

	private:
//		std::vector<Particle> m_particleList;
};

#endif