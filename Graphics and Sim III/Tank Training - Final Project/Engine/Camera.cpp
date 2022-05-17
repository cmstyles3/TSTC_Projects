// CAMERA.CPP

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// B E G I N N I N G ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

#include "Engine.h"


Camera::Camera()
{
	m_CameraType = AIRCRAFT;

	m_Position  = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Right     = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_Up        = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_Look      = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
}

Camera::Camera(CAMERA_TYPE cameraType)
{
}

Camera::~Camera()
{
}

void Camera::Walk(float units)
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

void Camera::Strafe(float units)
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

void Camera::Fly(float units)
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

void Camera::Pitch(float angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_Right, angle);

	D3DXVec3TransformCoord(&m_Up, &m_Up, &T);
	D3DXVec3TransformCoord(&m_Look, &m_Look, &T);

}

void Camera::Yaw(float angle)
{
	D3DXMATRIX T;

	if (m_CameraType == LANDOBJECT)
		D3DXMatrixRotationY(&T, angle);

	if (m_CameraType == AIRCRAFT)
		D3DXMatrixRotationAxis(&T, &m_Up, angle);

	D3DXVec3TransformCoord(&m_Right, &m_Right, &T);
	D3DXVec3TransformCoord(&m_Look, &m_Look, &T);
}

void Camera::Roll(float angle)
{
	D3DXMATRIX T;

	if (m_CameraType == AIRCRAFT)
	{
		D3DXMatrixRotationAxis(&T, &m_Look, angle);

		D3DXVec3TransformCoord(&m_Right, &m_Right, &T);
		D3DXVec3TransformCoord(&m_Up, &m_Up, &T);
	}

}

D3DXMATRIX Camera::GetMatrix()
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

void Camera::UpdateThirdPerson(D3DXVECTOR3 targetPos, D3DXMATRIX *V, float dt)
{
	D3DXVec3Normalize(&m_Look, &m_Look);

	D3DXVec3Cross(&m_Up, &m_Look, &m_Right);
	D3DXVec3Normalize(&m_Up, &m_Up);

	D3DXVec3Cross(&m_Right, &m_Up, &m_Look);
	D3DXVec3Normalize(&m_Right, &m_Right);

	D3DXVECTOR3 movementDirection = targetPos - m_Position;

	float length = D3DXVec3Length(&movementDirection);
	float distanceToMove = 5.0f * (length * dt);
	D3DXVec3Normalize(&movementDirection, &movementDirection);

	m_Position += (movementDirection * distanceToMove);

	float x = -D3DXVec3Dot(&m_Right, &m_Position);
	float y = -D3DXVec3Dot(&m_Up, &m_Position);
	float z = -D3DXVec3Dot(&m_Look, &m_Position);

	(*V)(0, 0) = m_Right.x;	(*V)(0, 1) = m_Up.x;  (*V)(0, 2) = m_Look.x;  (*V)(0, 3) = 0.0f;

	(*V)(1, 0) = m_Right.y; (*V)(1, 1) = m_Up.y;  (*V)(1, 2) = m_Look.y;  (*V)(1, 3) = 0.0f;

	(*V)(2, 0) = m_Right.z; (*V)(2, 1) = m_Up.z;  (*V)(2, 2) = m_Look.z;  (*V)(2, 3) = 0.0f;

	(*V)(3, 0) = x;         (*V)(3, 1) = y;	      (*V)(3, 2) = z; 	      (*V)(3, 3) = 1.0f;

	D3DXMatrixLookAtLH(V, &m_Position, &(targetPos + m_Look * 20.0f),&m_Up);

	D3DXVECTOR3 target = targetPos + m_Look * 20.0f;
	realLook = target - m_Position;

	D3DXVec3Normalize(&realLook, &realLook);
	D3DXVec3Cross(&realUp, &realLook, &m_Right);
	D3DXVec3Normalize(&realUp, &realUp);
}

D3DXVECTOR3 Camera::GetRealLook()
{
	return realLook;
}

D3DXVECTOR3 Camera::GetRealUp()
{
	return realUp;
}

void Camera::GetViewMatrix(D3DXMATRIX* V)
{
	//m_Position += vel;	
	
	D3DXVec3Normalize(&m_Look, &m_Look);

	D3DXVec3Cross(&m_Up, &m_Look, &m_Right);
	D3DXVec3Normalize(&m_Up, &m_Up);

	D3DXVec3Cross(&m_Right, &m_Up, &m_Look);
	D3DXVec3Normalize(&m_Right, &m_Right);

	D3DXVECTOR3 pos  = m_Position; //+ D3DXVECTOR3(0.0f, 1000.0f, 4000.0f);
	float x = -D3DXVec3Dot(&m_Right, &pos);
	float y = -D3DXVec3Dot(&m_Up, &pos);
	float z = -D3DXVec3Dot(&m_Look, &pos);

	(*V)(0, 0) = m_Right.x;	(*V)(0, 1) = m_Up.x;  (*V)(0, 2) = m_Look.x;  (*V)(0, 3) = 0.0f;

	(*V)(1, 0) = m_Right.y;  (*V)(1, 1) = m_Up.y;  (*V)(1, 2) = m_Look.y;  (*V)(1, 3) = 0.0f;

	(*V)(2, 0) = m_Right.z; 	(*V)(2, 1) = m_Up.z;  (*V)(2, 2) = m_Look.z;  (*V)(2, 3) = 0.0f;

	(*V)(3, 0) = x;        (*V)(3, 1) = y;	 (*V)(3, 2) = z; 	    (*V)(3, 3) = 1.0f;
	
	//Device->SetTransform(D3DTS_VIEW, &V);
}

void Camera::GetPosition(float &x, float &y, float &z)
{
	x = m_Position.x;
	y = m_Position.y;
	z = m_Position.z;
}

D3DXVECTOR3 Camera::GetPosition()
{
	return m_Position;
}

void Camera::SetPosition(float x, float y, float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void Camera::SetPosition(D3DXVECTOR3 pos)
{
	m_Position = pos;
}

void Camera::GetLook(D3DXVECTOR3 &look)
{
	look = m_Look;
}

D3DXVECTOR3 Camera::GetLook()
{
	return m_Look;
}

void Camera::GetUp(D3DXVECTOR3 &up)
{
	up = m_Up;
}

D3DXVECTOR3 Camera::GetUp()
{
	return m_Up;
}

void Camera::GetRight(D3DXVECTOR3 &right)
{
	right = m_Right;
}

D3DXVECTOR3 Camera::GetRight()
{
	return m_Right;
}

void Camera::SetLook(D3DXVECTOR3 look)
{
	m_Look = look;
}

void Camera::SetRight(D3DXVECTOR3 right)
{
	m_Right = right;
}

void Camera::SetUp(D3DXVECTOR3 up)
{
	m_Up = up;
}

ChaseCamera::ChaseCamera()
{
	m_stiffness = 1000.0f;
	m_damping = 300.0f;
	m_mass = 3.0f;
}
ChaseCamera::~ChaseCamera()
{
}

void ChaseCamera::UpdateWorldPositions()
{
	D3DXMATRIX transform;
	D3DXMatrixIdentity(&transform);
	transform(0, 0) = m_chaseDirection.x;
	transform(1, 0) = m_chaseDirection.y;
	transform(2, 0) = m_chaseDirection.z;

	D3DXVec3Normalize(&m_up, &m_up);
	transform(0, 1) = m_up.x;
	transform(1, 1) = m_up.y;
	transform(2, 1) = m_up.z;

	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &m_up, &m_chaseDirection);
	D3DXVec3Normalize(&right, &right);
	transform(0, 2) = right.x;
	transform(1, 2) = right.y;
	transform(2, 2) = right.z;

	D3DXVECTOR3 positionOffset;
	D3DXVec3TransformNormal(&positionOffset, &m_desiredPositionOffset, &transform); 
	m_desiredPosition =  m_chasePosition;// + positionOffset;

	D3DXVECTOR3 lookAtOffset;
	D3DXVec3TransformNormal(&lookAtOffset, &m_lookAtOffset, &transform);
	m_lookAt = m_chasePosition;// + lookAtOffset;
}

void ChaseCamera::UpdateViewMatrix()
{
	D3DXMatrixLookAtLH(&m_view, &(m_position+m_chaseDirection*-200 + m_up*40), &(m_lookAt + m_chaseDirection + m_up*40), &m_up);
}

void ChaseCamera::Reset()
{
	UpdateWorldPositions();
	
	m_velocity = D3DXVECTOR3(0.0f, 0.0f, 0.f);
	m_position = m_desiredPosition;
	m_position = D3DXVECTOR3(0.0f,0.0f, 0.0f);
	m_chaseDirection = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	UpdateViewMatrix();
}

void ChaseCamera::Update(float dt)
{
	UpdateWorldPositions();


	D3DXVECTOR3 stretch = m_position - m_desiredPosition;
	
	D3DXVECTOR3 force = -m_stiffness * stretch - m_damping * m_velocity;

	D3DXVECTOR3 acceleration = force/m_mass;
	m_velocity += acceleration * dt;
	m_position += m_velocity * dt;

	UpdateViewMatrix();
}


D3DXMATRIX ChaseCamera::GetViewMatrix()
{
	return m_view;
}

void ChaseCamera::SetChasePosition(D3DXVECTOR3 chasePosition)
{
	m_chasePosition = chasePosition;
}
void ChaseCamera::SetChaseDirection(D3DXVECTOR3 chaseDirection)
{
	m_chaseDirection = chaseDirection;
}
void ChaseCamera::SetUp(D3DXVECTOR3 up)
{
	m_up = up;
}
void ChaseCamera::SetDesiredPositionOffset(D3DXVECTOR3 desiredPositionOffset)
{
	m_desiredPositionOffset = desiredPositionOffset;
}

void ChaseCamera::SetLookAtOffset(D3DXVECTOR3 lookAtOffset)
{
	m_lookAtOffset = lookAtOffset;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMMED BY COLLIN MUNSON /////////////////// E N D I N G /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////