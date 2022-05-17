// CAMERA.H

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CAMERA__H
#define _CAMERA__H

#include <d3dx9.h>
#include <fstream>

using namespace std;

class Camera
{
	public:
		enum CAMERA_TYPE{LANDOBJECT, AIRCRAFT};
		Camera();
		Camera(CAMERA_TYPE cameraType);
		~Camera();

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
		D3DXVECTOR3 GetPosition();

		void SetPosition(float x, float y, float z);
		void SetPosition(D3DXVECTOR3 pos);

		void GetLook(D3DXVECTOR3 &look);
		D3DXVECTOR3 GetLook();

		void GetUp(D3DXVECTOR3 &up);
		D3DXVECTOR3 GetUp();

		void GetRight(D3DXVECTOR3 &right);
		D3DXVECTOR3 GetRight();

		void SetCameraType(CAMERA_TYPE cameraType);

		void SetLook(D3DXVECTOR3 look);
		void SetRight(D3DXVECTOR3 right);

		void SetUp(D3DXVECTOR3 up);

		D3DXVECTOR3 GetRealLook();
		D3DXVECTOR3 GetRealUp();
		
	private:
		CAMERA_TYPE  m_CameraType;
		D3DXVECTOR3 m_Position, m_Look, m_Up, m_Right;
		D3DXVECTOR3 realLook, realUp;
		//D3DXVECTOR3 up, right;//, pos;


};


class ChaseCamera
{
	public:
		ChaseCamera();
		~ChaseCamera();

		void UpdateWorldPositions();
		void UpdateViewMatrix();
		void Reset();

		void Update(float dt);

		D3DXMATRIX GetViewMatrix();

		void SetChasePosition(D3DXVECTOR3 chasePosition);
		void SetChaseDirection(D3DXVECTOR3 chaseDirection);
		void SetUp(D3DXVECTOR3 up);
		void SetDesiredPositionOffset(D3DXVECTOR3 desiredPositionOffset);
		void SetLookAtOffset(D3DXVECTOR3 lookAtOffset);
	private:
		D3DXVECTOR3 m_chasePosition;
		D3DXVECTOR3 m_chaseDirection;
		D3DXVECTOR3 m_up;

		D3DXVECTOR3 m_desiredPositionOffset;
		D3DXVECTOR3 m_desiredPosition;
		D3DXVECTOR3 m_lookAtOffset;
		D3DXVECTOR3 m_lookAt;

		float m_stiffness;
		float m_damping;
		float m_mass;

		D3DXVECTOR3 m_position;
		D3DXVECTOR3 m_velocity;

		D3DXMATRIX m_view;

};

#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////