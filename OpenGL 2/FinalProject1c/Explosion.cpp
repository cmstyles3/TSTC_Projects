#include "Explosion.h"

extern Camera g_cam;

void Explosion::StartExplosion(Vector3 pos, int numParticles, GLfloat color[3])
{
	for (int i = 0; i < numParticles; i++)
	{
		Particle temp;
		temp.active = true;
		temp.alpha = 1.0f;
		temp.pos = pos;
		temp.prevPos = pos;
		temp.vel = Vector3(rand()%501-250, rand()%501, rand()%501-250);
		memcpy(temp.color, color, sizeof(GLfloat) * 3);
		AddParticle(temp);
	}
}

void Explosion::Update(float dt)
{
	for (int i = 0; i < GetNumParticles(); i++)
	{
		m_particlelist[i].vel.y -= 100.6f * dt;

		static float count = 0.0f;

		count += dt;
		//if (count > 0.005f)
		{
			count = 0.0f;
			m_particlelist[i].vel.x *= (0.93f);
			m_particlelist[i].vel.y *= (0.93f);
			m_particlelist[i].vel.z *= (0.93f);
		}

		m_particlelist[i].alpha -= 0.1f * dt;

		if (m_particlelist[i].alpha <= 0) m_particlelist[i].active = false;

		m_particlelist[i].pos.x += m_particlelist[i].vel.x * dt;
		m_particlelist[i].pos.y += m_particlelist[i].vel.y * dt;
		m_particlelist[i].pos.z += m_particlelist[i].vel.z * dt;

		m_particlelist[i].obj.SetPosition(m_particlelist[i].pos);

		Vector3 pos = Vector3(m_particlelist[i].pos.x, m_particlelist[i].pos.y, m_particlelist[i].pos.z);
	//	particles[i].obj.Reset();
		m_particlelist[i].obj.SetPosition(pos);
		Vector3 look;
		Vec3Sub(&look, &m_particlelist[i].obj.GetPosition(), &g_cam.GetPosition());
		m_particlelist[i].obj.SetLook(look);	
	}

	bool swapped = false;
	int n = GetNumParticles();
	do
	{
		swapped = false;
		n = n-1;
		for (int i = 0; i < n; i++)
		{
			Vector3 campos = g_cam.GetPosition();
			Vector3 partpos1 = m_particlelist[i].obj.GetPosition();
			Vector3 partpos2 = m_particlelist[i+1].obj.GetPosition();
			float x1 = campos.x - partpos1.x;
			float y1 = campos.y - partpos1.y;
			float z1 = campos.z - partpos1.z;

			float x2 = campos.x - partpos2.x;
			float y2 = campos.y - partpos2.y;
			float z2 = campos.z - partpos2.z;
			Vector3 d1 = Vector3(campos.x - partpos1.x, campos.y - partpos1.y, campos.z - partpos1.z);
			Vector3 d2 = Vector3(campos.x - partpos2.x, campos.y - partpos2.y, campos.z - partpos2.z);
			float dist1 = Vec3Dot(&d1, &d1);
			float dist2 = Vec3Dot(&d2, &d2);

			if (dist1 < dist2)
			{
				Particle temp = m_particlelist[i];
				m_particlelist[i] = m_particlelist[i+1];
				m_particlelist[i+1] = temp;
				swapped = true;
			}
		}
	}
	while(swapped);
}

void Explosion::Draw()
{
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDepthMask(FALSE);
	//glDisable(GL_FOG);
	
	std::vector<Particle> temp;
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	for (int i = 0; i < GetNumParticles(); i++)
	{
		if (m_particlelist[i].active)
		{
			glBindTexture(GL_TEXTURE_2D, m_texture);
			glColor4f(m_particlelist[i].color[0], m_particlelist[i].color[1], m_particlelist[i].color[2], m_particlelist[i].alpha);
			/*glPushMatrix();
			Vector3 pos = m_particlelist[i].pos;
			glTranslatef(pos.x, pos.y, pos.z);
			glutSolidSphere(10.0f, 10, 10);
			glPopMatrix();*/

			glPushMatrix();
			m_particlelist[i].obj.SetTransformMatrix();

			glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-50.0f, 50.0f, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(50.0f, 50.0f, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(50.0f, -50.0f, 0.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-50.0f, -50.0f, 0.0f);
			glEnd();
			glPopMatrix();
			temp.push_back(m_particlelist[i]);
		}
		else
		{
			//m_particlelist[i] = m_particlelist.back();
			//m_particlelist.pop_back();
		}
	}
	m_particlelist = temp;
	glEnable(GL_FOG);
	glDepthMask(TRUE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}