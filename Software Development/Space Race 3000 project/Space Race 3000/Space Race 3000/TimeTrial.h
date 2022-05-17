#ifndef TIMETRIAL_H
#define TIMETRIAL_H

#include "../Engine/Engine.h"

class TimeTrial : public State
{
	enum {START, INITGAME, COUNTDOWN, PLAYGAME, RESULTS};

	public:
		TimeTrial();

		void Load();
		void Close();

		void OnLostDevice();
		void OnResetDevice();

		void Update(float dt);
		void Draw();

	private:
		Camera m_camera;
		Object3D m_playerShip;

		int m_currState;

		float m_shipSpeed;
		int m_throttle;

		float m_countDown;
		float m_raceTimer;

		int m_currRing;

};

#endif