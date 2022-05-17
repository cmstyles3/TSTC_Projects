#ifndef PARTICLE_H
#define PARTICLE_H

//#include "Main.h"
#include "vector.h"
#include "camera.h"

#include <vector>

struct Particle
{
	bool active;
	Vector3 pos;
	Vector3 prevPos;
	Vector3 vel;
	float alpha;
	Camera obj;
	GLfloat color[3];
};

class ParticleSystem
{
	public:
		virtual void Update(float dt) = 0;
		virtual void Draw() = 0;

		void AddParticle(const Particle &particle);

		int GetNumParticles() { return m_particlelist.size(); }
	protected:
		std::vector<Particle> m_particlelist;

};

#endif