#ifndef CANNON_H
#define CANNON_H

#include <gl/glut.h>

GLuint dlWheel;
GLuint dlAxis;
GLuint dlCannonBase;

GLuint dlCannon;

void InitCannonDisplayList()
{
	dlWheel = glGenLists(3);
	dlAxis = dlWheel + 1;
	dlCannonBase = dlWheel + 2;
	GLUquadric *quadric = gluNewQuadric();

	glNewList(dlWheel, GL_COMPILE);
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		gluCylinder(quadric, 10.0, 10.0, 2.0, 20, 1);
		//glutSolidCone(10.0, 10.0, 10, 10);
		glColor3f(0.6f, 0.6f, 0.6f);
		gluDisk(quadric, 8.0f, 10.0f, 20, 1);
		glTranslatef(0.0f, 0.0f, 2.0f);
		//glColor3f(0.0f, 0.0f, 0.6f);
		gluDisk(quadric, 8.0f, 10.0f, 20, 1);

		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -2.0f);
		glColor3f(0.2f, 0.2f, 0.2f);
		gluDisk(quadric, 0.0f, 2.2f, 10, 1);
		glPopMatrix();

		glColor3f(0.3f, 0.3f, 0.3f);
		glTranslatef(0.0f, 9.0f, -1.0f);
		glRotatef(90, 1, 0, 0);
		gluCylinder(quadric, 1.0f, 1.0f, 18.0f, 10, 1);
		glRotatef(90, 0, 1, 0);
		glTranslatef(-9.0f, 0.0f, -9.0f);
		gluCylinder(quadric, 1.0f, 1.0f, 18.0f, 10, 1);
	}glEndList();

	glNewList(dlAxis, GL_COMPILE);
	{
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0.0f, -10.0f);
		
		gluCylinder(quadric, 2.0, 2.0, 20, 10, 1);
	}glEndList();

	glNewList(dlCannonBase, GL_COMPILE);
	{
		glColor3f(0.45f, 0.45f, 0.45f);
		glPushMatrix();
		
		glCallList(dlAxis);
		glPopMatrix();

		glColor3f(1.0f, 0.0f, 0.0f);
		glPushMatrix();
		
		glTranslatef(10.0f, 0.0f, 0.0f);
		glRotatef(-90, 0, 1, 0);
		glCallList(dlWheel);
		glPopMatrix();

		glPushMatrix();
		
		glTranslatef(-10.0f, 0.0f, 0.0f);
		glRotatef(90, 0, 1, 0);
		glCallList(dlWheel);
		glPopMatrix();
	}glEndList();

	dlCannon = glGenLists(1);
	glNewList(dlCannon, GL_COMPILE);
	{
		glColor3f(0.38f, 0.38f, 0.38f);
		glTranslatef(0.0, 1.0f, 0.0f);
		glutSolidCube(10.0f);
		glColor3f(0.4f, 0.4f, 0.4f);
		glTranslatef(0.0f, 3.0f, -4.0f);
		gluCylinder(quadric, 3.0f, 3.0f, 25.0f, 10, 1);
		glColor3f(0.1f, 0.1f, 0.1f);
		gluCylinder(quadric, 2.8f, 2.8f, 25.0f, 10, 1);
	
		glColor3f(0.4f, 0.4f, 0.4f);
		gluDisk(quadric, 0.0f, 3.0f, 10, 1);

		glTranslatef(0.0f, 0.0f, 12.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		gluDisk(quadric, 0.0f, 2.8f, 10, 1);
	}glEndList();

	gluDeleteQuadric(quadric);
}

#endif