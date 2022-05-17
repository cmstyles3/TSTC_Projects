#ifndef DARTTHROWGAME_H
#define DARTTHROWGAME_H

#include "engine.h"
#include "camera.h"
#include "vertex.h"
#include "font.h"

#define DARTTHROWGAME_STATE 4

#define MAX_BALLOONS 25

#define BALLOON_RADIUS 3

struct Balloon
{
	D3DXVECTOR3 pos;
	DWORD color;
	float popped;
};

struct Dart
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 vel;
};

class DartThrowGameState : public State
{
	enum {START, PLAY, RESULT};
	public:
		DartThrowGameState();

		virtual void Load();
		virtual void Close();

		virtual void OnResetDevice();

		virtual void Update(float dt);
		virtual void Draw();


		void InitBalloons();
		void DrawBalloons();

		void StartDart(float dirx, float diry, float dirz);
		void MoveDart(float dt);
		void DrawDart();

	private:
		int m_currState;
		Camera m_camera;
		D3DXMATRIX m_proj;

		LPDIRECT3DTEXTURE9 m_corkboardTex;
		LPD3DXMESH m_corkboardMesh;

		Balloon m_balloons[MAX_BALLOONS];
		LPD3DXMESH m_balloonMesh;

		LPD3DXMESH m_dartMesh;
		Dart m_dart;

		LPDIRECT3DTEXTURE9 m_bkgdTex;
		LPD3DXMESH m_prizeMesh;
		LPDIRECT3DTEXTURE9 m_prizeTex;
		float m_rotPrize;

		float m_yaw, m_pitch;
		bool m_canThrowDart;
		int m_throws;
		int m_numBalloonsPopped;

		bool m_displayHelp;

		Sound *m_balloonPop;

		Font *m_font;
		
};

#endif
