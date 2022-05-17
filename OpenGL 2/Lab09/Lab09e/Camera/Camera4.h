#include <math.h>
#include "Point3.h"
#include "Vector3.h"
class Camera{
  private:
	Point3 eye;
	Vector3 u, v, n, up;
	double pitchangle, rotangle;
	double viewAngle, aspect, nearDist, farDist; 		// view volume shape
	void setModelviewMatrix(); 			// tell OpenGL where the camera is

  public:
	Camera(); 					// default constructor
	Point3 look;
	void setModelViewMatrix();
	void set(Point3 eye, Point3 look, Vector3 up); 	// like gluLookAt()
	void roll(float angle); 			// roll it
	void pitch(float angle); 			// increase pitch
	void yaw(float angle); 				// yaw it
	void slide(float delU, float delV, float delN); // slide it
	void Slide(float sU,float sV,float sN);
//	void rotate (Vector3 axis, float angle);
	void rotate (float angle);
	void rotate2 (float amt);
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
	look.set(lookset.x, lookset.y, lookset.z);
	up.set(upset);
	n.set(eyeset.x - lookset.x, eyeset.y - lookset.y, eyeset.z - lookset.z); // make n
	u.set(upset.cross(n)); 		// make u = up X n
	n.normalize(); u.normalize(); 	// make them unit length
	v.set(n.cross(u));  		// make v =  n X u
	pitchangle = atan((eye.z - look.z) / (eye.y - look.y)) * (180.0 / 3.14159265);
	rotangle = 0.0;
	setModelViewMatrix(); 		// tell OpenGL 
}

void Camera::slide(float delU,float delV,float delN)
{
	eye.x += delU * u.x + delV * v.x + delN * n.x;
	eye.y += delU * u.y + delV * v.y + delN * n.y;
	eye.z += delU * u.z + delV * v.z + delN * n.z;
	setModelViewMatrix();
}

void Camera::Slide(float sU,float sV,float sN)
{
	eye.x += sU;
	eye.y += sV;
	eye.z += sN;
	setModelViewMatrix();
}

void Camera :: roll(float angle)
{ // roll the camera through angle degrees around n
	float cs = cos(3.14159265/180 * angle);
	float sn = sin(3.14159265/180 * angle);
	Vector3 t(u); // remember old u
	u.set(cs*t.x - sn*v.x, cs*t.y - sn*v.y, cs*t.z - sn*v.z);
	v.set(sn*t.x + cs*v.x, sn*t.y + cs*v.y, sn*t.z + cs*v.z);
	setModelViewMatrix();
}

void Camera :: pitch (float amt)
{
	eye.y += amt;
	setModelViewMatrix();
}

void Camera :: yaw (float angle)
{ // yaw the camera through angle degrees around V
	float cs = cos(3.14159265/180 * angle);
	float sn = sin(3.14159265/180 * angle);
	Vector3 t(n); // remember old n
	n.set(cs*t.x - sn*u.x, cs*t.y - sn*u.y, cs*t.z - sn*u.z);
	u.set(sn*t.x + cs*u.x, sn*t.y + cs*u.y, sn*t.z + cs*u.z);
	v = n.cross(u);				// added for yaw problem, 8-15-04
	setModelViewMatrix();
}

void Camera :: rotate2 (float amt)
{
	eye.x += amt;
	eye.z += amt;
	setModelViewMatrix();
}

// void Camera :: rotate (Vector3 axis, float angle)
void Camera :: rotate (float angle)
{ // rotate the camera around its own axis using yaw with adjustment for pitch
	float cs = cos(3.14159265/180 * angle);
	float sn = sin(3.14159265/180 * angle);
	float radius = sqrt(pow(eye.x - look.x, 2.0) + pow(eye.z - look.z, 2.0));
	float x1, x2, z1, z2, a;
	x1 = radius * cos(3.14159265/180 * 0.0);
	z1 = radius * sin(3.14159265/180 * 0.0);
	x2 = radius * cos(3.14159265/180 * angle);
	z2 = radius * sin(3.14159265/180 * angle);
	look.x = look.x + x2 - x1;
	look.z = look.z + z2 - z1;
	a = sqrt(pow(double(x2 - x1), 2.0) + pow(double(z2 - z1), 2.0));

	Vector3 oldn(n); // remember old n
	Vector3 oldu(u); // remember old u
	Vector3 oldv(v); // remember old v

	n.set(eye.x - look.x, eye.y - look.y, eye.z - look.z);
	u.set(sn * oldn.x + cs * oldu.x, sn * oldn.y + cs * oldu.y, sn * oldn.z + cs * oldu.z);
//	u.set(sn * oldn.x + cs * oldu.x, oldu.y, sn * oldn.z + cs * oldu.z);
//	n.x = eye.x - look.x;
//	n.y = eye.y - look.y;
//	n.z = eye.z - look.z;

	setModelViewMatrix();
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
