// OBJECT3D.CPP

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine.h"

Object3D::Object3D()
{
	Reset();
}

Object3D::~Object3D()
{
}

void Object3D::Reset()
{
	m_right    = D3DXVECTOR3(1.0f, 0.0f, 0.0f); 
	m_up       = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_look     = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	m_position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void Object3D::MoveOnX(float units)
{
	m_position += m_right * units;
}

void Object3D::MoveOnY(float units)
{
	m_position += m_up * units;
}

void Object3D::MoveOnZ(float units)
{
	m_position += m_look * units;
}

void Object3D::Pitch(float angle)
{
	D3DXMATRIX r;
	D3DXMatrixRotationAxis(&r, &m_right, angle);

	D3DXVec3TransformCoord(&m_up, &m_up, &r);
	D3DXVec3TransformCoord(&m_look, &m_look, &r);
}

void Object3D::Yaw(float angle)
{
	D3DXMATRIX r;
	D3DXMatrixRotationAxis(&r, &m_up, angle);

	D3DXVec3TransformCoord(&m_right, &m_right, &r);
	D3DXVec3TransformCoord(&m_look, &m_look, &r);
}

void Object3D::Roll(float angle)
{
	D3DXMATRIX r;
	D3DXMatrixRotationAxis(&r, &m_look, angle);

	D3DXVec3TransformCoord(&m_right, &m_right, &r);
	D3DXVec3TransformCoord(&m_up, &m_up, &r);
}

D3DXVECTOR3 Object3D::GetPosition()
{
	return m_position;
}

void Object3D::SetPosition(D3DXVECTOR3 position)
{
	m_position = position;
}

D3DXVECTOR3 Object3D::GetRight()
{
	return m_right;
}

D3DXVECTOR3 Object3D::GetUp()
{
	return m_up;
}

D3DXVECTOR3 Object3D::GetLook()
{
	return m_look;
}

void Object3D::SetRight(D3DXVECTOR3 right)
{
	m_right = right;
}

void Object3D::SetUp(D3DXVECTOR3 up)
{
	m_up = up;
}

void Object3D::SetLook(D3DXVECTOR3 look)
{
	m_look = look;
}

D3DXMATRIX Object3D::GetMatrix()
{
	D3DXMATRIX world, rot, trans;
	
	D3DXVec3Normalize(&m_look, &m_look);

	D3DXVec3Cross(&m_up, &m_look, &m_right);
	D3DXVec3Normalize(&m_up, &m_up);

	D3DXVec3Cross(&m_right, &m_up, &m_look);
	D3DXVec3Normalize(&m_right, &m_right);
	D3DXMatrixIdentity(&rot);
	rot(0, 0) = m_right.x;	rot(0, 1) = m_up.x;  rot(0, 2) = m_look.x;  rot(0, 3) = 0.0f;

	rot(1, 0) = m_right.y;  rot(1, 1) = m_up.y;  rot(1, 2) = m_look.y;  rot(1, 3) = 0.0f;

	rot(2, 0) = m_right.z; 	rot(2, 1) = m_up.z;  rot(2, 2) = m_look.z;  rot(2, 3) = 0.0f;

	D3DXMatrixTranslation(&trans, m_position.x, m_position.y, m_position.z);
	D3DXMatrixInverse(&rot, 0, &rot);
	world = rot * trans;
	return(world);
}

D3DXMATRIX Object3D::GetViewMatrix()
{
	D3DXMATRIX view, rot, trans;
	
	D3DXVec3Normalize(&m_look, &m_look);

	D3DXVec3Cross(&m_up, &m_look, &m_right);
	D3DXVec3Normalize(&m_up, &m_up);

	D3DXVec3Cross(&m_right, &m_up, &m_look);
	D3DXVec3Normalize(&m_right, &m_right);
	D3DXMatrixIdentity(&rot);

	D3DXVECTOR3 position = m_position + D3DXVECTOR3(m_up.x * 30.0f + m_look.x * 100.0f, m_up.y * 30.0f + m_look.y * 100.0f, m_up.z * 30.0f + m_look.z * 100.0f);
	float x = -D3DXVec3Dot(&m_right, &position);
	float y = -D3DXVec3Dot(&m_up, &position);
	float z = -D3DXVec3Dot(&m_look, &position);

	rot(0, 0) = m_right.x;	rot(0, 1) = m_up.x;  rot(0, 2) = m_look.x;  rot(0, 3) = 0.0f;

	rot(1, 0) = m_right.y;  rot(1, 1) = m_up.y;  rot(1, 2) = m_look.y;  rot(1, 3) = 0.0f;

	rot(2, 0) = m_right.z; 	rot(2, 1) = m_up.z;  rot(2, 2) = m_look.z;  rot(2, 3) = 0.0f;
	rot(3, 0) = x; rot(3,1) = y; rot(3,2) = z;
	

	
	return(rot);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////