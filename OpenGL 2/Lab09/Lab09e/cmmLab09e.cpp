// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// cmmLab09e.cpp   Texture Animation 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include <windows.h>  
#include <gl/Gl.h>
#include <gl/Glu.h>
#include <gl/glut.h>
#include <math.h>
#include <iostream>
#include "RGBApixmap.h"
#include "camera/camera6.h"
#include "FloatPoint.h"

using namespace std;

Camera cam1; 				// global camera object(s)
int cam = 1;

int   SCREEN_WIDTH  = 800;
int   SCREEN_HEIGHT = 600;
const int   WORLD_SIZE = 200;
const float PI = 3.14159f;

RGBApixmap pic[12]; 			// create two (empty) global pixmaps
mRGB p;

GLubyte image[12][128][128][4];	

static GLuint texName[12];

int currframe = 0;


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// convert a bitmap image read from a file into a texture array
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void convertPixels(GLubyte image[128][128][4], int rows, int cols, int whichPic)
{
	for(int i = 0; i < rows; i++)
	{
		for(int j = 0; j < cols; j++)
		{
//			p = pic[whichPic].getPixela(i, j);			// line 42
			p = pic[whichPic].getPixel(i, j);			// line 42
			image[i][j][0] = p.r;
			image[i][j][1] = p.g;
			image[i][j][2] = p.b;
			// if the pixel from the bitmap is black, set alpha to zero
			// otherwise, set alpha to 255
			if(p.r == 0 && p.g == 0 && p.b == 0)		// line 46
				image[i][j][3] = 0;
			else
				image[i][j][3] = 255;
		}
	}	
}

void drawPolygon()
{
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-1.0, -1.0, 0.0);

		glTexCoord2f(0.0, 0.0);
		glVertex3f(-1.0, 1.0, 0.0);

		glTexCoord2f(1.0, 0.0);
		glVertex3f(1.0, 1.0, 0.0);

		glTexCoord2f(1.0, 1.0);
		glVertex3f(1.0, -1.0, 0.0);
	glEnd();
}

void drawObjects()
{
	// enable texturing for objects
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texName[currframe]);
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
	
	glTranslatef(0.0f, 0.0f, -1.5f);
	glRotatef(90, 0.0, 0.0, 1.0);
	
	drawPolygon();
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
}

void myInit()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 12; i++)
	{
		char buffer[100];
		sprintf(buffer, "image%i\.bmp", i+1);
		pic[i].readBMPFile(buffer);
		convertPixels(image[i], 128, 128, i);
		glGenTextures(1, &texName[i]);
		glBindTexture(GL_TEXTURE_2D, texName[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 128, 128, 0,
			 GL_RGBA,GL_UNSIGNED_BYTE, image[i]);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 128, 128, GL_RGBA, GL_UNSIGNED_BYTE, image[i]);

	}
	// set object positions
	srand(GetTickCount());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)-WORLD_SIZE, (GLdouble)WORLD_SIZE, (GLdouble)-WORLD_SIZE, (GLdouble)WORLD_SIZE, (GLdouble)-WORLD_SIZE, (GLdouble)WORLD_SIZE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
	
	drawObjects();

	glutSwapBuffers(); 
}


void myKeyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	// controls for camera
	case 27:
		exit(-1);
		break;
	}
	
	glutPostRedisplay();
}

void myReshape(int w, int h)
{
	SCREEN_WIDTH = w;
	SCREEN_HEIGHT = h;
 	glViewport(0, 0, (GLsizei)w, (GLsizei)h) ;
	glMatrixMode(GL_PROJECTION) ;
	glLoadIdentity() ;
	gluPerspective(65.0, (GLfloat)w/(GLfloat)h, 0.1, 200.0) ;
	glMatrixMode(GL_MODELVIEW) ;
	glLoadIdentity() ;
  	glTranslatef(0.0, 0.0, -5.0) ;
}

void idleFunc()
{
	cam1.setModelViewMatrix();
	myDisplay();
	
	glutPostRedisplay();

	currframe++;
	if (currframe >= 12) currframe = 0;
	Sleep(200);

}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Lab09e - Texture Animation");
	glutKeyboardFunc(myKeyboard);
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutIdleFunc(idleFunc);
	
	myInit();
	glViewport(0, 0, 800, 600);
	cam1.set(Point3(0, 0, 0), Point3(0, 0, -1), Vector3(0, 1, 0)); 	// make the initial camera
	cam1.setShape(30.0f, 64.0f/48.0f, 0.1f, 30.0f);
	
	glutMainLoop();
}