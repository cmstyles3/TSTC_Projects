#ifndef INITLOADING_H
#define INITLOADING_H

#include "engine.h"

#define INITLOADING_STATE 0

class InitLoadingState : public State
{
	public:
		InitLoadingState();

		virtual void Load();
		virtual void Update(float dt);

};

#endif