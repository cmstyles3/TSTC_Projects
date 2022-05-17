#ifndef DUCKSHOOTGAME_H
#define DUCKSHOOTGAME_H

#include "engine.h"
#include "vertex.h"
#include "font.h"

#define DUCKSHOOTGAME_STATE 2

#define MAX_PARTICLES 100
#define MAX_EXPLOSIONS 30

#define MAX_DUCKS 5

struct Particle
{
	float x, y;
	float xv, yv;
	D3DCOLOR color;
};

struct Explosion
{
	Particle particles[MAX_PARTICLES];
	bool active;
	float life;
};


enum {DEAD, DYING, ALIVE};

struct Duck
{
	D3DXVECTOR3 pos;
	float speed;
	int state;
};

class DuckShootGameState : public State
{

	enum {START, PLAY, RESULT};

	public:
		DuckShootGameState();

		virtual void Load();
		virtual void Close();

		virtual void OnLostDevice();
		virtual void OnResetDevice();

		virtual void Update(float dt);
		virtual void Draw();

		void MoveDucks(float dt);

		void StartExplosion(float x, float y);

		void MoveExplosions(float dt);

		void DrawExplosions();

	private:
		LPDIRECT3DTEXTURE9 m_duckTex, m_crosshairTex, m_bkgdTex;

		LPDIRECT3DTEXTURE9 m_ammoTex;
		LPD3DXMESH m_boxAmmo;
		float m_boxRot;

		D3DXVECTOR3 m_duckPos;
		int m_numShots;
		int m_numHits;

		Duck m_ducks[MAX_DUCKS];

		D3DXVECTOR3 m_position;
		Sound *m_duckSound, *m_gunSound;
		bool m_duckDead;
		float m_speed;

		int m_currentGameState;

		D3DXMATRIX m_proj, m_view;

		bool m_displayHelp;
		Font *m_font;

		bool m_target;

		Explosion m_explosions[MAX_EXPLOSIONS];
};

#endif