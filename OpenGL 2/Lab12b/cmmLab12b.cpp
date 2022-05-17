/* Instructions

The aircraft will automatically move from left to right and
will loop back to the left when it runs off the screen.
Press space bar to drop bowling ball from aircraft.
*/

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl/glut.h>

#include "bitmap.h"

float ctrlpoints[4][4][3] = {
		 { { -50.0,  0.0, -30.0 },
		    { -10.0,  0.0, -30.0 },
		    {  10.0,  0.0, -30.0 },
		    {  50.0,  0.0, -30.0 } },
		 { { -50.0,  0.0, -10.0 },
		    { -10.0,  500.0, -10.0 },
		    {  10.0,  0.0, -10.0 },
		    {  50.0,  0.0, -10.0 } },
		 { { -50.0,  0.0,  10.0 },
		    { -10.0,  0.0,  10.0 },
		    {  10.0,  0.0,  10.0 },
		    {  50.0,  0.0,  10.0 } },
		 { { -50.0,  0.0,  30.0 },
		    { -10.0,  0.0,  30.0 },
		    {  10.0,  0.0,  30.0 },
		    {  50.0,  0.0,  30.0 } },
};

GLfloat texpts[2][2][2] = {
			{ { 0.0, 0.0 }, { 0.0, 10.0 } },
			{ { 10.0, 0.0 }, { 10.0, 10.0 } }
};


GLfloat jetRotation[16];

GLuint dlWing, dlBox, dlCone, dlJet;
GLuint surfaceTexture, bkTexture;

GLfloat xPos = -120.0f;
GLfloat ballXPos = -100.0f, ballYPos = 150.0f;
GLfloat ballXVel = 100.0f;

bool ballDropped = false;
bool changeColor = false;


void MakeDisplayList()
{

	dlWing = glGenLists(4);
	dlBox = dlWing + 1;
	dlCone = dlWing + 2;
	dlJet = dlWing + 3;

	float PI2 = 2.0 * 3.141592654;

	//build trig lists so we don't need to calc every time
	glNewList(dlCone, GL_COMPILE);
	{
		glBegin(GL_TRIANGLE_FAN);
		{
			glVertex3f(0, 0, 5);
			for(float x = PI2, i = 0; i <=20; x -= PI2/20.0f, i++)
			{
				glVertex3f(1.5*sin(x), 1.5*cos(x), 0);
			}

		} glEnd();

		glBegin(GL_POLYGON);
		{
			for(float x = PI2, i = 0; i <=20; x -= PI2/20.0f, i++)
			{
				glVertex3f(1.5*sin(x), 1.5*cos(x), 0);
			}

		} glEnd();
			

	} glEndList();

	glNewList(dlWing, GL_COMPILE);
	{
		glBegin(GL_TRIANGLE_STRIP);
		{
			glVertex3f(0, 3, 0);
			glVertex3f(0, -1, 5);
			glVertex3f(-0.5, -3, 0);
			glVertex3f(0, -3, 5);
			glVertex3f(0.5, -3, 0);
			glVertex3f(0, -1, 5);
			glVertex3f(0, 3, 0);
			

		} glEnd();
			

	} glEndList();

	glNewList(dlBox, GL_COMPILE);
	{
		glBegin(GL_QUAD_STRIP);
		{
			glVertex3f(2.0, 2.0, 3.0);
			glVertex3f(-2.0, 2.0, 3.0);
			glVertex3f(2.0, -2.0, 3.0);
			glVertex3f(-2.0, -2.0, 3.0);
			glVertex3f(2.0, -2.0, -3.0);
			glVertex3f(-2.0, -2.0, -3.0);
			glVertex3f(2.0, 2.0, -3.0);
			glVertex3f(-2.0, 2.0, -3.0);
			glVertex3f(2.0, 2.0, 3.0);
			glVertex3f(-2.0, 2.0, 3.0);
		} glEnd();

		glBegin(GL_QUADS);
		{
			glVertex3f(2.0, 2.0, 3.0);
			glVertex3f(2.0, -2.0, 3.0);
			glVertex3f(2.0, -2.0, -3.0);
			glVertex3f(2.0, 2.0, -3.0);
			glVertex3f(-2.0, 2.0, 3.0);
			glVertex3f(-2.0, -2.0, 3.0);
			glVertex3f(-2.0, -2.0, -3.0);
			glVertex3f(-2.0, 2.0, -3.0);

		} glEnd();
			

	} glEndList();

	GLfloat lt_grey[4] = {0.6, 0.6, 0.6, 1.0};
	GLfloat lt_grey2[4] = {0.6, 0.6, 0.85, 1.0};
	GLfloat dk_grey[4] = {0.2, 0.2, 0.2, 1.0};
	GLfloat lt_blue[4] = {109.0f/255.0f, 207.0f/255.0f, 246.0f/255.0f, 1.0};
	GLfloat flame[4] = {1.0, 0.5, 0.0, 1.0};
	
	glNewList(dlJet, GL_COMPILE);
	{
		
		glPushAttrib(GL_CURRENT_BIT);
		glPushMatrix();
		glColor4fv(lt_grey);

		//draw body of jet
		glScalef(0.8, 0.8, 3.0);
		glTranslatef(-3, 0, 0);
		glCallList(dlBox);
		glTranslatef(6, 0, 0);
		glCallList(dlBox);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, 1.5);
		glColor3fv(lt_grey2);
		glScalef(1, 1, 3.5);
		glCallList(dlBox);
		
		//cockpit
		glColor3fv(lt_grey);
		glTranslatef(0, 0, 3);
		glScalef(1, 1, 0.8);
		glCallList(dlCone);
		//glass
		glColor4fv(lt_blue);
		glTranslatef(0, 1.0, 1.3);
		glScalef(0.7, 0.7, 0.3);
		glRotatef(10, 1, 0, 0);
		glCallList(dlCone);
		glRotatef(180, 1, 0, 0);
		glCallList(dlCone);

		//main wings
		glPopMatrix();
		glPushMatrix();
		glColor4fv(lt_grey2);
		glScalef(2.5, 2.5, 1.5);
		glTranslatef(1.5, 0, 3.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(90, 0, 1, 0);
		glCallList(dlWing);
		glRotatef(180, 0, 1, 0);
		glTranslatef(0, 0, 3);
		glCallList(dlWing);

		//horizontal stabilizer
		glTranslatef(0, -7.5, 0);
		glScalef(0.6, 0.6, 0.6);
		glCallList(dlWing);
		glRotatef(180, 0, 1, 0);
		glTranslatef(0, 0, 5);
		glCallList(dlWing);

		//vertical stabilizer
		glPushMatrix();
		glTranslatef(0.5, 1, -0.7);
		glRotatef(75, 0, 1, 0);
		glCallList(dlWing);
		glPopMatrix();
		glTranslatef(0.5, 1, -4.3);
		glRotatef(105, 0, 1, 0);
		glCallList(dlWing);
		
		//Engines
		glPopMatrix();
		glPushMatrix();
		glColor4fv(flame);
		//glScalef(4, 4, 4);
		glTranslatef(-2.5, 0, -10);
		glCallList(dlCone);
		glTranslatef(5, 0, 0);
		glCallList(dlCone);


		//restore
		glPopMatrix();
		glPopAttrib();

	} glEndList();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
	glPopMatrix();
}

void Init()
{
	MakeDisplayList();
	BitmapFile bmf;
	
	

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100.0f, 100.0f, -100.0f, 100.0f, 0.0f, 1000.0f);
	//gluPerspective(60, 800.0/600.0, 1.0f, 1000.0f);
	gluLookAt(0.0, 100.0, 100.0, 0.0, 100.0, 0.0, 0.0, 1.0, 0.0);

	// define the Bezier surface points and texture
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &ctrlpoints[0][0][0]);
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2, &texpts[0][0][0]);
	glEnable(GL_MAP2_TEXTURE_COORD_2);
	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	bmf.load("grass.bmp");
	glGenTextures(1, &surfaceTexture);
	glBindTexture(GL_TEXTURE_2D, surfaceTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmf.w, bmf.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmf.rgba_data);
	bmf.load("bk.bmp");
	glGenTextures(1, &bkTexture);
	glBindTexture(GL_TEXTURE_2D, bkTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmf.w, bmf.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmf.rgba_data);
	

}

void Keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case ' ':
			if (!ballDropped)
			{
				ballDropped = true;
				ballXPos = xPos;
				ballYPos = 180.0f;
				ballXVel = 50.0f;
			}
			break;

	}
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, bkTexture);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-100.0f, 200.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(100.0f, 200.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(100.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
	}
	glEnd();

	glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	
	glBindTexture(GL_TEXTURE_2D, surfaceTexture);
	glTranslatef(-50.0f, 0.0f, 0.0f);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	
	glTranslatef(100.0f, 0.0f, 0.0f);
	glRotatef(180, 0, 1, 0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);

	GLUquadric *obj = gluNewQuadric();
	glPushMatrix();
	//glTranslatef(0.0f, 10.0f, 0.0f);
	glRotatef(-90, 1, 0, 0);
	if (changeColor)
		glColor3f(1.0f, 0.0f, 1.0f);
	else glColor3f(0.0f, 0.0f, 1.0f);
	gluCylinder(obj, 5.0f, 5.0f, 25.0f, 10, 1);
	glPopMatrix();

	gluDeleteQuadric(obj);

	//glMultMatrixf(jetRotation);
	glPushMatrix();
	glTranslatef(xPos, 180.0f, 0.0f);
	glRotatef(90, 0, 1, 0);
	glCallList(dlJet);
	glPopMatrix();

	if (ballDropped)
	{
		glPushMatrix();
		glTranslatef(ballXPos, ballYPos, 0.0f);
		glColor3f(0.0f, 0.0f, 0.0f);
		glutSolidSphere(2.0, 20, 20);
		glPopMatrix();
	}

	
	glutSwapBuffers();
	
}

void Idle()
{
	static float lastTime = (float)timeGetTime();
	float currTime = (float)timeGetTime();
	float dt = (currTime - lastTime)*0.001f;

	xPos += 50.0f * dt;
	if (xPos >= 120) xPos = -120.0f;

	if (ballDropped)
	{
		ballXVel -= (ballXVel * 0.10) * dt;
			
		if (ballXVel < 0) ballXVel = 0;

		ballXPos += ballXVel * dt;
		ballYPos -= 100.0f * dt;
		if (ballYPos < -100) ballDropped = false;

		if (abs(ballYPos - 12.5f) < 13.5f && abs(ballXPos - 0.0f) < 7.0f)
		{
			changeColor = !changeColor;
			ballDropped = false;
		}
	}

	Display();

	lastTime = currTime;
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Lab12b - Evaluators and NURBS");
	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutKeyboardFunc(Keyboard);

	Init();
	glutMainLoop();

}


