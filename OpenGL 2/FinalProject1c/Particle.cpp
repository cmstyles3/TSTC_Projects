#include "Particle.h"

void ParticleSystem::AddParticle(const Particle &particle)
{
	m_particlelist.push_back(particle);
}