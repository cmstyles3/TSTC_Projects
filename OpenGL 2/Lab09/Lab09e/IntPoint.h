class IntPoint
{
	public:
		int x, y;
		IntPoint( ) { x = y =  0; }
		IntPoint(IntPoint& p)  { x = p.x; y = p.y; }
		IntPoint(int xx, int yy)  { x = xx; y = yy; }
		void set(int xx, int yy)  { x = xx; y = yy; }
};