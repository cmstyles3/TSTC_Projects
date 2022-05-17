/* LAB10A
// Collin Munson
// -------------------------------------
// CONTROLS
/ 'w' or 'W'  - move ship forward
  's' or 'S'  - move ship backward
  'a' or 'A'  - yaw ship left
  'd' or 'D'  - yaw ship right
  'q' or 'Q'  - move ship left
  'e' or 'E'  - move ship right
		 '1'  - move ship up
		 '2'  - move ship down
  left arrow  - roll ship left
  right arrow - roll ship right
  up arrow    - pitch ship up
  down arrow  - pitch ship down
  'r' or 'R'  - reset ship's position
*/

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <gl/glut.h>
#include <iostream>
#include <vector>
#include "bitmap.h"

GLfloat jetRotation[16];
GLfloat x, y, z;

struct JetInfo
{
	GLfloat jetRotation[16];
	void Set(GLfloat jetRotation[16])
	{
		memcpy(this->jetRotation, jetRotation, sizeof(GLfloat) * 16);
	}
};

struct VaporTrailInfo
{
	GLfloat vaporTrailRot[16];
	void Set(GLfloat vaporTrailRot[16])
	{
		memcpy(this->vaporTrailRot, vaporTrailRot, sizeof(GLfloat) * 16);
	}
};

std::vector<JetInfo> jetInfo;
std::vector<VaporTrailInfo> vaporTrailInfo;


GLuint dlWing, dlBox, dlCone, dlJet;
GLuint surfaceTexture;

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
	glClearColor(0.16, 0.65, 0.9, 1.0);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 800.0/600.0, 1, 1000);
	glViewport(0, 0, 800, 600);
	
	glEnable(GL_DEPTH_TEST);

	MakeDisplayList();

	BitmapFile bmf;
	bmf.load("sand512.bmp");
	glGenTextures(1, &surfaceTexture);
	glBindTexture(GL_TEXTURE_2D, surfaceTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmf.w, bmf.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmf.rgba_data);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 512, 512, GL_RGBA, GL_UNSIGNED_BYTE, bmf.rgba_data);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA);
	glEnable(GL_ALPHA_TEST);

	JetInfo jetinfo;
	VaporTrailInfo vaporTrailinfo;
	jetinfo.Set(jetRotation);
	vaporTrailinfo.Set(jetRotation);
	for (int i = 0; i < 20; i++)
	{
		jetInfo.push_back(jetinfo);
		vaporTrailInfo.push_back(vaporTrailinfo);
	}
}

void Render(int i, GLfloat a)
{
	glColor4f(1.0, 1.0, 1.0, a);	//green

	glPushMatrix();
	glMultMatrixf(vaporTrailInfo[i].vaporTrailRot);
	glTranslatef(0.0f, 0.0f,-8.0f);
	glutSolidSphere(1.0f, 10, 10);
	glPopMatrix();
	
	glPushMatrix();
	glMultMatrixf(jetInfo[i].jetRotation);
	glCallList(dlJet);
	glPopMatrix();

	//glLoadIdentity();
	
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -50.0f);

	glBindTexture(GL_TEXTURE_2D, surfaceTexture);
	
	glColor4f(1.0, 1.0, 1.0, 0.1);	//green
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-5000.0f, -200.0f, -10000.0f);

		glTexCoord2f(100.0f, 0.0f);
		glVertex3f(500.0f, -200.0f, -10000.0f);

		glTexCoord2f(100.0f, 100.0f);
		glVertex3f(5000.0f, -200.0f, 0.0f);

		glTexCoord2f(0.0f, 100.0f);
		glVertex3f(-5000.0f, -200.0f, 0.0f);
	glEnd();

	for(int i = 0; i < 20; i++)
	{
		Render(i, 1.0 - GLfloat(i / 20.0));
		glAccum(GL_ACCUM, 1.f/20);
	} 
	glAccum(GL_RETURN, 1.0f);

	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'w':
		case 'W':
			{
			glPushMatrix();
			glLoadMatrixf(jetRotation);
			glTranslatef(0.0f, 0.0f, 2.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
			glPopMatrix();
			}
			break;
		case 's':
		case 'S':
			{
			glPushMatrix();
			glLoadMatrixf(jetRotation);
			glTranslatef(0.0f, 0.0f, -2.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
			glPopMatrix();
			}
			break;
		case 'a':
		case 'A':
			{
			glPushMatrix();
			glLoadMatrixf(jetRotation);
			glRotatef(3.0f, 0.0f, 1.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
			glPopMatrix();
			}break;

		case 'd':
		case 'D':
			{
			glPushMatrix();
			glLoadMatrixf(jetRotation);
			glRotatef(-3.0f, 0.0f, 1.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
			glPopMatrix();
			}break;

		case 'q':
		case 'Q':
			{
				glPushMatrix();
				glLoadMatrixf(jetRotation);
				glTranslatef(-2.0f, 0.0f, 0.0f);
				glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
				glPopMatrix();
				
			}break;
		case 'e':
		case 'E':
			{
				glPushMatrix();
				glLoadMatrixf(jetRotation);
				glTranslatef(2.0f, 0.0f, 0.0f);
				glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
				glPopMatrix();
			}break;
		case '1':
			{
				glPushMatrix();
				glLoadMatrixf(jetRotation);
				glTranslatef(0.0f, 2.0f, 0.0f);
				glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
				glPopMatrix();
			}break;
		case '2':
			{
				glPushMatrix();
				glLoadMatrixf(jetRotation);
				glTranslatef(0.0f, -2.0f, 0.0f);
				glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
				glPopMatrix();
			}break;
		case 'r':
		case 'R':
			{
				glPushMatrix();
				glLoadIdentity();
				glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
				glPopMatrix();
			}break;

		case 27:
			exit(0);
			break;

	}
	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			{
				glPushMatrix();
				glLoadMatrixf(jetRotation);
				glRotatef(-3.0f, 1.0f, 0.0f, 0.0f);
				glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
				glPopMatrix();
	
			}break;

		case GLUT_KEY_DOWN:
			{
				glPushMatrix();
				glLoadMatrixf(jetRotation);
				glRotatef(3.0f, 1.0f, 0.0f, 0.0f);
				glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
				glPopMatrix();
			}break;

		case GLUT_KEY_LEFT:
			{
				glPushMatrix();
				glLoadMatrixf(jetRotation);
				glRotatef(-10.0f, 0.0f, 0.0f, 1.0f);
				glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
				glPopMatrix();
			}break;
		case GLUT_KEY_RIGHT:
			{
				glPushMatrix();
				glLoadMatrixf(jetRotation);
				glRotatef(10.0f, 0.0f, 0.0f, 1.0f);
				glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
				glPopMatrix();
			}break;


	}
	glutPostRedisplay();
}

void Idle()
{
	static DWORD timer = 0;
	if (GetTickCount() - timer > 20)
	{
		for (int i = 19; i > 0; i--)
			jetInfo[i].Set(jetInfo[i-1].jetRotation);
		jetInfo[0].Set(jetRotation);

		timer = GetTickCount();

		
	}

	static DWORD vaporTimer = 0;
	if (GetTickCount() - vaporTimer > 150)
	{
		for (int i = 19; i > 0; i--)
			vaporTrailInfo[i].Set(vaporTrailInfo[i-1].vaporTrailRot);
		vaporTrailInfo[0].Set(jetRotation);
		vaporTimer = GetTickCount();
	}
	glutPostRedisplay();
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	glutInitWindowPosition(width/2-400,height/2-300);
	glutCreateWindow("Lab10a - Motion Blur");

	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutIdleFunc(Idle);

	Init();
	
	glutMainLoop();

}
