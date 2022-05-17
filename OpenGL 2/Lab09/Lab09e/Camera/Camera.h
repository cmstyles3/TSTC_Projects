#include <math.h>
#include "Point3.h"
#include "Vector3.h"
class Camera{
  private:
	Point3 eye;
	Vector3 u, v, n, up;
	double viewAngle, aspect, nearDist, farDist; 		// view volume shape
	void setModelviewMatrix(); 			// tell OpenGL where the camera is

  public:
	Camera(); 					// default constructor
	Point3 lookat;
	void setModelViewMatrix();
	void set(Point3 eye, Point3 look, Vector3 up); 	// like gluLookAt()
	void roll(float angle); 			// roll it
	void pitch(float angle); 			// increase pitch
	void yaw(float angle); 				// yaw it
	void slide(float delU, float delV, float delN); // slide it
//	void rotate (Vector3 axis, float angle);
	void rotate (float angle);
	void setShape(float vAng, float asp, float nearD, float farD);
//	void setOblique(Vector3 d,..others..)
};

Camera::Camera()
{
}

void Camera :: setModelViewMatrix(void)
{ // load modelview matrix with existing camera values
	float m[16];
	Vector3 eVec(eye.x, eye.y, eye.z); // a vector version of eye 
	m[0] =  u.x; m[4] =  u.y; m[8]  =  u.z;  m[12] = -eVec.dot(u);
	m[1] =  v.x; m[5] =  v.y; m[9]  =  v.z;  m[13] = -eVec.dot(v);
	m[2] =  n.x; m[6] =  n.y; m[10] =  n.z;  m[14] = -eVec.dot(n);
	m[3] =  0;   m[7] =  0;   m[11] =  0;    m[15] = 1.0;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m); // load OpenGL’s modelview matrix
}
void Camera:: set(Point3 eyeset, Point3 lookset, Vector3 upset)
{	// create a modelview matrix and send it to OpenGL
	eye.set(eyeset.x, eyeset.y, eyeset.z); 		// store the given eye position
	lookat.set(lookset.x, lookset.y, lookset.z);
	up.set(upset);
	n.set(eyeset.x - lookset.x, eyeset.y - lookset.y, eyeset.z - lookset.z); // make n
	u.set(upset.cross(n)); 		// make u = up X n
	n.normalize(); u.normalize(); 	// make them unit length
	v.set(n.cross(u));  		// make v =  n X u
	setModelViewMatrix(); 		// tell OpenGL 
}

void Camera :: roll(float angle)
{ // roll the camera through angle degrees
	float cs = cos(3.14159265/180 * angle);
	float sn = sin(3.14159265/180 * angle);
	Vector3 t(u); // remember old u
	u.set(cs*t.x - sn*v.x, cs*t.y - sn*v.y, cs*t.z - sn*v.z);
	v.set(sn*t.x + cs*v.x, sn*t.y + cs*v.y, sn*t.z + cs*v.z);
	setModelViewMatrix();
}

void Camera::slide(float delU,float delV,float delN)
{
	eye.x += delU * u.x + delV * v.x + delN * n.x;
	eye.y += delU * u.y + delV * v.y + delN * n.y;
	eye.z += delU * u.z + delV * v.z + delN * n.z;
	setModelViewMatrix();
}

void Camera :: pitch (float angle)
{ // pitch the camera through angle degrees around U
	float cs = cos(3.14159265/180 * angle);
	float sn = sin(3.14159265/180 * angle);
	Vector3 t(v); // remember old v
	v.set(cs*t.x - sn*n.x, cs*t.y - sn*n.y, cs*t.z - sn*n.z);
	n.set(sn*t.x + cs*n.x, sn*t.y + cs*n.y, sn*t.z + cs*n.z);
	setModelViewMatrix();
}

void Camera :: yaw (float angle)
{ // yaw the camera through angle degrees around V
	float cs = cos(3.14159265/180 * angle);
	float sn = sin(3.14159265/180 * angle);
	Vector3 t(n); // remember old n
	n.set(cs*t.x - sn*u.x, cs*t.y - sn*u.y, cs*t.z - sn*u.z);
	u.set(sn*t.x + cs*u.x, sn*t.y + cs*u.y, sn*t.z + cs*u.z);
	setModelViewMatrix();
}

// void Camera :: rotate (Vector3 axis, float angle)
void Camera :: rotate (float angle)
{ // rotate the camera through angle degrees around axis			1.0 deg	45.0 deg	
/*
	float cs = cos(3.14159265/180 * angle);		// 	.9998	.7071
	float sn = sin(3.14159265/180 * angle);		// 	.0175	.7071
	float x1, x2, z2, asq, a;
	float angleB, angleD, angleG, angleH;
	x1 = eye.x - lookat.x;
	asq = (x1* x1) + (x1 * x1) - (2.0 * x1 * x1 * cs);	// law of cosines
	a = sqrt(asq);
	angleB = (180.0 - angle) / 2.0;
	angleD = 180.0 - angleB;
	angleG = angleD - 45.0;
	angleH = 90.0 - angleG;
	float csH = cos(3.14159265/180 * angleH);
	float snH = sin(3.14159265/180 * angleH);
	z2 = a * csH;
	x2 = a * snH;
	if(angle > 0.0)
	{
		lookat.z = lookat.z + z2;
		lookat.x = lookat.x - x2;
	}
	else
	{
		lookat.z = lookat.z - z2;
		lookat.x = lookat.x + x2;
	}
	n.set(eye.x - lookat.x, eye.y - lookat.y, eye.z - lookat.z); 	// make n
	u.set(up.cross(n)); 				// make u = up X n
	n.normalize(); u.normalize(); 			// make them unit length
	v.set(n.cross(u));  				// make v =  n X u
	setModelViewMatrix(); 				// tell OpenGL 

*/
/*
	glVertex3f((GLfloat) radius*cos(angle), (GLfloat) radius*sin(angle), z1);
	angle += angleInc; 
	glVertex3f((GLfloat) radius*cos(angle), (GLfloat) radius*sin(angle), z2);

//	glRotatef(angle, eVec.x, eVec.y, eVec.z);
*/
	Vector3 eVec(eye.x, eye.y, eye.z); // a vector version of eye 
	float cs = cos(3.14159265/180 * angle);
	float sn = sin(3.14159265/180 * angle);
	float r[16];
	r[0] =  (1-cs) * (eVec.x * eVec.x) + cs;
	r[4] =  (1-cs) * (eVec.x * eVec.y) + (sn * eVec.z);
	r[8] =  (1-cs) * (eVec.x * eVec.z)  - (sn * eVec.y);
	r[12] = 0.0f;
	r[1] =  (1-cs) * (eVec.x * eVec.y) + (sn * eVec.z);
	r[5] =  (1-cs) * (eVec.y * eVec.y) + cs;
	r[9] =  (1-cs) * (eVec.y * eVec.z) + (sn * eVec.x);
	r[13] = 0.0f;
	r[2] =  (1-cs) * (eVec.x * eVec.z) + (sn * eVec.y);
	r[6] =  (1-cs) * (eVec.y * eVec.z) - (sn * eVec.x);
	r[10] =  (1-cs) * (eVec.z * eVec.z) + cs;
	r[14] = 0.0f;
	r[3] = 0.0f;
	r[7] = 0.0f;
	r[11] = 0.0f;
	r[15] = 1.0f;
	glMatrixMode(GL_MODELVIEW);
	glMultMatrixf(r);             // postmultiply it by r

}

void Camera::setShape(float vAng, float asp, float nearD, float farD)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(vAng,asp,nearD,farD);
}

/*
void Camera:: setOblique(Vector3 d,..others..)
{ // establish camera for oblique projections
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(l,r,b,t,n,f);         // set the projection matrix
	if(d.z == 0.0) return;		// for orthographic projections
	float m[16]; // space for a 4 by 4 matrix
	for(int i = 0; i < 16; i++)   // start with identity matrix
	    m[i] = (i%5 == 0)? 1.0 : 0.0;// identity matrix
	m[8] = -d.x/d.z;              // add the shear terms
	m[9] = -d.y/d.z;
	glMultMatrixf(m);             // postmultiply it by m
}
*/
