class Vector3   
{
public:
	double x, y, z;
	Vector3( ) 			//constructor 1
	{
		x = y = z = 0.0;
	}
	Vector3(double xx, double yy, double zz)	 //constructor 2
	{
		x=xx;
		y=yy;
		z=zz;
	}
	Vector3(Vector3& v)		 //constructor 3
	{
		x=v.x;
		y=v.y;
		z=v.z;
	}
	void set(double dx, double dy, double dz)
	{
		x=dx;
		y=dy;
		z=dz;
	}
	void set(Vector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	void flip( )			// reverse this vector
	{
		x = -x;
		y = -y;
		z = -z;
	}
	void setDiff(Point3& a, Point3& b)	// vector of difference a - b
	{
		x = a.x - b.x;
		y = a.y - b.y;
		z = a.z - b.z;
	}
	void normalize( );		// adjust this vector to unit length
	Vector3 cross(Vector3 b);	// return this cross b
	float dot(Vector3 b);		// return this dotted with b
};

Vector3 Vector3::cross(Vector3 b)
{
	Vector3 c(y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x);
	return c;
}

float Vector3::dot(Vector3 b)
{
	return x*b.x + y*b.y + z*b.z;
}

void Vector3::normalize()
{
	double sizeSq = x*x + y*y + z*z;
	if(sizeSq < 0.0000001)
	{
//		cerr << "\nnormalize() sees vector (0,0,0)!";
		return;
	}
	double scaleFactor = 1.0/(float)sqrt(sizeSq);
	x *= scaleFactor; y *= scaleFactor; z *= scaleFactor;
}

