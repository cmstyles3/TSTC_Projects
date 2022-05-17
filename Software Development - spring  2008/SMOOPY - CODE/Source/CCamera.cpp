#include "CCamera.h"



CCamera::CCamera()
{
	m_CameraType = AIRCRAFT;

	m_Position  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Right     = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_Up        = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_Look      = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

CCamera::CCamera(CAMERA_TYPE cameraType)
{
}

CCamera::~CCamera()
{
}

void CCamera::Walk(float units)
{
	switch(m_CameraType)
	{
		case LANDOBJECT:
			m_Position += D3DXVECTOR3(m_Look.x, 0.0f, m_Look.z) * units;
			break;
		case AIRCRAFT:
			m_Position += m_Look * units;
			break;
	}
}

void CCamera::Strafe(float units)
{
	switch(m_CameraType)
	{
		case LANDOBJECT:
			m_Position += D3DXVECTOR3(m_Right.x, 0.0f, m_Right.z) * units;
			break;
		case AIRCRAFT:
			m_Position += m_Right * units;
			break;
	}
}

void CCamera::Fly(float units)
{
	switch(m_CameraType)
	{
		case LANDOBJECT:
			m_Position.y += units;
			break;
		case AIRCRAFT:
			m_Position += m_Up * units;
			break;
	}
}

void CCamera::Pitch(float angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_Right, angle);

	D3DXVec3TransformCoord(&m_Up, &m_Up, &T);
	D3DXVec3TransformCoord(&m_Look, &m_Look, &T);

}

void CCamera::Yaw(float angle)
{
	D3DXMATRIX T;

	if (m_CameraType == LANDOBJECT)
		D3DXMatrixRotationY(&T, angle);

	if (m_CameraType == AIRCRAFT)
		D3DXMatrixRotationAxis(&T, &m_Up, angle);

	D3DXVec3TransformCoord(&m_Right, &m_Right, &T);
	D3DXVec3TransformCoord(&m_Look, &m_Look, &T);
}

void CCamera::Roll(float angle)
{
	D3DXMATRIX T;

	if (m_CameraType == AIRCRAFT)
	{
		D3DXMatrixRotationAxis(&T, &m_Look, angle);

		D3DXVec3TransformCoord(&m_Right, &m_Right, &T);
		D3DXVec3TransformCoord(&m_Up, &m_Up, &T);
	}

}

D3DXMATRIX CCamera::GetMatrix()
{
	D3DXMATRIX world, rot, trans;
	
	D3DXVec3Normalize(&m_Look, &m_Look);
 
	D3DXVECTOR3 pos(m_Position.x , m_Position.y , m_Position.z);

	D3DXVec3Cross(&m_Up, &m_Look, &m_Right);
	D3DXVec3Normalize(&m_Up, &m_Up);

	D3DXVec3Cross(&m_Right, &m_Up, &m_Look);
	D3DXVec3Normalize(&m_Right, &m_Right);
	D3DXMatrixIdentity(&rot);
	rot(0, 0) = m_Right.x;	rot(0, 1) = m_Up.x;  rot(0, 2) = m_Look.x;  rot(0, 3) = 0.0f;

	rot(1, 0) = m_Right.y;  rot(1, 1) = m_Up.y;  rot(1, 2) = m_Look.y;  rot(1, 3) = 0.0f;

	rot(2, 0) = m_Right.z; 	rot(2, 1) = m_Up.z;  rot(2, 2) = m_Look.z;  rot(2, 3) = 0.0f;

	D3DXMatrixTranslation(&trans, pos.x, pos.y, pos.z);
	D3DXMatrixInverse(&rot, 0, &rot);
	world = rot * trans;
	return(world);

}

void CCamera::UpdateThirdPerson(D3DXVECTOR3 targetPos, D3DXMATRIX *V, float dt)
{
	D3DXVec3Normalize(&m_Look, &m_Look);

	D3DXVec3Cross(&m_Up, &m_Look, &m_Right);
	D3DXVec3Normalize(&m_Up, &m_Up);

	D3DXVec3Cross(&m_Right, &m_Up, &m_Look);
	D3DXVec3Normalize(&m_Right, &m_Right);

	D3DXVECTOR3 movementDirection = targetPos - m_Position;

	float length = D3DXVec3Length(&movementDirection);
	float distanceToMove = 10.0f * (length * dt);
	D3DXVec3Normalize(&movementDirection, &movementDirection);

	m_Position += (movementDirection * distanceToMove);

	float x = -D3DXVec3Dot(&m_Right, &m_Position);
	float y = -D3DXVec3Dot(&m_Up, &m_Position);
	float z = -D3DXVec3Dot(&m_Look, &m_Position);

	(*V)(0, 0) = m_Right.x;	(*V)(0, 1) = m_Up.x;  (*V)(0, 2) = m_Look.x;  (*V)(0, 3) = 0.0f;

	(*V)(1, 0) = m_Right.y; (*V)(1, 1) = m_Up.y;  (*V)(1, 2) = m_Look.y;  (*V)(1, 3) = 0.0f;

	(*V)(2, 0) = m_Right.z; (*V)(2, 1) = m_Up.z;  (*V)(2, 2) = m_Look.z;  (*V)(2, 3) = 0.0f;

	(*V)(3, 0) = x;         (*V)(3, 1) = y;	      (*V)(3, 2) = z; 	      (*V)(3, 3) = 1.0f;


}

void CCamera::GetViewMatrix(D3DXMATRIX* V)
{
	//m_Position += vel;	
	
	D3DXVec3Normalize(&m_Look, &m_Look);

	D3DXVec3Cross(&m_Up, &m_Look, &m_Right);
	D3DXVec3Normalize(&m_Up, &m_Up);

	D3DXVec3Cross(&m_Right, &m_Up, &m_Look);
	D3DXVec3Normalize(&m_Right, &m_Right);

	
	float x = -D3DXVec3Dot(&m_Right, &m_Position);
	float y = -D3DXVec3Dot(&m_Up, &m_Position);
	float z = -D3DXVec3Dot(&m_Look, &m_Position);

	(*V)(0, 0) = m_Right.x;	(*V)(0, 1) = m_Up.x;  (*V)(0, 2) = m_Look.x;  (*V)(0, 3) = 0.0f;

	(*V)(1, 0) = m_Right.y;  (*V)(1, 1) = m_Up.y;  (*V)(1, 2) = m_Look.y;  (*V)(1, 3) = 0.0f;

	(*V)(2, 0) = m_Right.z; 	(*V)(2, 1) = m_Up.z;  (*V)(2, 2) = m_Look.z;  (*V)(2, 3) = 0.0f;

	(*V)(3, 0) = x;        (*V)(3, 1) = y;	 (*V)(3, 2) = z; 	    (*V)(3, 3) = 1.0f;
	
	//Device->SetTransform(D3DTS_VIEW, &V);
}

void CCamera::GetPosition(float &x, float &y, float &z)
{
	x = m_Position.x;
	y = m_Position.y;
	z = m_Position.z;
}

void CCamera::SetPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void CCamera::GetLook(D3DXVECTOR3 &look)
{
	look = m_Look;
}

void CCamera::GetUp(D3DXVECTOR3 &up)
{
	up = m_Up;
}

void CCamera::GetRight(D3DXVECTOR3 &right)
{
	right = m_Right;
}

void CCamera::SetLook(D3DXVECTOR3 look)
{
	m_Look = look;
}

void CCamera::SetRight(D3DXVECTOR3 right)
{
	m_Right = right;
}

void CCamera::SetUp(D3DXVECTOR3 up)
{
	m_Up = up;
}