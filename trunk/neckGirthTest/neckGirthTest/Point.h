#pragma once

class Point
{
public:
	int x,y,z;
	int getZ() { return z; }
	void setZ(int val) { z = val; }
	int getX() { return x; }
	void setX(int val) { x = val; }
	int getY() { return y; }
	void setY(int val) { y = val; }
	Point(int x,int y,int z)
	{
		setX(x);
		setY(y);
		setZ(z);
	}
	~Point(void);

};
