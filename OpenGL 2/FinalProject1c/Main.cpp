#include "Main.h"

Font g_font;
Camera g_cam;

// Camera variables
float g_pitch = 0.0f;
float g_yaw = 0.0f;
float g_roll = 0.0f;

float g_walk = 0.0f;
float g_strafe = 0.0f;
float g_fly = 0.0f;

// cannon variables
Camera g_cannonBaseObj;
Camera g_cannonObj;
float g_cannonYaw = 0.0f;
float g_cannonPitch = 0.0f;
float g_cannonWalk = 0.0f;
bool g_moving = false;

Camera g_target;
float g_targetYaw = 0.0f;
float g_targetPitch = 0.0f;
float g_targetDist = 200.0f;

bool g_fired = false;

CannonBall g_cannonBalls[MAX_CANNONBALLS];
Balloon g_balloons[MAX_BALLOONS];

GLuint g_grassTexture;

Explosion g_explosion;

float g_angle, g_angle2;

int g_numHits = 0, g_numExploded = 0;
bool g_gameover = false;

float g_timer = 80;
bool g_disableTimer = false;

bool g_calcStats = 0;
int g_finalNumHits = 0;
int g_finalNumExploded = 0;
int g_finalScore = 0;

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	char gameModeStr[32];
	sprintf(gameModeStr, "%ix%i:32", (int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);
	glutGameModeString(gameModeStr);
	glutEnterGameMode();
	glutDisplayFunc(Display);
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);
	glutSpecialFunc(SpclKeyDown);
	glutSpecialUpFunc(SpclKeyUp);
	glutIdleFunc(Idle);
	
	Init();
	glutMainLoop();
	glutLeaveGameMode();
	Kill();
}

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

}

void Init()
{
	srand(GetTickCount());

	g_font.init();

	glClearColor(0.0, 0.0, 1.0, 1.0);

	// ENABLE ANTI-ALIASING
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_BLEND);

	// SETUP PROJECTION MATRIX -------------------------
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, SCREEN_WIDTH/SCREEN_HEIGHT, 1.0, 2000.0);
	// -------------------------------------------------

	// ENABLE FOG ----------------------------------
	glEnable(GL_FOG);
	GLfloat fogColor[4] = {0.0f, 0.0f, 1.0f, 1.0f};
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.06f);
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_START, 500.0f);
	glFogf(GL_FOG_END, 2000.0f);
	// ---------------------------------------------

	// LIGHTS --------------------------------------
	GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
	GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightPosition[] = {1.0f, 1.0f, 1.0f, 0.0f};

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glEnable(GL_LIGHT0);

	// ---------------------------------------------

	glEnable(GL_DEPTH_TEST);

	g_cam.Reset();
	g_cam.SetViewMatrix();
	InitCannonDisplayList();


	InitCannonBalls();
	InitBalloons();

	LoadTexture("grass.bmp", g_grassTexture);
	GLuint texture;
	LoadTexture("smoke.bmp", texture);
	g_explosion.SetTexture(texture);
}

void Kill()
{
}

void KeyDown(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 27:
			exit(0);
			break;

		case 'w':
			g_moving = true;
			g_cannonWalk = 100.0f;
			//g_walk = 80.0f;
			break;
		case 's':
			g_cannonWalk = -100.0f;
			g_walk = -80.0f;
			break;


		case 'a':
			g_cannonYaw = -40.0f;
			break;
		case 'd':
			g_cannonYaw = 40.0f;
			break;

		case 'q':
			g_cannonPitch = 40.0f;
			break;

		case 'e':
			g_cannonPitch = -40.0f;
			break;

		case 'b':
			g_moving = true;
			g_cannonWalk = 80.0f;
			break;

		case ' ':
			g_fired = true;
			break;

		case '1':
			g_targetDist += 10.0f;
			break;
		case '2':
			g_targetDist -= 10.0f;
			break;

		case '3':
			g_cannonBaseObj.Fly(10);
			break;
		case '4':
			g_cannonBaseObj.Fly(-10);
			break;

		case '5':
			{
				for (int i = 0; i < MAX_BALLOONS; i++)
				{
					if (g_balloons[i].active)
					{
						g_balloons[i].active = false;
						GLfloat color[3] = {0.0f, 1.0f, 0.0f};
						g_explosion.StartExplosion(g_balloons[i].pos, 6, color);
					}
				}
			}break;
	}
}

void KeyUp(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'w':
		case 's':
			g_moving = false;
			g_cannonWalk = 0.0f;
			//g_walk = 0.0f;
			break;

		case 'a':
		case 'd':
			g_cannonYaw = 0.0f;
			break;

		case 'e':
		case 'q':
			g_cannonPitch = 0.0f;
			break;

		case 'b':
			g_moving = false;
			g_cannonWalk = 0.0f;
			break;

		case ' ':
			g_fired = false;
			break;
	}
}

void SpclKeyDown(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
			g_pitch = -60.0f;
			break;
		case GLUT_KEY_DOWN:
			g_pitch = 60.0f;
			break;

		case GLUT_KEY_LEFT:
			g_yaw = -60.0f;
			break;
		case GLUT_KEY_RIGHT:
			g_yaw = 60.0f;
			break;
	}
}

void SpclKeyUp(int key, int x, int y)
{
	switch(key)
	{
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN:
			g_pitch = 0.0f;
			break;

		case GLUT_KEY_LEFT:
		case GLUT_KEY_RIGHT:
			g_yaw = 0.0f;
			break;
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

void Update(float dt)
{
	g_timer -= dt;
	if (g_timer < 0 && !g_disableTimer) 
	{
		g_gameover = true;
		g_disableTimer = true;
		g_calcStats = true;
	}

	if (g_calcStats)
	{
		g_calcStats = false;
		g_finalNumHits = g_numHits;
		g_finalNumExploded = g_numExploded;
		g_finalScore = g_numHits * 2000 - g_numExploded * 100;
	}
	//g_cam.Pitch(g_pitch * dt);
	//g_cam.Yaw(g_yaw * dt);

	g_target.Pitch(-g_pitch * dt);
	g_target.Yaw(-g_yaw * dt);

	//g_cam.Walk(g_walk * dt);

	g_cannonObj.Yaw(g_cannonYaw * dt);
	g_cannonBaseObj.Yaw(g_cannonYaw * dt);
	g_cannonObj.Pitch(g_cannonPitch * dt);
	
	g_cannonBaseObj.Walk(g_cannonWalk * dt);
	g_cannonObj.SetPosition(g_cannonBaseObj.GetPosition());
	g_target.SetPosition(g_cannonObj.GetPosition());

	Vector3 targetPos = g_target.GetPosition();
	Vector3 targetLook = g_target.GetLook();
	Vector3 targetUp = g_target.GetUp();
	Vector3 pos = Vector3(targetPos.x + targetUp.x * 20 - targetLook.x * g_targetDist, 
		targetPos.y + targetUp.y * 20 - targetLook.y * g_targetDist, targetPos.z + targetUp.z * 20 - targetLook.z * g_targetDist);

	g_cam.SetRUL(g_target.GetRight(), g_target.GetUp(), g_target.GetLook());
	//pos.y += 20;
	if (pos.y < 0) pos.y = 0;
	g_cam.SetPosition(pos);

	g_angle = ((g_cannonWalk * dt)/(2*20.0f * PI)) * 360.0f;
	g_angle2 += g_angle;

	static float timer = 1.0f;

	timer -= dt;
	if (g_fired && timer <= 0.0f) 
	{
		timer = 0.4f;
		FireCannonBall();
	}

	/*Vector3 cannonPos = g_cannonObj.GetPosition();
	Vector3 cannonLook = g_cannonObj.GetLook();
	Vector3 cameraPos = Vector3(cannonPos.x - cannonLook.x * 40, cannonPos.y - cannonLook.y * 40, cannonPos.z - cannonLook.z * 40);

	
	g_cam.SetPosition(cameraPos);
	g_cam.SetRUL(g_cannonObj.GetRight(), g_cannonObj.GetUp(), g_cannonObj.GetLook());*/

	UpdateCannonBalls(dt);
	UpdateBalloons(dt);
	g_explosion.Update(dt);
}

void Display()
{
	g_cam.SetViewMatrix();
	glTranslatef(0.0f, 9.0f, -10.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();
	GLfloat lightPosition[] = {0.0f, 0.0f, 1.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	DrawPlane();
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
	glColor3d(1.0, 0.0, 0.0);
	g_cannonBaseObj.SetTransformMatrix();
	static float angle = 0.0f;
	if (g_cannonWalk > 0) angle += g_angle;
	else if (g_cannonWalk < 0) angle -= g_angle;
	glRotatef(g_angle2, 1, 0, 0);
	//glTranslatef(0.0f, 0.0f, -100.0f);
	glCallList(dlCannonBase);
	glPopMatrix();

	glPushMatrix();
	g_cannonObj.SetTransformMatrix();
	glCallList(dlCannon);

	glPopMatrix();

	DrawCannonBalls();
	DrawBalloons();

	/*glPushMatrix();
	glLoadIdentity();
	g_font.use();
	glColor3f(1.0f, 0.0f, 0.0f);
	g_font.glprintf(0.1, 0.0, "stuff");
	glPopMatrix();*/

	
	g_explosion.Draw();

	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(0.0f, 0.0f, -2.0f);
	if (g_gameover)
	{
		g_font.glprintf(-0.5f, 0.6f, "GAME OVER");
		g_font.glprintf(-0.5f, 0.5f, "SCORE = %d", g_finalScore);
		g_font.glprintf(-0.5f, 0.4f, "NUMBER OF HITS = %d", g_finalNumHits);
		g_font.glprintf(-0.5f, 0.3f, "NUMBER OF EXPLODED = %d", g_finalNumExploded);

	}
	else
	{
		g_font.glprintf(-1.05, 0.75, "Timer = %d", (int)g_timer, g_numExploded);
	}
	glPopMatrix();
	glutSwapBuffers();
}

void DrawPlane()
{
	glBindTexture(GL_TEXTURE_2D, g_grassTexture);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 20.0f);
	glVertex3f(-4000.0f, -10.0f, -4000.0f);
	glTexCoord2f(20.0f, 20.0f);
	glVertex3f( 4000.0f, -10.0f, -400.0f);
	glTexCoord2f(20.0f, 0.0f);
	glVertex3f( 4000.0f, -10.0f,  4000.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-4000.0f, -10.0f,  4000.0f);
	glEnd();
}

void InitCannonBalls()
{
	ZeroMemory(g_cannonBalls, sizeof(CannonBall) * MAX_CANNONBALLS);
}

void FireCannonBall()
{
	for (int i = 0;  i < MAX_CANNONBALLS; i++)
	{
		if (g_cannonBalls[i].active == false)
		{
			Vector3 cannonPos = g_cannonObj.GetPosition();
			Vector3 cannonLook = g_cannonObj.GetLook();
			Vector3 cannonUp = g_cannonObj.GetUp();

			g_cannonBalls[i].pos.x = cannonPos.x + (cannonUp.x * 4) + (cannonLook.x * 8);
			g_cannonBalls[i].pos.y = cannonPos.y + (cannonUp.y * 4) + (cannonLook.y * 8);
			g_cannonBalls[i].pos.z = cannonPos.z + (cannonUp.z * 4) + (cannonLook.z * 8);

			g_cannonBalls[i].vel.x = cannonLook.x * 1000;
			g_cannonBalls[i].vel.y = cannonLook.y * 1000;
			g_cannonBalls[i].vel.z = cannonLook.z * 1000;

			g_cannonBalls[i].active = true;

			break;
		}
	}
}

void UpdateCannonBalls(float dt)
{
	for (int i = 0; i < MAX_CANNONBALLS; i++)
	{
		if (g_cannonBalls[i].active)
		{
			g_cannonBalls[i].pos.x += g_cannonBalls[i].vel.x * dt;
			g_cannonBalls[i].pos.y += g_cannonBalls[i].vel.y * dt;
			g_cannonBalls[i].pos.z += g_cannonBalls[i].vel.z * dt;

			for (int j = 0; j < MAX_BALLOONS; j++)
			{
				if (g_balloons[j].active && g_balloons[j].respawnTimer < 0)
				{
					Sphere a, b;
					a.c = g_cannonBalls[i].pos;
					a.r = 2.5f; 
					b.c = g_balloons[j].pos;
					b.r = 20.0f;

					if (TestSphereSphere(a, b))
					{
						g_balloons[j].hit = true;
						g_balloons[j].active = false;
						g_cannonBalls[i].active = false;
						GLfloat color[] = {1.0f, 1.0f, 1.0f};
						g_explosion.StartExplosion(g_balloons[j].pos, 10, color);
						g_numHits++;
						
					}
				}
			}

			g_cannonBalls[i].vel.y -= 200.0f * dt;
			if (g_cannonBalls[i].pos.y < -10.0f) 
			{
				GLfloat color[] = {1.0f, 1.0f, 1.0f};
				g_explosion.StartExplosion(g_cannonBalls[i].pos, 6, color);
				g_cannonBalls[i].active = false;
			}
		}
	}
}

void DrawCannonBalls()
{
	glColor3f(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < MAX_CANNONBALLS; i++)
	{
		if (g_cannonBalls[i].active)
		{
	
			glPushMatrix();
			glTranslatef(g_cannonBalls[i].pos.x, g_cannonBalls[i].pos.y, g_cannonBalls[i].pos.z);
			//gluSphere(quadric, 2.5f, 20, 20);
			glutSolidSphere(2.5f, 20, 20);
			glPopMatrix();
		}
	}

}


void InitBalloons()
{
	ZeroMemory(g_balloons, sizeof(MAX_BALLOONS));
}

void UpdateBalloons(float dt)
{
	for (int i = 0; i < MAX_BALLOONS; i++)
	{
		if (g_balloons[i].active == false)
		{
			g_balloons[i].active = true;
			g_balloons[i].pos = Vector3(rand()%2001-1000, rand()%200+100, rand()%2001-1000);//rand()%1000-500, rand()%200+200, rand()%1000-500);
			g_balloons[i].respawnTimer = rand()%5+5;
			g_balloons[i].lifeTimer = rand()%10 + 20;
		}
		else
		{
			if (g_balloons[i].respawnTimer > 0)
			{
				g_balloons[i].respawnTimer -= dt;
			}
			else
			{
				g_balloons[i].lifeTimer -= dt;
				if (g_balloons[i].lifeTimer < 0)
				{
					GLfloat color[] = {1.0f, 0.0f, 0.0f};
					g_explosion.StartExplosion(g_balloons[i].pos, 10, color);
					g_balloons[i].active = false;
					g_numExploded++;
					
				}
			}
		}
	}
}

void DrawBalloons()
{
	glEnable(GL_LIGHTING);
	GLfloat matAmbient[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat matDiffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat matSpecular[] = {0.4f, 0.4f, 0.4, 1.0f};
	GLfloat highShininess[] = {50.0};
	GLfloat matEmission[] = {0.5f, 0.5f, 0.5f, 0.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, highShininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
	for (int i = 0; i < MAX_BALLOONS; i++)
	{
		if (g_balloons[i].active)
		{
			
			if (g_balloons[i].respawnTimer > 0)
			{
				GLfloat matDiffuse[] = {0.5f, 0.5f, 0.5f, 0.4f};
				GLfloat matEmission[] = {0.5f, 0.5f, 0.5f, 0.0f};
				glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
				glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
			}
			else if (g_balloons[i].lifeTimer < 5)
			{
				GLfloat matEmission[] = {1.0f, 0.0f, 0.0f, 0.0f};
				glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
			}
			else if (g_balloons[i].lifeTimer < 10)
			{
				GLfloat matEmission[] = {1.0f, 1.0f, 0.0f, 0.0f};
				glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
			}
			else
			{
				GLfloat matEmission[] = {0.0f, 1.0f, 0.0f, 0.0};
				glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
			}

			glPushMatrix();
			glTranslatef(g_balloons[i].pos.x, g_balloons[i].pos.y, g_balloons[i].pos.z);
			glutSolidSphere(20.0f, 20, 20);
			glTranslatef(0.0f, 30.0f, 0.0f);
			g_font.glprintf(0.0, 0.0, "%i", g_balloons[i].respawnTimer > 0 ? (int)g_balloons[i].respawnTimer : (int)g_balloons[i].lifeTimer);
			glPopMatrix();
		}
	}
	glDisable(GL_LIGHTING);
}
