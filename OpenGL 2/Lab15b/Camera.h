#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>
#include <math.h>
#include <gl/glut.h>

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


class Camera
{
	//enum { LANDOBJECT, AIRCRAFT };

	public:
		Camera();
		Camera(float x, float y, float z);

		void Reset();

		void SetViewMatrix();

		void SetTransformMatrix();

		void Strafe(float units);
		void Walk(float units);
		void Fly(float units);

		void Pitch(float angle);
		void Yaw(float angle);
		void Roll(float angle);

		void SetPosition(const Vector3 &pos);

		void SetLook(Vector3 look);

		Vector3 GetPosition() {return(Vector3(m_position.x, m_position.y, m_position.z));}


	private:
		Vector3 m_right, m_up, m_look;
		Vector3 m_position;
		float m_transform[16];
};

#endif