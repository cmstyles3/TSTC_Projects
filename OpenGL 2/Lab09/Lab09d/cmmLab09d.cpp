// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// cmmLab09d.cpp   Billboarding 
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

RGBApixmap pic[2]; 			// create two (empty) global pixmaps
mRGB p;

GLubyte image1[512][512][4];		// for cactus.bmp
GLubyte image2[512][512][4]; // for sand512.bmp
static GLuint texName, texName2;

const int NUM_OBJECTS = 100;
FloatPoint objpos[NUM_OBJECTS];

bool firstFrame = true;


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// convert a bitmap image read from a file into a texture array
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void convertPixels(GLubyte image[512][512][4], int rows, int cols, int whichPic)
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
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-2.0, 0.0, 0.0);

		glTexCoord2f(0.0, 1.0);
		glVertex3f(2.0, 0.0, 0.0);

		glTexCoord2f(1.0, 1.0);
		glVertex3f(2.0, 4.0, 0.0);

		glTexCoord2f(1.0, 0.0);
		glVertex3f(-2.0, 4.0, 0.0);
	glEnd();
}

void drawObjects()
{
	// enable texturing for objects
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texName);
	// make sure the transparent part of the texture isn't drawn
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_FLAT);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);
	glColor4f(1.0, 1.0, 1.0, 0.05);
	
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		float sidex = objpos[i].x - cam1.eye.x;
		float sidez = objpos[i].z - cam1.eye.z;
		double radians = atan(sidex / sidez);
		double angle = radians *(180 / PI);
		glPushMatrix();
			glTranslatef(objpos[i].x, objpos[i].y, objpos[i].z);
			glRotatef(angle, 0.0, 1.0, 0.0);
			drawPolygon();
		glPopMatrix();
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA);
	glDisable(GL_BLEND);
}

void drawSurface()
{
	glPushMatrix();
	glColor4f(0.5, 1.0, 0.5, 1.0);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texName2);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3i(-WORLD_SIZE, 0, -WORLD_SIZE);
	glTexCoord2f(100.0f, 0.0f);
	glVertex3i( WORLD_SIZE, 0, -WORLD_SIZE);
	glTexCoord2f(100.0f, 100.0f);
	glVertex3i( WORLD_SIZE, 0,  WORLD_SIZE);
	glTexCoord2f(0.0f, 100.0f);
	glVertex3i(-WORLD_SIZE, 0,  WORLD_SIZE);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void myInit()
{
	glClearColor(0.4f, 0.76f, 0.95f, 0.0f);

	// set up texture for objects
	pic[0].readBMPFile("cactus.bmp"); 		// 512 x 512
	convertPixels(image1, 512, 512, 0);			// convert a bmp to a texture array
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0,
			 GL_RGBA,GL_UNSIGNED_BYTE, image1);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 512, 512, GL_RGBA, GL_UNSIGNED_BYTE, image1);

	// set up texture for objects
	pic[1].readBMPFile("sand512.bmp"); 		// 512 x 512
	convertPixels(image2, 512, 512, 1);			// convert a bmp to a texture array
	glGenTextures(1, &texName2);
	glBindTexture(GL_TEXTURE_2D, texName2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 512, 512, 0,
			 GL_RGBA,GL_UNSIGNED_BYTE, image2);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 512, 512, GL_RGBA, GL_UNSIGNED_BYTE, image2);

	// set object positions
	srand(GetTickCount());
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		objpos[i].x = (float)((rand()%(WORLD_SIZE*2)) - (WORLD_SIZE));
		objpos[i].y = 0.0;
		objpos[i].z = (float)((rand()%(WORLD_SIZE*2)) - (WORLD_SIZE));
	}

	// adjust position of one object for easier testing
	objpos[0].x = 0.0;
	objpos[0].y = 0.0;
	objpos[0].z = 0.0;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)-WORLD_SIZE, (GLdouble)WORLD_SIZE, (GLdouble)-WORLD_SIZE, (GLdouble)WORLD_SIZE, (GLdouble)-WORLD_SIZE, (GLdouble)WORLD_SIZE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	
	
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	

	drawSurface();
	
	drawObjects();

	glutSwapBuffers(); 
}

void mySpecial(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_UP:
			glClear(GL_COLOR_BUFFER_BIT);
			cam1.slide(0,0, -2.0);
			break;
		case GLUT_KEY_DOWN:
			glClear(GL_COLOR_BUFFER_BIT);	
			cam1.slide(0,0,2.0);
			break;
	    case GLUT_KEY_LEFT:
			glClear(GL_COLOR_BUFFER_BIT);	
			cam1.yaw(-2.0);
			break;
	    case GLUT_KEY_RIGHT:
			glClear(GL_COLOR_BUFFER_BIT);	
			cam1.yaw(2.0);
			break;

	      default:
		      break;
	}
	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	// controls for camera
	case 27:
		exit(-1);
		break;
	case 'F': 				// slide camera forward
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.slide(0,0, 0.2);
		break;
	case 'f':				//slide camera back	
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.slide(0,0,-0.2);
		break;
	case 'G': 				// slide camera to the right
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.slide(0.2,0, 0);
		break;
	case 'g':				//slide camera to the left	
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.slide(-0.2,0,0);
		break;
	case 'H': 				// slide camera up
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.slide(0, 0.2, 0);
		break;
	case 'h': 				// slide camera down
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.slide(0, -0.2, 0);
		break;	
	// add up/down and left/right controls	
	case 'P':
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.pitch(-1.0);
		break;
	case 'p':
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.pitch(1.0);
		break;
	// add roll and yaw controls
	case 'R':
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.roll(-1.0);
		break;
	case 'r':
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.roll( 1.0);
		break;
	case 'Y':
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.yaw(-1.0);
		break;
	case 'y':
		glClear(GL_COLOR_BUFFER_BIT);	
		cam1.yaw( 1.0);
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

}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Lab09d - Billboarding");
	glutKeyboardFunc(myKeyboard);
	glutDisplayFunc(myDisplay);
	glutSpecialFunc(mySpecial);
	glutReshapeFunc(myReshape);
	glutIdleFunc(idleFunc);
	
	myInit();
	glViewport(0, 0, 800, 600);
	cam1.set(Point3(0, 1, 0), Point3(0, 1, -1), Vector3(0, 1, 0)); 	// make the initial camera
	cam1.setShape(30.0f, 64.0f/48.0f, 0.1f, 300.0f);
	
	glutMainLoop();
}