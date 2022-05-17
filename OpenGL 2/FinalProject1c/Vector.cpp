#include "Vector.h"

void Vec3Sub(Vector3 *out, Vector3 *vec1, Vector3 *vec2)
{
	out->x = vec1->x - vec2->x;
	out->y = vec1->y - vec2->y;
	out->z = vec1->z - vec2->z;
}

float Vec3Dot(const Vector3 *vec1, const Vector3 *vec2)
{
	return (vec1->x * vec2->x + vec1->y * vec2->y + vec1->z * vec2->z);
}

float Vec3Length(const Vector3 *vec)
{
	float length = sqrt(vec->x*vec->x + vec->y*vec->y + vec->z*vec->z);
	return (length != 0.0f) ? length : 1.0f;
}

void Vec3Normalize(Vector3 *out, const Vector3 *in)
{
	float length = Vec3Length(in);
	out->x = in->x / length;
	out->y = in->y / length;
	out->z = in->z / length;
}

void Vec3Cross(Vector3 *out, const Vector3 *vec1, const Vector3 *vec2)
{
	out->x = vec1->y * vec2->z - vec1->z * vec2->y;
	out->y = vec1->z * vec2->x - vec1->x * vec2->z;
	out->z = vec1->x * vec2->y - vec1->y * vec2->x;
}
