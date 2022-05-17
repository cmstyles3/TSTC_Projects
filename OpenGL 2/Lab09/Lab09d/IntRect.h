class IntRect  //aligned rectangle with integer coordinates, used for viewport
{
public:
	int l, r, b, t;
	IntRect() {l = 0; r=100; b=0; t=100;} //constructors
	IntRect(int left, int right, int bottom, int top)
		{l = left; r=right; b=bottom; t=top;}
	void set(int left, int right, int bottom, int top)
	{
		l=left; r=right; b=bottom; t=top;
	}
	void draw(void); //draw this rectangle using OpenGL
	int getL(void)
	{
		return l;
	}
	int getR(void)
	{
		return r;
	}
	int getT(void)
	{
		return t;
	}
	int getB(void)
	{
		return b;
	}

private:
	int lx, rx, bx, tx;
};