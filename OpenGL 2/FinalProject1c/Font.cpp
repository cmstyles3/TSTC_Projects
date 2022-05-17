#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <gl/gl.h>

#include "font.h"
#include "bitmap.h"

#define FONT_SIZE 0.08f
#define FONT_DISTANCE 1.7001f

bool Font::init()
{
	char *filename = "font.bmp";
	BitmapFile texFile;
	if(!(texFile.load(filename)))
		return false;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texFile.w,texFile.h, 0, GL_RGBA,GL_UNSIGNED_BYTE, texFile.rgba_data);
	return true;
}

void Font::use()
{
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Font::glprintf(double xPos,double yPos,const char *fmt,...)
{
	char string[256];
	va_list ap;
	unsigned char temp;
	double x1,y1,x2,y2;
	unsigned int pos = 0;
	int r=0,c=0;
	
	if (fmt == NULL)
		return;
	
	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	va_end(ap);
		
	glRasterPos2f(xPos, yPos);

	GLvoid *font_style = GLUT_BITMAP_HELVETICA_18;
	for (int i = 0; i < strlen(string); i++)
	{
		glutBitmapCharacter(font_style, string[i]);
	}	

	/*for(pos=0;pos<strlen(string);pos++)
	{
		temp = string[pos];
		x1 = 32.0*(temp%16)+8.0;
		x2 = x1+16.0;
		y1 = 32.0*(temp/16);
		y2 = y1+32.0;
		
		glTexCoord2d(x1/512.0,y1/512.0); glVertex2d((xPos+pos)*FONT_SIZE*0.5,(yPos-1)*FONT_SIZE);
		glTexCoord2d(x2/512.0,y1/512.0); glVertex2d((xPos+(pos+1))*FONT_SIZE*0.5,(yPos-1)*FONT_SIZE);
		glTexCoord2d(x2/512.0,y2/512.0); glVertex2d((xPos+(pos+1))*FONT_SIZE*0.5,yPos*FONT_SIZE);
		glTexCoord2d(x1/512.0,y2/512.0); glVertex2d((xPos+pos)*FONT_SIZE*0.5,yPos*FONT_SIZE);
	}*/

}