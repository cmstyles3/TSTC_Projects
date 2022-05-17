// cmmLab11c.cpp
// Programmed by Collin Munson
// Controls 
// --------
/* 
   '1' - select star object
   '2' - select triangle object
   '3' - select cylinder object
   '4' - select box object - this object can not be moved

   when an object is selected it will become the target for the camera to look at

   use arrow keys - to rotate camera around selected object
   '+' - moves camera closer to selected object
   '-' - moves camera farther from selected object

   'z' - moves selected object in the negative z direction
   'Z' - moves selected object in the position z direction
   'y' - moves selected object in the position y direction
   'Y' - moves selected object in the negative y direction
   'x' - moves selected object in the positive x direction
   'X' - moves selected object in the negative x direction

   Esc - exits application

*/

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl/glut.h>

#ifndef CALLBACK
#define CALLBACK
#endif

#define PI 3.1415f

GLvoid *font_style = GLUT_BITMAP_HELVETICA_18;
GLuint startList;


struct Object
{
	GLfloat x, y, z;
};


enum {BOX, STAR, TRIANGLE, CYLINDER};

int currObject = STAR;

Object objects[4] = 
{
	{   0.0f, 0.0f,  0.0f},
	{-450.0f, -300.0f, 300.0f},
	{-300.0f, -450.0f, 300.0f},
	{-300.0f, -600.0f, 300.0f}
};


GLfloat targetX = objects[STAR].x, targetY = objects[STAR].y, targetZ = objects[STAR].z;

GLfloat distance = 400.0f;

float theta = 0.0f, phi = 0.0f;

float width = 800.0f, height = 600.0f;

float cameraX = objects[STAR].x, cameraY = objects[STAR].y, cameraZ = objects[STAR].z + distance;

//--------------------------------------------------------------------------------------------------------------------------------------
//  Setup tesselator callback functions
//--------------------------------------------------------------------------------------------------------------------------------------
void CALLBACK BeginCallback(GLenum which)
{
	glBegin(which);
}

void CALLBACK ErrorCallback(GLenum errorCode)
{
	const GLubyte *estring;

	estring = gluErrorString(errorCode);
	fprintf(stderr, "Tessellation Error: %s\n", estring);
	exit(0);
}

void CALLBACK EndCallback(void)
{
	glEnd();
}

void CALLBACK VertexCallback(GLdouble *vertex)
{
	const GLdouble *pointer;

	pointer = (GLdouble *) vertex;
	glColor3dv(pointer+3);
	glVertex3dv((GLdouble*)vertex);
}

/*  combineCallback is used to create a new vertex when edges
 *  intersect.  coordinate location is trivial to calculate,
 *  but weight[4] may be used to average color, normal, or texture
 *  coordinate data.  In this program, color is weighted.
 */
void CALLBACK CombineCallback(GLdouble coords[3], GLdouble *vertex_data[4],
                     GLfloat weight[4], GLdouble **dataOut )
{
	GLdouble *vertex;
	int i;

	vertex = (GLdouble *) malloc(6 * sizeof(GLdouble));

	vertex[0] = coords[0];
	vertex[1] = coords[1];
	vertex[2] = coords[2];
	for (i = 3; i < 6; i++)
		vertex[i] = weight[0] * vertex_data[0][i] + weight[1] * vertex_data[1][i]
			+ weight[2] * vertex_data[2][i] + weight[3] * vertex_data[3][i];
	*dataOut = vertex;
}



void Reshape(int w, int h)
{
	width = w, height = h;

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (float)w/(float)h, 1, 5000);

	gluLookAt(cameraX, cameraY, cameraZ, targetX, targetY, targetZ, 0.0, 1.0, 0.0);
	
}

void Init()
{
	GLUtesselator *tobj;
	GLUquadric *qobj;

	GLdouble rect[4][3] = 
	{
		-300.0f, -100.0f, 0.0f,
		 300.0f, -100.0f, 0.0f,
		 300.0f,  100.0f, 0.0f,
		-300.0f,  100.0f, 0.0f
	};

	const double starXOffset = -150.0f;

	GLdouble star[5][3] = 
	{
		cosf(PI/2.0f) * 80 + starXOffset, sinf(PI/2.0f) * 80, 0.0,
		cosf(306 * PI/180.0f) * 80.0f + starXOffset, sinf(306 * PI/180.0f) * 80.0f, 0.0,
		cosf(162 * PI/180.0f) * 80.0f + starXOffset, sinf(162 * PI/180.0f) * 80.0f, 0.0,
		cosf(18 * PI/180.0f) * 80.0f + starXOffset, sinf(18 * PI/180.0f) * 80.0f, 0.0, 
		cosf(234 * PI/180.0f) * 80.0f + starXOffset, sinf(234 * PI/180.0f) * 80.0f, 0.0,
	};

	GLdouble tri[3][3] = 
	{
		  0.0,  70.0, 0.0,
		 70.0, -70.0, 0.0,
		-70.0, -70.0, 0.0	
	};

	GLdouble quad[4][3] = 
	{
		-60.0+150.0f,  60.0, 0.0,
		 60.0+150.0f,  60.0, 0.0,
		 60.0+150.0f, -60.0, 0.0,
		 -60.0+150.0f, -60.0, 0.0
	};

	startList = glGenLists(4);

	qobj = gluNewQuadric();

	tobj = gluNewTess();
	gluTessCallback(tobj, GLU_TESS_VERTEX, (void (__stdcall*)(void))glVertex3dv);
	gluTessCallback(tobj, GLU_TESS_BEGIN, (void (__stdcall*)(void))BeginCallback);
	gluTessCallback(tobj, GLU_TESS_END, (void (__stdcall*)(void))EndCallback);
	gluTessCallback(tobj, GLU_TESS_ERROR, (void (__stdcall*)(void))ErrorCallback);
	gluTessCallback(tobj, GLU_TESS_COMBINE, (void (__stdcall*)(void))CombineCallback);

	glNewList(startList, GL_COMPILE);
	glColor3f(1.0f, 1.0f, 0.0f);
	glShadeModel(GL_FLAT);
	gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
		gluTessBeginPolygon(tobj, NULL);
			gluTessBeginContour(tobj);
				gluTessVertex(tobj, rect[0], rect[0]);
				gluTessVertex(tobj, rect[1], rect[1]);
				gluTessVertex(tobj, rect[2], rect[2]);
				gluTessVertex(tobj, rect[3], rect[3]);
			gluTessEndContour(tobj);
			gluTessBeginContour(tobj);
				 gluTessVertex(tobj, quad[0], quad[0]);
				 gluTessVertex(tobj, quad[1], quad[1]);
				 gluTessVertex(tobj, quad[2], quad[2]);
				 gluTessVertex(tobj, quad[3], quad[3]);
				
			gluTessEndContour(tobj);
			gluTessBeginContour(tobj);
				 gluTessVertex(tobj, tri[0], tri[0]);
				 gluTessVertex(tobj, tri[1], tri[1]);
				 gluTessVertex(tobj, tri[2], tri[2]);
				
			gluTessEndContour(tobj);
			gluTessBeginContour(tobj);
				 gluTessVertex(tobj, star[0], star[0]);
				 gluTessVertex(tobj, star[1], star[1]);
				 gluTessVertex(tobj, star[2], star[2]);
				 gluTessVertex(tobj, star[3], star[3]);
				 gluTessVertex(tobj, star[4], star[4]);
			gluTessEndContour(tobj);
		 gluTessEndPolygon(tobj);
		 glBegin(GL_QUADS);
		 //top face
			glColor3f(1.0f, 0.9f, 0.0f);
			glVertex3f(-300.0f, 100.0f, -200.0f);
			glVertex3f( 300.0f, 100.0f, -200.0f);
			glVertex3f( 300.0f, 100.0f, 0.0f);
			glVertex3f(-300.0f, 100.0f, 0.0f);

			//back face
			glColor3f(1.0f, 0.8f, 0.0f);
			glVertex3f( 300.0f, 100.0f, -200.0f);
			glVertex3f(-300.0f, 100.0f, -200.0f);
			glVertex3f(-300.0f, -100.0f, -200.0f);
			glVertex3f( 300.0f, -100.0f, -200.0f);

			//right face
			glColor3f(1.0f, 0.85f, 0.0f);
			glVertex3f( 300.0f, 100.0f, 0.0f);
			glVertex3f( 300.0f, 100.0f, -200.0f);
			glVertex3f( 300.0f, -100.0f, -200.0f);
			glVertex3f( 300.0f, -100.0f,  0.0f);

			//left face
			glColor3f(1.0f, 0.85f, 0.0f);
			glVertex3f(-300.0f,  100.0f, -200.0f);
			glVertex3f(-300.0f,  100.0f,    0.0f);
			glVertex3f(-300.0f, -100.0f,   0.0f);
			glVertex3f(-300.0f, -100.0f, -200.0f);

			//bottom face
			glColor3f(1.0f, 0.9f, 0.0f);
			glVertex3f(-300.0f, -100.0f, -200.0f);
			glVertex3f( 300.0f, -100.0f, -200.0f);
			glVertex3f( 300.0f, -100.0f, 0.0f);
			glVertex3f(-300.0f, -100.0f, 0.0f);



		 glEnd();
	glEndList();

	glNewList(startList + 1, GL_COMPILE);
		glColor3f(1.0, 0.0, 0.0);
		for (int i = 0; i < 5; i++)
		{
			star[i][0] -= starXOffset;
		}
		glShadeModel(GL_SMOOTH);    
		gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
		gluTessBeginPolygon(tobj, NULL);
			gluTessBeginContour(tobj);
				gluTessVertex(tobj, star[0], star[0]);
				gluTessVertex(tobj, star[1], star[1]);
				gluTessVertex(tobj, star[2], star[2]);
				gluTessVertex(tobj, star[3], star[3]);
				gluTessVertex(tobj, star[4], star[4]);
			gluTessEndContour(tobj);
		gluTessEndPolygon(tobj);

		glColor3f(0.7, 0.0, 0.0);
		glPushMatrix();
		glTranslatef(0.0, 0.0, -50.0);
		gluTessBeginPolygon(tobj, NULL);
			gluTessBeginContour(tobj);
				gluTessVertex(tobj, star[0], star[0]);
				gluTessVertex(tobj, star[1], star[1]);
				gluTessVertex(tobj, star[2], star[2]);
				gluTessVertex(tobj, star[3], star[3]);
				gluTessVertex(tobj, star[4], star[4]);
			gluTessEndContour(tobj);
		gluTessEndPolygon(tobj);
		glPopMatrix();

		glColor3f(0.8, 0.0, 0.0);
		glBegin(GL_QUADS);
			//glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3d(star[0][0], star[0][1], 0.0);
			glVertex3d(star[0][0], star[0][1], -50.0);
			//glColor3d(star[1][3], star[1][4], star[1][5]);
			glVertex3d(star[1][0], star[1][1], -50.0);
			glVertex3d(star[1][0], star[1][1], 0.0);

			//glColor3d(star[1][3], star[1][4], star[1][5]);
			glVertex3d(star[1][0], star[1][1], 0.0);
			glVertex3d(star[1][0], star[1][1], -50.0);
			//glColor3d(star[2][3], star[2][4], star[0][5]);
			glVertex3d(star[2][0], star[2][1], -50.0);
			glVertex3d(star[2][0], star[2][1], 0.0);

			//glColor3d(star[2][3], star[2][4], star[2][5]);
			glVertex3d(star[2][0], star[2][1], 0.0);
			glVertex3d(star[2][0], star[2][1], -50.0);
			//glColor3d(star[3][3], star[3][4], star[3][5]);
			glVertex3d(star[3][0], star[3][1], -50.0);
			glVertex3d(star[3][0], star[3][1],  0.0);

			//glColor3d(star[3][3], star[3][4], star[3][5]);
			glVertex3d(star[3][0], star[3][1], 0.0);
			glVertex3d(star[3][0], star[3][1], -50.0);
			//glColor3d(star[4][3], star[4][4], star[4][5]);
			glVertex3d(star[4][0], star[4][1], -50.0);
			glVertex3d(star[4][0], star[4][1], 0.0);

			//glColor3d(star[4][3], star[4][4], star[4][5]);
			glVertex3d(star[4][0], star[4][1],  0.0);
			glVertex3d(star[4][0], star[4][1], -50.0);
			//glColor3d(star[0][3], star[0][4], star[0][5]);
			glVertex3d(star[0][0], star[0][1], -50.0);
			glVertex3d(star[0][0], star[0][1], 0.0);
		glEnd();
	glEndList();

	glNewList(startList + 2, GL_COMPILE);
		glColor3f(0.0, 1.0, 0.0);
		glShadeModel(GL_SMOOTH);    
		gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
		gluTessBeginPolygon(tobj, NULL);
			gluTessBeginContour(tobj);
				gluTessVertex(tobj, tri[0], tri[0]);
				gluTessVertex(tobj, tri[1], tri[1]);
				gluTessVertex(tobj, tri[2], tri[2]);
				
				
			gluTessEndContour(tobj);
		gluTessEndPolygon(tobj);

		glColor3f(0.0, 0.7, 0.0);
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, -50.0f);
		gluTessBeginPolygon(tobj, NULL);
			gluTessBeginContour(tobj);
				gluTessVertex(tobj, tri[0], tri[0]);
				gluTessVertex(tobj, tri[1], tri[1]);
				gluTessVertex(tobj, tri[2], tri[2]);
				
				
			gluTessEndContour(tobj);
		gluTessEndPolygon(tobj);
		glPopMatrix();

		glColor3f(0.0, 0.8, 0.0);
		glBegin(GL_QUADS);
			//glColor3d(star[0][3], star[0][4], star[0][5]);
			glVertex3d(tri[0][0], tri[0][1], 0.0);
			glVertex3d(tri[0][0], tri[0][1], -50.0);
			//glColor3d(star[1][3], star[1][4], star[1][5]);
			glVertex3d(tri[1][0], tri[1][1], -50.0);
			glVertex3d(tri[1][0], tri[1][1], 0.0);

			glVertex3d(tri[1][0], tri[1][1], 0.0);
			glVertex3d(tri[1][0], tri[1][1], -50.0);
			//glColor3d(star[1][3], star[1][4], star[1][5]);
			glVertex3d(tri[2][0], tri[2][1], -50.0);
			glVertex3d(tri[2][0], tri[2][1], 0.0);

			glVertex3d(tri[2][0], tri[2][1], 0.0);
			glVertex3d(tri[2][0], tri[2][1], -50.0);
			//glColor3d(star[1][3], star[1][4], star[1][5]);
			glVertex3d(tri[0][0], tri[0][1], -50.0);
			glVertex3d(tri[0][0], tri[0][1], 0.0);

			
		glEnd();
	glEndList();
	gluDeleteTess(tobj);

	glNewList(startList + 3, GL_COMPILE);
	for (int i = 0; i < 4; i++)
	{
		quad[i][0] += starXOffset;
	}
	glColor3f(0.0f, 0.0f, 0.8f);
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glRotatef(45, 0, 0, 1);
	gluCylinder(qobj, 60, 60, 50, 20, 1);
	glPopMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	gluDisk(qobj, 0, 60, 20, 1); 
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.7f);
	glTranslatef(0.0f, 0.0f, -50.0f);
	gluDisk(qobj, 0, 60, 20, 1);

	glPopMatrix();
	
	glEndList();

	gluDeleteQuadric(qobj);



	glClearColor(0.0, 0.0, 0.0, 1.0);

	Reshape(800, 600);

	glEnable(GL_DEPTH_TEST);

	
}


void Keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:
			exit(0);
			break;

		case 'z':
			if (currObject != BOX)
				objects[currObject].z -= 10.0f;
			break;

		case 'Z':
			if (currObject != BOX)
				objects[currObject].z += 10.0f;
			break;

		case 'y':
			if (currObject != BOX)
				objects[currObject].y += 10.0f;
			break;
		case 'Y':
			if (currObject != BOX)
				objects[currObject].y -= 10.0f;
			break;

		case 'x':
			if (currObject != BOX)
				objects[currObject].x += 10.0f;
			break;
		case 'X':
			if (currObject != BOX)
				objects[currObject].x -= 10.0f;
			break;

		case '1':
			currObject = STAR;
			break;

		case '2':
			currObject = TRIANGLE;
			break;

		case '3':
			currObject = CYLINDER;
			break;

		case '4':
			currObject = BOX;
			break;
		case '+':
			distance -= 10.0f;
			break;
		case '-':
			distance += 10.0f;
			break;
	}


	targetX = objects[currObject].x;
	targetY = objects[currObject].y;
	targetZ = objects[currObject].z;

	cameraX = (cosf(phi) * sinf(theta) * distance)+targetX ;
	cameraY = (sinf(phi)               * distance)+targetY ; 
	cameraZ = (cosf(phi) * cosf(theta) * distance)+targetZ ; 


	Reshape(width, height);

	
	glutPostRedisplay();
}

void SpecialKeyboard(int key, int x, int y)
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
			phi += 0.1f;
			break;

		case GLUT_KEY_DOWN:
			phi -= 0.1f;
			break;
	}

	if (phi > 1.57f) phi = 1.57f;
	else if (phi < -1.57f) phi = -1.57f;
	if (theta > 6.28f) theta = 0.0f;
	else if (theta < 0.0f) theta = 6.28f;

	targetX = objects[currObject].x;
	targetY = objects[currObject].y;
	targetZ = objects[currObject].z;

	cameraX = (cosf(phi) * sinf(theta) * distance)+targetX ;
	cameraY = (sinf(phi)               * distance)+targetY ; 
	cameraZ = (cosf(phi) * cosf(theta) * distance)+targetZ ; 


	Reshape(width, height);

	
	glutPostRedisplay();
}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//DrawText(-100.0f, 100.0f, 0.0f, "stuff");
	
	glCallList(startList);

	glPushMatrix();
	glTranslatef(objects[STAR].x, objects[STAR].y, objects[STAR].z);
	glCallList(startList+1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(objects[TRIANGLE].x, objects[TRIANGLE].y, objects[TRIANGLE].z);
	glCallList(startList+2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(objects[CYLINDER].x, objects[CYLINDER].y, objects[CYLINDER].z);
	glCallList(startList+3);
	glPopMatrix();

	glutSwapBuffers();
}



void Menu(int item)
{
	switch(item)
	{
		case BOX:
		case STAR:
		case TRIANGLE:
		case CYLINDER:
			currObject = item;
			break;
	}
	targetX = objects[currObject].x;
	targetY = objects[currObject].y;
	targetZ = objects[currObject].z;

	cameraX = (cosf(phi) * sinf(theta) * distance)+targetX ;
	cameraY = (sinf(phi)               * distance)+targetY ; 
	cameraZ = (cosf(phi) * cosf(theta) * distance)+targetZ ; 
	Reshape(width, height);
	glutPostRedisplay();
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Lab11c - Tessellators and Quadrics");
	//Init
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeyboard);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutCreateMenu(Menu);
	glutAddMenuEntry("SELECT BOX", 0); 
	glutAddMenuEntry("SELECT STAR", 1); 
	glutAddMenuEntry("SELECT TRIANGLE", 2); 
	glutAddMenuEntry("SELECT CYLINDER", 3); 
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	Init();
	glutMainLoop();

}