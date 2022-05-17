#ifndef MAIN_H
// OPENGL FINAL PROJECT
// PROGRAMMED BY COLLIN MUNSON


#define MAIN_H

#include <windows.h>
#include <gl/glut.h>
#include <stdio.h>
#include <vector>

#include "Bitmap.h"
#include "Font.h"
#include "Camera.h"
#include "Cannon.h"
#include "CollisionDetection.h"
#include "Explosion.h"

#define SCREEN_WIDTH  800.0f
#define SCREEN_HEIGHT 600.0f

#define MAX_CANNONBALLS 50

#define PI 3.14159f

struct CannonBall
{
	bool active;
	Vector3 pos;
	Vector3 vel;
};

#define MAX_BALLOONS 20

struct Balloon
{
	bool active;
	Vector3 pos;
	bool hit;
	float lifeTimer;
	float respawnTimer;
};

void KeyDown(unsigned char key, int x, int y);
void KeyUp(unsigned char key, int x, int y);

void SpclKeyDown(int key, int x, int y);
void SpclKeyUp(int key, int x, int y);

void Init();
void Kill();

void Update(float dt);
void Display();
void Idle();

void DrawPlane();

void InitCannonBalls();
void FireCannonBall();
void UpdateCannonBalls(float dt);
void DrawCannonBalls();

void InitBalloons();
void UpdateBalloons(float dt);
void DrawBalloons();


#endif