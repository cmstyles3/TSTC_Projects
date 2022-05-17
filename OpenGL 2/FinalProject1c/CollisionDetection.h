#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include "Vector.h"

struct Sphere
{
	Vector3 c; //center
	float r; //radius
};

int TestSphereSphere(Sphere a, Sphere b);


#endif
