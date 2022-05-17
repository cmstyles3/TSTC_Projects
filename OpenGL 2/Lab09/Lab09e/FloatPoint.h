class FloatPoint
{
	public:
		float x, y, z;
		FloatPoint( ) { x = y = z = 0.0; }
		FloatPoint(FloatPoint& p)  { x = p.x; y = p.y; z = p.z; }
		FloatPoint(float xx, float yy, float zz)  { x = xx; y = yy; z = zz; }
		void set(float xx, float yy, float zz)  { x = xx; y = yy; z = zz; }
};