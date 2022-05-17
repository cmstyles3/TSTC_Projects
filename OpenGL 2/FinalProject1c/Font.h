#ifndef _FONT__H_
#define _FONT__H_

#include <gl/glut.h>

struct Font
{
	unsigned int texture;
	bool init();
	void use();
	void glprintf(double,double,const char *,...);
};

#endif