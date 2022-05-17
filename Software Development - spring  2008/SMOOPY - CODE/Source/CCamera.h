#ifndef _CAMERA__H
#define _CAMERA__H

#include <d3dx9.h>
#include <fstream>

using namespace std;

class CCamera
{
	public:
		enum CAMERA_TYPE{LANDOBJECT, AIRCRAFT};
		CCamera();
		CCamera(CAMERA_TYPE cameraType);
		~CCamera();

		void Strafe(float units);
		void Fly(float units);
		void Walk(float units);

		void Pitch(float angle);
		void Yaw(float angle);
		void Roll(float angle);

		D3DXMATRIX GetMatrix();

		void GetViewMatrix(D3DXMATRIX* V);

		void UpdateThirdPerson(D3DXVECTOR3 targetPos, D3DXMATRIX* V, float dt);

		void GetPosition(float &x, float &y, float &z);
		void SetPosition(float x, float y, float z);

		void GetLook(D3DXVECTOR3 &look);

		void GetUp(D3DXVECTOR3 &up);

		void GetRight(D3DXVECTOR3 &right);

		void SetCameraType(CAMERA_TYPE cameraType);

		void SetLook(D3DXVECTOR3 look);
		void SetRight(D3DXVECTOR3 right);

		void SetUp(D3DXVECTOR3 up);
		
	private:
		CAMERA_TYPE  m_CameraType;
		D3DXVECTOR3 m_Position, m_Look, m_Up, m_Right;
		//D3DXVECTOR3 up, right;//, pos;


};


#endif