#include <windows.h>
#include <stdio.h>
#include <gl/glut.h>

#define BUFSIZE 512

struct Board
{
	bool hasTri;
	bool selected;
	int color;
};

int color[16] = {0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0};
Board board[4][4];
int numTries = 0;
int numTris = 0;

void Init()
{
	srand(GetTickCount());
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ZeroMemory(&board, sizeof(Board) * 16);
	for (int i = 0; i < 3; i++)
	{
		int col = rand() % 4;
		int row = rand() % 4;
		while (board[row][col].hasTri)
		{
			col = rand() % 4;
			row = rand() % 4;
		}
		board[row][col].hasTri = true;
	}
	
	//Assign Colors
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			board[i][j].color = color[i * 4 + j];
		}
	}
}

void DrawSquares(GLenum mode)
{
	for (int i = 0; i < 4; i++)
	{
		if (mode == GL_SELECT)
			glLoadName(i);
		for (int j = 0; j < 4; j++)
		{
			if (mode == GL_SELECT)
				glPushName(j);
			if (board[i][j].color == 0)
				glColor3f(0.0f, 0.0f, 0.0f);
			else glColor3f(1.0f, 0.0f, 0.0f);
			glRecti(i, j, i + 1, j + 1);
			if (mode == GL_SELECT)
				glPopName();
		}
	}
}

void DrawTriangles()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (board[i][j].hasTri)
			{
				glPushMatrix();
				glTranslatef(i, j, 0);
				glColor4f(0.0f, 1.0f, 0.0f, board[i][j].selected ? 1.0f : 0.0f);
				glBegin(GL_TRIANGLES);
					glVertex3f(0.5f, 1.0f, 0.0f);
					glVertex3f(1.0f, 0.0f, 0.0f);
					glVertex3f(0.0f, 0.0f, 0.0f);
				glEnd();
				glPopMatrix();
			}
		}
	}
}
//
void ProcessHits(GLint hits, GLuint buffer[])
{ 
	GLuint ii, jj, names, *ptr;
	ptr = (GLuint *)buffer;
	for (int i = 0; i < hits; i++)
	{
		names = *ptr;
		ptr+=3;
		
		for (int j = 0; j < names; j++)
		{
			if (j == 0)
				ii = *ptr;
			else if (j == 1)
				jj = *ptr;
			ptr++;
		}
		if (board[ii][jj].selected == true)
			MessageBox(0, "This square has already been selected", "Message", 0);
		else	
		{
			board[ii][jj].selected = true;
			numTries++;
			if (board[ii][jj].hasTri)
			{
				numTris++;
				MessageBox(0, "You have found a triangle", "Message", 0);
				if (numTris >= 3) MessageBox(0, "You have found all the triangles", "Message", 0);
			}
			if (numTries >= 6) 
			{
				MessageBox(0, "You used all 6 tries, the program will exit now", "Message", 0);
				exit(0);
			}
		}
		//printf("%i & %i", ii, jj);
	}
}

void PickSquares(int button, int state, int x, int y)
{
	GLuint selectBuf[BUFSIZE];
	GLint hits;
	GLint viewport[4];

	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
		return;

	glGetIntegerv(GL_VIEWPORT, viewport);

	glSelectBuffer(BUFSIZE, selectBuf);
	(void)glRenderMode(GL_SELECT);

	glInitNames();
	glPushName(0);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y), 5.0, 5.0, viewport);
	gluOrtho2D(0.0, 4.0, 0.0, 4.0);
	DrawSquares(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glFlush();

	hits = glRenderMode(GL_RENDER);
	ProcessHits(hits, selectBuf);

	glutPostRedisplay();

}

void Display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	DrawSquares(GL_RENDER);
	DrawTriangles();
	glFlush();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 4.0, 0.0, 4.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Lab13c - Selection and Feedback");
	Init();
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutMouseFunc(PickSquares);
	glutMainLoop();
}