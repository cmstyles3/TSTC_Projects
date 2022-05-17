/*
 Instructions
 
 Controls

 Press Escape to Exit application

------CAMERA CONTROLS --------
(note that these controls are based upon
the cameras current orientation, not related
to the axis)
    up arrow - pitch up
  down arrow - pitch down
  left arrow - yaw left
 right arrow - yaw right
         'q' - roll left
		 'e' - roll right
         'w' - move forward
		 's' - move backward
		 'a' - move left
		 'd' - move right
		 'z' - move down
		 'x' - move up
		 'r' - reset orientation
 -----------------------------

 SMOKE COLOR CONTROLS

 '1' - decrease red
 '2' - increase red

 '3' - decrease green
 '4' - increase green

 '5' - decrease blue
 '6' - increase blue


*/
#include <windows.h>
#include <gl/glut.h>
#include <vector>

#include "bitmap.h"
#include "camera.h"
#include "aircraft.h"

struct Particle
{
	float x, y, z;
	float xv, yv, zv;
	float alpha;

	Camera obj;
};

#define MAX_PARTICLES 50
Particle particles[MAX_PARTICLES];

float red=0.4f, green=0.4f, blue=0.4;

Camera g_cam1;

GLuint g_smokeTexture;
GLuint airstripTexture;
GLuint groundTexture;
GLuint skyTexture[6];

Camera aircraftObj;

bool moveforward = false;
bool movebackward = false;
bool strafeleft = false;
bool straferight = false;

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

void Init()
{
	g_cam1 = Camera(0,10,400);

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		particles[i].x = (float)(rand()%1000-500) * 0.01f;
		particles[i].y = -20.0f;//(float)(rand()%500) * 0.1f;
		particles[i].z = (float)(rand()%1000-500) * 0.01f;
		particles[i].obj = Camera(particles[i].x, particles[i].y, particles[i].z);
		particles[i].alpha = 1.0f;
		particles[i].xv = (float)(rand()%100 - 50) * 0.1f;
		particles[i].yv = (float)(rand()%200+20) * 0.1f;
		particles[i].zv = (float)(rand()%100 - 50) * 0.1f;
		
	}

	

	glClearColor(0.0, 0.0, 1.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 800.0/600.0, 0.1, 2000.0);

	glEnable(GL_TEXTURE_2D);
	
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glEnable(GL_DEPTH_TEST);

	LoadTexture("smoke.bmp", g_smokeTexture);
	LoadTexture("grass.bmp", groundTexture);
	LoadTexture("gravel.bmp", airstripTexture);
	LoadTexture("north.bmp", skyTexture[0]);
	LoadTexture("west.bmp", skyTexture[1]);
	LoadTexture("south.bmp", skyTexture[2]);
	LoadTexture("east.bmp", skyTexture[3]);
	LoadTexture("up.bmp", skyTexture[4]);
	LoadTexture("down.bmp", skyTexture[5]);

	aircraftObj = Camera(4.0f, 3.0f, -1.0);
	aircraftObj.Pitch(10);
	MakeDisplayList();
	

}
void Kill()
{
	glDeleteTextures(6, skyTexture);
	glDeleteTextures(1, &groundTexture);
	glDeleteTextures(1, &airstripTexture);
	glDeleteTextures(1, &g_smokeTexture);
}

void DrawSkyBox()
{
#define SKYBOX_SIZE  1000.0f
	glPushMatrix();
	Vector3 pos = g_cam1.GetPosition();
	glTranslatef(pos.x, pos.y, pos.z);
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glBindTexture(GL_TEXTURE_2D, skyTexture[0]);
	

	glBegin(GL_QUADS);
	// North
	
	glTexCoord2f(0.0, 1.0);
		glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE+1);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, -SKYBOX_SIZE+1);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE+1);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, -SKYBOX_SIZE+1);
	glEnd();

	//West
	glBindTexture(GL_TEXTURE_2D, skyTexture[1]);
	
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-SKYBOX_SIZE+1, SKYBOX_SIZE, SKYBOX_SIZE);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(-SKYBOX_SIZE+1, SKYBOX_SIZE, -SKYBOX_SIZE);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(-SKYBOX_SIZE+1, -SKYBOX_SIZE, -SKYBOX_SIZE);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-SKYBOX_SIZE+1, -SKYBOX_SIZE, SKYBOX_SIZE);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skyTexture[2]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE-1);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE, SKYBOX_SIZE-1);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE-1);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE, SKYBOX_SIZE-1);	
	
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skyTexture[3]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(SKYBOX_SIZE-1, SKYBOX_SIZE, -SKYBOX_SIZE);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(SKYBOX_SIZE-1, SKYBOX_SIZE, SKYBOX_SIZE);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(SKYBOX_SIZE-1, -SKYBOX_SIZE, SKYBOX_SIZE);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(SKYBOX_SIZE-1, -SKYBOX_SIZE, -SKYBOX_SIZE);	
	
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skyTexture[4]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE-1, SKYBOX_SIZE);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE-1, SKYBOX_SIZE);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(SKYBOX_SIZE, SKYBOX_SIZE-1, -SKYBOX_SIZE);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-SKYBOX_SIZE, SKYBOX_SIZE-1, -SKYBOX_SIZE);	
	
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skyTexture[5]);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE+1, -SKYBOX_SIZE);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE+1, -SKYBOX_SIZE);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(SKYBOX_SIZE, -SKYBOX_SIZE+1, SKYBOX_SIZE);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(-SKYBOX_SIZE, -SKYBOX_SIZE+1, SKYBOX_SIZE);	
	
	glEnd();
	glPopMatrix();



}

void DrawSurface()
{
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, groundTexture);
	
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);

	GLfloat x = -500.0, z = -500.0;
	glColor3f(1.0f, 1.0f, 1.0f);

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
	glVertex3f(-50.0, 0.5, -500.0);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(50.0f, 0.5, -500.0);
	glTexCoord2f(1.0, 16.0);
	glVertex3f(50.0f, 0.5, 500.0f);
	glTexCoord2f(0.0, 16.0);
	glVertex3f(-50, 0.5, 500.0f);

	glEnd();
	
	glPopMatrix();
}

void DrawSmoke()
{
	glDepthMask(FALSE);
	glEnable(GL_BLEND);

	for (int i = 0;  i < MAX_PARTICLES; i++)
	{
		glColor4f(red, green, blue,particles[i].alpha);
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, g_smokeTexture);
		particles[i].obj.SetTransformMatrix();
		
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-20.0f, 20.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(20.0f, 20.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(20.0f, -20.0f, 0.0f);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-20.0f, -20.0f, 0.0f);
		glEnd();
		glPopMatrix();
		
	}
	
	glDisable(GL_BLEND);
	glDepthMask(TRUE);
}

void Display()
{
	g_cam1.SetViewMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawSkyBox();
	
	DrawSurface();

	glPushMatrix();
	aircraftObj.SetTransformMatrix();
	//glTranslatef(0.0f, 10.0f, 0.0f);
	glCallList(dlJet);
	glPopMatrix();
	
	DrawSmoke();

	
	
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
			moveforward = true;
			
			break;

		case 's':
			movebackward = true;
			
			break;

		case 'a':
			strafeleft = true;
			
			break;

		case 'd':
			straferight = true;
			
			break;

		case 'q':
			g_cam1.Roll(1);
			break;

		case 'e':
			g_cam1.Roll(-1);
			break;

		case 'z':
			g_cam1.Fly(-1);
			break;

		case 'x':
			g_cam1.Fly(1);
			break;

		case 'r':
			g_cam1.Reset();
			break;

		case '1':
			red -= 0.1f;
			if (red < 0.0f) red = 0.0f;
			break;
		case '2':
			red += 0.1f;
			if (red > 1.0f) red = 1.0f;
			break;

		case '3':
			green -= 0.1f;
			if (green < 0.0f) green = 0.0f;
			break;
		case '4':
			green += 0.1f;
			if (green > 1.0f) green = 1.0f;
			break;

		case '5':
			blue -= 0.1f;
			if (blue < 0.0f) blue = 0.0f;
			break;
		case '6':
			blue += 0.1f;
			if (blue > 1.0f) blue = 1.0f;
			break;

		

	}
}

void KeyboardUp(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'w':
			moveforward = false;
			break;

		case 's':
			movebackward = false;
			break;

		case 'a':
			strafeleft = false;
			
			break;

		case 'd':
			straferight = false;
			
			break;
	}
}

void SpecialKey(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			g_cam1.Pitch(-1);
			break;
		case GLUT_KEY_DOWN:
			g_cam1.Pitch(1);
			break;
		case GLUT_KEY_LEFT:
			g_cam1.Yaw(-1);
			break;
		case GLUT_KEY_RIGHT:
			g_cam1.Yaw(1);
			break;
	}
}

void Update(float dt)
{
	if (moveforward)
		g_cam1.Walk(1);

	if (movebackward)
		g_cam1.Walk(-1);

	if (strafeleft)
		g_cam1.Strafe(-1);
	if (straferight)
		g_cam1.Strafe(1);

	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		particles[i].x += particles[i].xv * dt * (particles[i].alpha+1.0f);
		particles[i].z += particles[i].zv * dt * (particles[i].alpha+1.0f);
		particles[i].xv += 0.9f * dt;
		particles[i].y += particles[i].yv * dt * (particles[i].alpha+1.0f);
		
		particles[i].alpha  = (-(particles[i].y+particles[i].x) +200.0f)/200.0f;
		
		if (particles[i].alpha <= 0.0f)
		{	
			particles[i].x = (float)(rand()%1000-500) * 0.02f;
			particles[i].y = -10.0f;
			particles[i].z = (float)(rand()%1000-500) * 0.02f;
			particles[i].obj = Camera(particles[i].x, particles[i].y, particles[i].z);
			particles[i].alpha = 1.0f;
			particles[i].xv = (float)(rand()%50 - 25) * 0.1f;
			particles[i].yv = (float)(rand()%40+40) * 0.1f;
			particles[i].zv = (float)(rand()%50 - 25) * 0.1f;
			
		}
		Vector3 pos = Vector3(particles[i].x, particles[i].y, particles[i].z);
	//	particles[i].obj.Reset();
		particles[i].obj.SetPosition(pos);
		Vector3 look;
		
		Vec3Sub(&look, &particles[i].obj.GetPosition(), &g_cam1.GetPosition());
		particles[i].obj.SetLook(look);	
		
		
	}
}

void Idle()
{
	static float lastTime = (float)timeGetTime();
	float currTime = (float)timeGetTime();
	float dt = (currTime - lastTime)*0.001f;
	
	Update(dt);
		
	Display();

	lastTime = currTime;
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(50, 50);
	
	glutCreateWindow("Lab15b - Natural Phenomena");

	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);//void (*func)(unsigned char key,int x,int y));
	glutSpecialFunc(SpecialKey);
	glutIdleFunc(Idle);

	Init();

	glutMainLoop();

	Kill();
}