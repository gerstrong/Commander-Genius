

#ifndef __Vector2D_H__
#define __Vector2D_H__

#include <cmath>

template<typename _T>
struct Vector2D
{
    Vector2D() : x(0), y(0) {}

    Vector2D(_T _x, _T _y) : x(_x), y(_y) {}

    _T	x, y;
	
	
	// Methods	
	template<typename _T2>
    Vector2D(const _T2& other)
    {
        x = (_T)other.x; y = (_T)other.y;
	}
	
	float GetLength() const { return sqrtf((float)x*x + (float)y*y); }
    _T GetLength2() const { return x*x + y*y; }
	
	float GetAngle() const { return (float)atan2((float)y,(float)x); }
	
    Vector2D Normalize() { return *this/GetLength(); }
	
    _T Scalar(const Vector2D& vec) const { return x*vec.x + y*vec.y; }
	
    Vector2D orthogonal() const { return Vector2D(y, -x); }
	
    _T Cross(const Vector2D& oth) const { return x * oth.y - y * oth.x; }
	
	// Returns the directions of the vector like (1,-1) or (0,1)
    Vector2D dir() const
	{
		const int dirx = (x!=0) ? x/abs(x) : 0;
		const int diry = (y!=0) ? y/abs(y) : 0;
        return Vector2D(dirx, diry);
	}

	// Overloads
    Vector2D operator*(const float scalar) const {
        return Vector2D(x*scalar,y*scalar);
	}
    Vector2D operator*(const int scalar) const {
        return Vector2D(x*scalar,y*scalar);
	}

    Vector2D operator/(const float scalar) const {
        return Vector2D(x/scalar,y/scalar);
	}
    Vector2D operator/(const int scalar) const {
        return Vector2D(x/scalar,y/scalar);
	}
    Vector2D operator+(const Vector2D& vec) const {
        return Vector2D(x+vec.x,y+vec.y);
	}
    Vector2D operator-(const Vector2D& vec) const {
        return Vector2D(x-vec.x,y-vec.y);
	}
    Vector2D operator<<(const unsigned int bits) const {
        return Vector2D(x<<bits,y<<bits);
	}
    Vector2D operator>>(const unsigned int bits) const {
        return Vector2D(x>>bits,y>>bits);
	}
    Vector2D operator-() const {
        return Vector2D(-x,-y);
	}
    Vector2D& operator+=(const Vector2D& vec) {
		x+=vec.x; y+=vec.y;
		return *this;
	}
    Vector2D& operator-=(const Vector2D& vec) {
		x-=vec.x; y-=vec.y;
		return *this;
	}
    Vector2D& operator*=(const float scalar) {
		x*=scalar; y*=scalar;
		return *this;
	}
    Vector2D& operator*=(const int scalar) {
		x*=scalar; y*=scalar;
		return *this;
	}
	

	template<typename _T2>
    bool operator<(const Vector2D<_T2> & op) const {
		return ((y == op.y && (x < op.x))
				|| y < op.y);
	}
	
	template<typename _T2>
    bool operator==(const Vector2D<_T2> & op) const {
		return (x==op.x && y==op.y);
	}
	
	template<typename _T2>
    bool operator!=(const Vector2D<_T2> & op) const {
		return (x!=op.x || y!=op.y);
	}
	
	template<typename _T2>
    bool operator<=(const Vector2D<_T2> & op) const {
		return ((*this < op) || (*this == op));
	}
	
};


#endif  // __Vector2D_H__
