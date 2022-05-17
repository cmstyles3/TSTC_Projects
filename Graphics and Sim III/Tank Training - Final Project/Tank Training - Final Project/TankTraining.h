#ifndef TANKTRAINING_H
#define TANKTRAINING_H

#include "../Engine/Engine.h"

//states
enum {PLAY, EDIT};

#define NUM_OBJECTS 2

enum {TREE, BUILDING};

struct Object
{
	int id;
	D3DXVECTOR3 pos;
	D3DXMATRIX trans;
};

#define MAX_RAINDROP 100

struct RainDrop
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 vel;
};

#define MAX_SHELLS 30

struct Shell
{
	bool active;
	Object3D object;
	float speed;
	float timer;
	//D3DXVECTOR3 vel;
};

class TankTraining : public State
{
	public:
		TankTraining();

		void Load();
		void Close();

		void OnResetDevice();

		void Update(float dt);
		void Draw();

		void DrawTerrain();

		void SaveObjects();
		void LoadObjects();

		void ShootShell();
		void UpdateShells(float dt);
		void DrawShells();

	private:
		int m_currState;

		Camera m_camera;
		Terrain *m_terrain;

		Mesh m_tankMesh;
		Object3D m_tankObj;

		int m_currObj;
		Mesh m_objMeshes[NUM_OBJECTS];

		std::vector<Object> m_objects;

		Sound *m_sounds[4];

		Font m_font;

		LPDIRECT3DTEXTURE9 m_rainTex;
		RainDrop m_rainDrops[MAX_RAINDROP];

		LPD3DXMESH m_shellMesh;
		Shell m_shells[MAX_SHELLS];

		bool m_displayControls;
};

#endif