#ifndef BASEBALLTOSSGAME_H
#define BASEBALLTOSSGAME_H

#include "main.h"
#include "camera.h"
#include "vertex.h"
#include "font.h"

#define BASEBALLTOSSGAME_STATE 3

#define NUM_BOTTLES 3

struct Bottle
{
	D3DXVECTOR3 pos;
};

class BaseballTossGameState : public State
{
	enum {START, PLAY, RESULT};
	public:
		BaseballTossGameState();
		
		virtual void Load();
		virtual void Close();

		virtual void OnResetDevice();

		virtual void Update(float dt);
		virtual void Draw();

	private:
		int m_currState;
		D3DXMATRIX m_proj;

		LPDIRECT3DTEXTURE9 m_bkgdTex;

		LPD3DXMESH m_ball;

		LPD3DXMESH m_bottleMesh;

		Camera m_camera;

		Bottle m_bottles[NUM_BOTTLES];

		Font *m_font;

		
};

#endif