#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>
#include <gl/glut.h>
#include "vector.h"

class Camera
{
	enum CameraType { LANDOBJECT, HYBRID, AIRCRAFT };

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
		void SetRUL(Vector3 right, Vector3 up, Vector3 look);


		Vector3 GetPosition() {return(Vector3(m_position.x, m_position.y, m_position.z));}

		Vector3 GetLook() {return (m_look);}
		Vector3 GetUp() {return (m_up);}
		Vector3 GetRight() {return (m_right);}


	private:
		Vector3 m_right, m_up, m_look;
		Vector3 m_position;
		float m_transform[16];

		CameraType m_cameraType;

		float m_angle;
};

#endif