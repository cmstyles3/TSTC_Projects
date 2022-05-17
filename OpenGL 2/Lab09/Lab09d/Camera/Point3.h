class Point3   //single point w/ double-precision coordinates
{
public:
	double x, y, z;
	Point3() {x = y = z=0.0f;} //constructor 1
	Point3(double xx, double yy, double zz) {x=xx; y=yy;z=zz;} //constructor 2
	void set(double xx, double yy, double zz) {x=xx; y=yy; z=zz;}
	double getX() {return x;}
	double getY() {return y;}
	double getZ() {return z;}
	void draw(void)
	{
		glBegin(GL_POINTS); //draw this point
		glVertex3f((GLfloat)x, (GLfloat)y, (GLfloat)z);
		glEnd();
	}
	void build4tuple(double v[])
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
		v[3] = 1.0f;
	}
};