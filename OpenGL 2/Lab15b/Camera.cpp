
#include "Camera.h"

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

Camera::Camera()
{
	memset(m_transform, 0, 16 * sizeof(float));
	m_right = Vector3(1.0f, 0.0f, 0.0f);
	m_up = Vector3(0.0f, 1.0f, 0.0f);
	m_look = Vector3(0.0f, 0.0f, -1.0f);
	m_position = Vector3(0, 0, 0);
}

Camera::Camera(float x, float y, float z)
{
	memset(m_transform, 0, 16 * sizeof(float));
	m_right = Vector3(1.0f, 0.0f, 0.0f);
	m_up = Vector3(0.0f, 1.0f, 0.0f);
	m_look = Vector3(0.0f, 0.0f, -1.0f);
	m_transform[0] = 1.0f;
	m_transform[5] = 1.0f;
	m_transform[10] = -1.0f;
	m_transform[15] = 1.0f;
	m_position = Vector3(x, y, z);
	m_transform[12] = x, m_transform[13] = y, m_transform[14] = z;
}

void Camera::Reset()
{
	memset(m_transform, 0, 12 * sizeof(float));
	m_right = Vector3(1.0f, 0.0f, 0.0f);
	m_up = Vector3(0.0f, 1.0f, 0.0f);
	m_look = Vector3(0.0f, 0.0f, -1.0f);
	m_transform[0] = 1.0f;
	m_transform[5] = 1.0f;
	m_transform[10] = -1.0f;
	m_transform[15] = 1.0f;
}

void Camera::SetTransformMatrix()
{
	glMultMatrixf(m_transform);
}


void Camera::SetViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float x = -Vec3Dot(&m_right, &m_position);
	float y = -Vec3Dot(&m_up, &m_position);
	float z =  Vec3Dot(&m_look, &m_position);

	float viewMatrix[16] = 
	{
		m_right.x, m_up.x, -m_look.x, 0,
		m_right.y, m_up.y, -m_look.y, 0,
		m_right.z, m_up.z, -m_look.z, 0,
		        x,      y,         z, 1

	};

	glLoadMatrixf(viewMatrix);
}
void Camera::Walk(float units)
{
	m_transform[12] += m_look.x * units;
	m_transform[13] += m_look.y * units;
	m_transform[14] += m_look.z * units;

	m_position = Vector3(m_transform[12], m_transform[13], m_transform[14]);
}

void Camera::Strafe(float units)
{
	m_transform[12] += m_right.x * units;
	m_transform[13] += m_right.y * units;
	m_transform[14] += m_right.z * units;

	m_position = Vector3(m_transform[12], m_transform[13], m_transform[14]);

}

void Camera::Fly(float units)
{
	m_transform[12] += m_up.x * units;
	m_transform[13] += m_up.y * units;
	m_transform[14] += m_up.z * units;

	m_position = Vector3(m_transform[12], m_transform[13], m_transform[14]);
}

void Camera::Yaw(float angle)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(m_transform);
	glRotatef(angle, 0, 1, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, m_transform);
	glPopMatrix();

	m_right = Vector3(m_transform[0], m_transform[1], m_transform[2]);
	m_look = Vector3(m_transform[8], m_transform[9], m_transform[10]);
}

void Camera::Pitch(float angle)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(m_transform);
	glRotatef(angle, 1, 0, 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, m_transform);
	glPopMatrix();

	m_up = Vector3(m_transform[4], m_transform[5], m_transform[6]);
	m_look = Vector3(m_transform[8], m_transform[9], m_transform[10]);
}

void Camera::Roll(float angle)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(m_transform);
	glRotatef(angle, 0, 0, 1);
	glGetFloatv(GL_MODELVIEW_MATRIX, m_transform);
	glPopMatrix();

	m_right = Vector3(m_transform[0], m_transform[1], m_transform[2]); 
	m_up = Vector3(m_transform[4], m_transform[5], m_transform[6]);
}

void Camera::SetLook(Vector3 look)
{
	m_look = look;

	Vec3Normalize(&m_look, &m_look);

	Vec3Cross(&m_up, &m_look, &m_right);
	Vec3Normalize(&m_up, &m_up);

	Vec3Cross(&m_right, &m_up, &m_look);
	Vec3Normalize(&m_right, &m_right);

	m_transform[0] = m_right.x;
	m_transform[1] = m_right.y;
	m_transform[2] = m_right.z;

	m_transform[4] = m_up.x;
	m_transform[5] = m_up.y;
	m_transform[6] = m_up.z;

	m_transform[8] = m_look.x;
	m_transform[9] = m_look.y;
	m_transform[10] = m_look.z;
}

void Camera::SetPosition(const Vector3 &pos)
{
	m_position = pos;
	m_transform[12] = pos.x;
	m_transform[13] = pos.y;
	m_transform[14] = pos.z;
}
