#ifndef COLLISION_H
#define COLLISION_H

#include <d3dx9.h>

#define EPSILON 0.001f

struct Sphere
{
	D3DXVECTOR3 center;
	float radius;
};

struct AABB
{
	D3DXVECTOR3 min;
	D3DXVECTOR3 max;
	D3DXVECTOR3 c;
};

struct OBB
{
	D3DXVECTOR3 c;
	D3DXVECTOR3 u[3];
	float e[3];
};


int TestAABBAABB(AABB a, AABB b);
int TestSphereSphere(Sphere a, Sphere b);
int TestOBBOBB(OBB &a, OBB &b);

#endif 