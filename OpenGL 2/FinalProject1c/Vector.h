#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

struct Vector3
{
	float x, y, z;
	Vector3() {}
	Vector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

void Vec3Sub(Vector3 *out, Vector3 *vec1, Vector3 *vec2);
float Vec3Dot(const Vector3 *vec1, const Vector3 *vec2);
float Vec3Length(const Vector3 *vec);
void Vec3Normalize(Vector3 *out, const Vector3 *in);
void Vec3Cross(Vector3 *out, const Vector3 *vec1, const Vector3 *vec2);

#endif