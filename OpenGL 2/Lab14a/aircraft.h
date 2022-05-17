#include <math.h>
#include <gl/glut.h>

GLfloat jetRotation[16];

GLuint dlWing, dlBox, dlCone, dlJet;

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
		//glColor4fv(lt_grey);

		//draw body of jet
		glScalef(0.8, 0.8, 3.0);
		glTranslatef(-3, 0, 0);
		glCallList(dlBox);
		glTranslatef(6, 0, 0);
		glCallList(dlBox);
		glPopMatrix();
		glPushMatrix();
		glTranslatef(0, 0, 1.5);
		//glColor3fv(lt_grey2);
		glScalef(1, 1, 3.5);
		glCallList(dlBox);
		
		//cockpit
		//glColor3fv(lt_grey);
		glTranslatef(0, 0, 3);
		glScalef(1, 1, 0.8);
		glCallList(dlCone);
		//glass
		//glColor4fv(lt_blue);
		glTranslatef(0, 1.0, 1.3);
		glScalef(0.7, 0.7, 0.3);
		glRotatef(10, 1, 0, 0);
		glCallList(dlCone);
		glRotatef(180, 1, 0, 0);
		glCallList(dlCone);

		//main wings
		glPopMatrix();
		glPushMatrix();
		//glColor4fv(lt_grey2);
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
		//glColor4fv(flame);
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