#ifndef EXPLOSION_H
#define EXPLOSION_H

//#include "Main.h"
#include "Particle.h"
#include "Vector.h"
#include <gl/glut.h>


class Explosion : public ParticleSystem
{
	public:
		void SetTexture(GLuint texture) { m_texture = texture;}
		void Update(float dt);
		void Draw();

		void StartExplosion(Vector3 pos, int numParticles, GLfloat color[3]);

	private:
		GLuint m_texture;
};

#endif