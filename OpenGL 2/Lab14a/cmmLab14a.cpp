
/* 
INSTRUCTIONS
The camera for this program is an orbital type camera.
You can move the target x, y, z and the distance from that point.
You can also set the camera  to target the aircraft, which is on by default.

// CONTROLS
	Escape key - Exits program
	'w' || 'W' - move ship forward
	's' || 'S' - move ship backward
	'q' || 'Q' - ship pitch down
	'e' || 'E' - ship pitch up
	'r' || 'R' - reset ships position
	'f' || 'F' - toggle for follow ship

	       '1' - move camera closer to target point
	       '2' - move camera farther from target point
		   'x' - move camera target point in the positive x direction
		   'X' - move camera target point in the negative x direction
		   'y' - move camera target point in the positive y direction
		   'Y' - move camera target point in the negative y direction
		   'z' - move camera target point in the positive z direction
		   'Z' - move camera target point in the negative z direction
	
	left arrow  - rotate camera left around target
	right arrow - rotate camera right around target
	up arrow    - rotate camera up around target
	down arrow  - rotate camera down around target

/*/


#include <windows.h>
#include "bitmap.h"
#include "aircraft.h"

#define PI 3.141592654f

GLfloat   lightPos[4] = {0.0, 1000.0, 0.0, 1.0};
GLfloat   shadowMatrix[16];

GLuint airstripTexture;
GLuint groundTexture;

float theta = 0.0f, phi = 0.0f;
float distance = 100.0f;

float targetX = 0.0f, targetY = 4.0f, targetZ = 0.0f;
float cameraX = 0.0f, cameraY = 0.0f, cameraZ = -distance;

int width = 800, height = 600;
float yPos = 0.0f;
bool followAircraft= true;

void SetShadowMatrix(GLfloat destMat[16], float lightPos[4], float plane[4])
{
  GLfloat dot;

  // dot product of plane and light position
  dot = plane[0] * lightPos[0] + plane[1] * lightPos[1] + plane[2] * lightPos[2] + plane[3] * lightPos[3];

  // first column
  destMat[0] = dot - lightPos[0] * plane[0];
  destMat[4] = 0.0f - lightPos[0] * plane[1];
  destMat[8] = 0.0f - lightPos[0] * plane[2];
  destMat[12] = 0.0f - lightPos[0] * plane[3];

  // second column
  destMat[1] = 0.0f - lightPos[1] * plane[0];
  destMat[5] = dot - lightPos[1] * plane[1];
  destMat[9] = 0.0f - lightPos[1] * plane[2];
  destMat[13] = 0.0f - lightPos[1] * plane[3];

  // third column
  destMat[2] = 0.0f - lightPos[2] * plane[0];
  destMat[6] = 0.0f - lightPos[2] * plane[1];
  destMat[10] = dot - lightPos[2] * plane[2];
  destMat[14] = 0.0f - lightPos[2] * plane[3];

  // fourth column
  destMat[3] = 0.0f - lightPos[3] * plane[0];
  destMat[7] = 0.0f - lightPos[3] * plane[1];
  destMat[11] = 0.0f - lightPos[3] * plane[2];
  destMat[15] = dot - lightPos[3] * plane[3];
} // end SetShadowMatrix()

void LoadTexture(char *filename, GLuint &texture)
{
  // get a texture object
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);


  BitmapFile bmf;
  bmf.load(filename);

  // set up the texture
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmf.w, bmf.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, bmf.rgba_data);

} // end LoadTexture()



void Reshape(int w, int h)
{
	width = w;
	height = h;
	// set the viewport to the new dimensions
	glViewport(0, 0, w, h);

	// select the projection matrix and clear it out
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// set the perspective with the appropriate aspect ratio
	gluPerspective(45.0f, (GLfloat)w/(GLfloat)h, 1.0f, 1000.0f);
	gluLookAt(cameraX, cameraY, cameraZ, targetX, targetY, targetZ, 0.0, 1.0, 0.0);

	// select modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	glutPostRedisplay();
}

void SetCamera()
{
	targetX;// = 0.0f;
	targetY; //= 0.0f;//objects[currObject].y;
	targetZ; //= 0.0f;//objects[currObject].z;

	cameraX = (cosf(phi) * sinf(theta) * -distance)+targetX ;
	cameraY = (sinf(phi)               * -distance)+targetY ; 
	cameraZ = (cosf(phi) * cosf(theta) * -distance)+targetZ ; 

	Reshape(width, height);

}

void Init()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	// set up a single white light
	GLfloat lightColor[] = { 1.0f, 1.0f, 1.0f, 0.6 };
	GLfloat ambientColor[] = { 0.5f, 0.5f, 0.5f, 0.6 };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);
	MakeDisplayList();
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	LoadTexture("grass.bmp", groundTexture);
	LoadTexture("gravel.bmp", airstripTexture);

	jetRotation[13] = 20;
	jetRotation[14] = -400;
	targetX = jetRotation[12];
		targetY = jetRotation[13];
		targetZ = jetRotation[14];

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float plane[4] = { 0.0, 1.0, 0.0, 0.0 };
  SetShadowMatrix(shadowMatrix, lightPos, plane);
  
	

	
}

void DrawAircraft()
{
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	//glPushMatrix();
	//glLoadMatrixf(jetRotation);
	glCallList(dlJet);
	//glPopMatrix();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

void DrawSurface()
{
	GLfloat surfaceColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, surfaceColor);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, surfaceColor);
	glMaterialf(GL_FRONT, GL_SHININESS, 200.0);

	glBindTexture(GL_TEXTURE_2D, groundTexture);
	
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);

	GLfloat x = -500.0, z = -500.0;

	for (GLint i = 0; i < 10; i++, x += 100.0)
	{
		for (GLint j = 0; j < 10; j++, z += 100.0)
		{
			// draw the plane slightly offset so the shadow shows up
			glTexCoord2f(0.0, 0.0);
			glVertex3f(x, 0.0, z);
			glTexCoord2f(1.0, 0.0);
			glVertex3f(x + 100.0f, 0.0, z);
			glTexCoord2f(1.0, 1.0);
			glVertex3f(x + 100.0f, 0.0, z + 100.0f);
			glTexCoord2f(0.0, 1.0);
			glVertex3f(x, 0.0, z + 100.0f);
		}
		z = -500.0;
	}
	
	glEnd();

	glBindTexture(GL_TEXTURE_2D, airstripTexture);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-50.0, 0.1, -500.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(50.0f, 0.1, -500.0);
	glTexCoord2f(1.0, 16.0);
	glVertex3f(50.0f, 0.1, 500.0f);
	glTexCoord2f(0.0, 16.0);
	glVertex3f(-50, 0.1, 500.0f);

	glEnd();
	
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//glLoadIdentity();

	DrawSurface();

	// *****************************************************************************************************
  // prepare to write to the stencil buffer by turning off writes to the
  // color and depth buffer
// *****************************************************************************************************
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthMask(GL_FALSE);

// *****************************************************************************************************
  // set up the stencil func and op to place a 1 in the stencil buffer
  // everywhere we're about to draw
// *****************************************************************************************************
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

// *****************************************************************************************************
  // render the surface. Since the depth and color buffers are disabled,
  // only the stencil buffer will be modified
// *****************************************************************************************************
  DrawSurface();
  // *****************************************************************************************************
  // turn the color and depth buffers back on
// *****************************************************************************************************
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_TRUE);

  // *****************************************************************************************************
  // from this point until the stencil test is disabled, only draw where
  // the stencil buffer is set to 1
// *****************************************************************************************************
  glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);

// *****************************************************************************************************
  // don't modify the contents of the stencil buffer
// *****************************************************************************************************
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

// *****************************************************************************************************
    // draw the shadow as black, blended with the surface, with no lighting, and not
    // preforming the depth test
    // Note that blending is enabled for blending of the shadow with the surface
// *****************************************************************************************************
  glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

// *****************************************************************************************************
    // make sure that we don't draw at any raster position more than once
// *****************************************************************************************************
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glColor4f(0.0, 0.0, 0.0, 1.0f);

// *****************************************************************************************************
    // project the cube through the shadow matrix
// *****************************************************************************************************
	
	//glLoadMatrixf(jetRotation);
	
	//glLoadMatrixf(jetRotation);
	float jet[16];
	
	memcpy(jet, jetRotation, sizeof(float)*16);
	//jet[13] = -jetRotation[13];
	//glLoadMatrixf(jetRotation);
	
	glLoadMatrixf(shadowMatrix);
	glMultMatrixf(jetRotation);
	
	
	
	DrawAircraft();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
  glPopMatrix();
  glDisable(GL_STENCIL_TEST);
  

	glPushMatrix();
	glColor4f(0.0, 0.0, 1.0, 1.0f);
	
	
	glLoadMatrixf(jetRotation);
	DrawAircraft();
	glPopMatrix();
	glFlush();

	glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:
			exit(0);
			break;

		case 'w':
		case 'W':
			glPushMatrix();
			glLoadMatrixf(jetRotation);
			glTranslatef(0.0f, 0.0f, 3.0f);
			//glRotatef(-3.0f, 1.0f, 0.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
			glPopMatrix();
			break;
		case 's':
		case 'S':
			glPushMatrix();
			glLoadMatrixf(jetRotation);
			glTranslatef(0.0f, 0.0f, -3.0f);
			//glRotatef(-3.0f, 1.0f, 0.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
			glPopMatrix();
			break;

		case 'e':
		case 'E':
			glPushMatrix();
			glLoadMatrixf(jetRotation);
			//glTranslatef(0.0f, 0.0f, 3.0f);
			glRotatef(-3.0f, 1.0f, 0.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
			glPopMatrix();
			break;
		case 'q':
		case 'Q':
			glPushMatrix();
			glLoadMatrixf(jetRotation);
			//glTranslatef(0.0f, 0.0f, 3.0f);
			glRotatef(3.0f, 1.0f, 0.0f, 0.0f);
			glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
			glPopMatrix();
			break;

		case '1':
			distance += 1.0f;
			break;
		case '2':
			distance -= 1.0f;
			break;

		case 'x':
			targetX += 1.0f;
			break;
		case 'X':
			targetX -= 1.0f;
			break;
		case 'y':
			targetY += 1.0f;
			break;
		case 'Y':
			targetY -= 1.0f;
			break;
		case 'z':
			targetZ += 1.0f;
			break;
		case 'Z':
			targetZ -= 1.0f;
			break;

		case 'f':
		case 'F':
			followAircraft = !followAircraft;
			break;
		case 'r':
		case 'R':
			{
				jetRotation[13] = 20;
				jetRotation[14] = -400;
				targetX = jetRotation[12];
				targetY = jetRotation[13];
				targetZ = jetRotation[14];
				phi = 0.0f;
				theta = 0.0f;
				distance = 100.0f;
			}break;
			
	}
	if (followAircraft)
	{
		targetX = jetRotation[12];
		targetY = jetRotation[13];
		targetZ = jetRotation[14];
	}
	if (jetRotation[13] < 3)
	{
		jetRotation[13] = 3;
		glPushMatrix();
		//glLoadMatrixf(jetRotation);
		glTranslatef(jetRotation[12], jetRotation[13], jetRotation[14]);
		glGetFloatv(GL_MODELVIEW_MATRIX, jetRotation);
		glPopMatrix();
	}
	SetCamera();
	glutPostRedisplay();
}

void SpecialKey(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_LEFT:
			theta -= 0.1f;
			break;
		case GLUT_KEY_RIGHT:
			theta += 0.1f;
			break;
		case GLUT_KEY_UP:
			phi -= 0.1f;
			break;
		case GLUT_KEY_DOWN:
			phi += 0.1f;
			break;
	}
	if (phi > 1.57f) phi = 1.57f;
	else if (phi < -1.57f) phi = -1.57f;
	if (theta > 6.28f) theta = 0.0f;
	else if (theta < 0.0f) theta = 6.28f;

	SetCamera();
	glutPostRedisplay();
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("Lab14a - Shadows and Reflections");
	Init();
    glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKey);
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}