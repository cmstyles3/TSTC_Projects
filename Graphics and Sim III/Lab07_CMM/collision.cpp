#include "collision.h"

int TestAABBAABB(AABB a, AABB b)
{
	
	if ((a.max.x + a.c.x) < (b.min.x + b.c.x) || (a.min.x + a.c.x) > (b.max.x + b.c.x) ||
		(a.max.y + a.c.y) < (b.min.y + b.c.y) || (a.min.y + a.c.y) > (b.max.y + b.c.y) ||
		(a.max.z + a.c.z) < (b.min.z + b.c.z) || (a.min.z + a.c.z) > (b.max.z + b.c.z))
		return 0;
	return 1;
}

int TestSphereSphere(Sphere a, Sphere b)
{
	D3DXVECTOR3 d = a.center - b.center;
	float dist2 = D3DXVec3Dot(&d, &d);
	float radiusSum = a.radius + b.radius;
	return dist2 <= radiusSum * radiusSum;
}

int TestOBBOBB(OBB &a, OBB &b)
{
	float ra, rb;
	float R[3][3], absR[3][3];

	// Compute rotation matrix expressing b in a's coordinate frame
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			R[i][j] = D3DXVec3Dot(&a.u[i], &b.u[j]);

	// Compute translation vector t
	D3DXVECTOR3 t = b.c - a.c;

	// Bring translation into a's coordinate frame
	t = D3DXVECTOR3(D3DXVec3Dot(&t, &a.u[0]), D3DXVec3Dot(&t, &a.u[1]), D3DXVec3Dot(&t, &a.u[2]));

	// Compute common subexpressions.  Add in an epsilon term to
	// counteract arithmetic errors when two edges are parallel and 
	// their cross product is (near) null
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			absR[i][j] = abs(R[i][j]) + EPSILON;

	// Test axes L = A0, L = A1, L = A2
	for (int i = 0; i < 3; i++)
	{
		ra = a.e[i];
		rb = b.e[0] * absR[i][0] + b.e[1] * absR[i][1] + b.e[2] * absR[i][2];
		if (abs(t[i]) > ra + rb) return 0;
	}

	// Test axes L = B0, L = B1, L = B2
	for (int i = 0; i < 3; i++)
	{
		ra = a.e[0] * absR[0][i] + a.e[1] * absR[1][i] + a.e[2] * absR[2][i];
		rb = b.e[i];
		if (abs(t[0] * R[0][i] + t[1] * R[1][i] + t[2] * R[2][i]) > ra + rb) return 0;
	}

	// Test axis  L = A0 x B0
	ra = a.e[1] * absR[2][0] + a.e[2] * absR[1][0];
	rb = b.e[1] * absR[0][2] + b.e[2] * absR[0][1];
	if (abs(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return 0;

	// Test axis L = A0 x B1
	ra = a.e[1] * absR[2][1] + a.e[2] * absR[1][1];
	rb = b.e[0] * absR[0][2] + b.e[2] * absR[0][0];
	if (abs(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return 0;

	// Test axis L = A0 x B2
	ra = a.e[1] * absR[2][2] + a.e[2] * absR[1][2];
	rb = b.e[0] * absR[0][1] + b.e[1] * absR[0][0];
	if (abs(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return 0;

	// Test axis L = A1 x B0
	ra = a.e[0] * absR[2][0] + a.e[2] * absR[0][0];
	rb = b.e[1] * absR[1][2] + b.e[2] * absR[1][1];
	if (abs(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return 0;

	// Test axis L = A1 x B1
	ra = a.e[0] * absR[2][1] + a.e[2] * absR[0][1];
	rb = b.e[0] * absR[1][2] + b.e[2] * absR[1][0];
	if (abs(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return 0;

	// Test axis L = A1 x B2
	ra = a.e[0] * absR[2][2] + a.e[2] * absR[0][2];
	rb = b.e[0] * absR[1][1] + b.e[1] * absR[1][0];
	if (abs(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return 0;

	// Test axis L = A2 x B0
	ra = a.e[0] * absR[1][0] + a.e[1] * absR[0][0];
	rb = b.e[1] * absR[2][2] + b.e[2] * absR[2][1];
	if (abs(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return 0;

	// Test axis L = A2 x B1
	ra = a.e[0] * absR[1][1] + a.e[1] * absR[0][1];
	rb = b.e[0] * absR[2][2] + b.e[2] * absR[2][0];
	if (abs(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return 0;

	// Test axis L = A2 x B2
	ra = a.e[0] * absR[1][2] + a.e[1] * absR[0][2];
	rb = b.e[0] * absR[2][1] + b.e[1] * absR[2][0];
	if (abs(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return 0;

	// Since no separating axis is found, the OBBs must be intersecting
	return(1);
}