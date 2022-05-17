#ifndef EDITTRACK_H
#define EDITTRACK_H

#include "../Engine/Engine.h"


class EditTrack : public State
{
	public:
		EditTrack();

		void Load();
		void Close();

		//void OnLostDevice();
		void OnResetDevice();

		void Update(float dt);
		void Draw();

	private:	
		
		int m_red, m_green, m_blue;
		Object3D m_camObj;
};

#endif