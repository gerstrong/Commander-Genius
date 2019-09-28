

#ifndef GsVec2D_H
#define GsVec2D_H

#include <cmath>
#include <initializer_list>

template<typename _T>
struct GsVec2D
{
    GsVec2D() {}

    GsVec2D(_T _x, _T _y) : x(_x), y(_y) {}

    template<typename _T2>
    GsVec2D (std::initializer_list<_T2> list)
    {
        const _T2* it=begin(list);

        if(it==end(list)) return;
        x = static_cast<_T>(*it);
        it++;
        if(it==end(list)) return;
        y = static_cast<_T>(*it);
        it++;
    }

    _T	x=0, y=0;


    // Methods
    template<typename _T2>
    GsVec2D(const _T2& other)
    {
        x = static_cast<_T>(other.x);
        y = static_cast<_T>(other.y);
    }

    float GetLength() const { return sqrtf(static_cast<float>(x*x) +
                                           static_cast<float>(y*y)); }

    _T GetLength2() const { return x*x + y*y; }
	
    float GetAngle() const { return static_cast<float>
                               (atan2(static_cast<float>(y),
                                      static_cast<float>(x))); }
	
    GsVec2D Normalize() { return *this/GetLength(); }

    _T Scalar(const GsVec2D& vec) const { return x*vec.x + y*vec.y; }

    GsVec2D orthogonal() const { return GsVec2D(y, -x); }

    _T Cross(const GsVec2D& oth) const { return x * oth.y - y * oth.x; }

    // Returns the directions of the vector like (1,-1) or (0,1)
    GsVec2D dir() const
    {
        const int dirx = (x!=0) ? x/abs(x) : 0;
        const int diry = (y!=0) ? y/abs(y) : 0;
        return GsVec2D(dirx, diry);
    }

    // Overloaded operations
    GsVec2D operator=(const _T value) {
        x = value; y = value;
        return *this;
    }


    GsVec2D operator*(const float scalar) const {
        return GsVec2D(x*scalar,y*scalar);
    }
    GsVec2D operator*(const int scalar) const {
        return GsVec2D(x*scalar,y*scalar);
    }

    GsVec2D operator/(const float scalar) const {
        return GsVec2D(x/scalar,y/scalar);
    }
    GsVec2D operator/(const int scalar) const {
        return GsVec2D(x/scalar,y/scalar);
    }
    GsVec2D operator+(const GsVec2D& vec) const {
        return GsVec2D(x+vec.x,y+vec.y);
    }
    GsVec2D operator-(const GsVec2D& vec) const {
        return GsVec2D(x-vec.x,y-vec.y);
    }
    GsVec2D operator<<(const unsigned int bits) const {
        return GsVec2D(x<<bits,y<<bits);
    }
    GsVec2D operator>>(const unsigned int bits) const {
        return GsVec2D(x>>bits,y>>bits);
    }
    GsVec2D operator-() const {
        return GsVec2D(-x,-y);
    }
    GsVec2D& operator+=(const GsVec2D& vec) {
        x+=vec.x; y+=vec.y;
        return *this;
    }
    GsVec2D& operator-=(const GsVec2D& vec) {
        x-=vec.x; y-=vec.y;
        return *this;
    }
    GsVec2D& operator*=(const float scalar) {
        x*=scalar; y*=scalar;
        return *this;
    }
    GsVec2D& operator*=(const int scalar) {
        x*=scalar; y*=scalar;
        return *this;
    }


    template<typename _T2>
    bool operator<(const GsVec2D<_T2> & op) const
    {
        return ((y == op.y && (x < op.x))
                || y < op.y);
    }

    template<typename _T2>
    bool operator>(const GsVec2D<_T2> & op) const
    {
        return(op < *this);
    }


    template<typename _T2>
    bool operator==(const GsVec2D<_T2> & op) const
    {
        return (x==op.x && y==op.y);
    }

    template<typename _T2>
    bool operator!=(const GsVec2D<_T2> & op) const
    {
        return (x!=static_cast<_T>(op.x) ||
                y!=static_cast<_T>(op.y));
    }

    template<typename _T2>
    bool operator<=(const GsVec2D<_T2> & op) const {
        return ((*this < op) || (*this == op));
    }

};


#endif  // GsVec2D_H
