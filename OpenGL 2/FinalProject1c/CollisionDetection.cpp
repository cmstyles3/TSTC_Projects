#include "CollisionDetection.h"

int TestSphereSphere(Sphere a, Sphere b)
{
	Vector3 d = Vector3(a.c.x - b.c.x, a.c.y - b.c.y, a.c.z - b.c.z);
	float dist2 = Vec3Dot(&d, &d);
	float radiusSum = a.r + b.r;
	return dist2 <= radiusSum * radiusSum;
}