#ifndef __OX93_MATH_VECTORS_H__
#define __OX93_MATH_VECTORS_H__

// Includes...
#include "Ox93_Core.h"

class Ox93_Vector_2
{
public:
	Ox93_Vector_2();
	Ox93_Vector_2(float fX, float fY);
	Ox93_Vector_2(const Ox93_Vector_2& xVector) { x = xVector.x; y = xVector.y; }
	~Ox93_Vector_2() {}

	Ox93_Vector_2 operator +(const Ox93_Vector_2& xVector) const { return Ox93_Vector_2(x + xVector.x, y + xVector.y); }
	Ox93_Vector_2 operator -(const Ox93_Vector_2& xVector) const { return Ox93_Vector_2(x - xVector.x, y - xVector.y); }

	float x;
	float y;
};

class Ox93_Vector_3
{
public:
	Ox93_Vector_3();
	Ox93_Vector_3(float fX, float fY, float fZ);
	Ox93_Vector_3(const Ox93_Vector_3& xVector) { x = xVector.x; y = xVector.y; z = xVector.z; }
	~Ox93_Vector_3() {}

	operator DirectX::XMVECTOR() const { return DirectX::XMVectorSet( x, y, z, 0 ); }

	float operator [](char i) const { switch (i) { case 0: return x;  case 1: return y; case 2: return z; default: Ox93_Assert(false, "Access Violation for Ox93_Vector_3"); return 0.f; } }

	bool operator !=(Ox93_Vector_3 xVector) const { return (x != xVector.x) || (y != xVector.y) || (z != xVector.z); }

	void operator +=(Ox93_Vector_3 xVector) { x += xVector.x; y += xVector.y; z += xVector.z; }
	void operator -=(Ox93_Vector_3 xVector) { x -= xVector.x; y -= xVector.y; z -= xVector.z; }
	void operator *=(float f) { x *= f; y *= f; z *= f; }

	Ox93_Vector_3 operator +(Ox93_Vector_3 xVector) const { return Ox93_Vector_3(x + xVector.x, y + xVector.y, z + xVector.z); }
	Ox93_Vector_3 operator -(Ox93_Vector_3 xVector) const { return Ox93_Vector_3(x - xVector.x, y - xVector.y, z - xVector.z); }
	Ox93_Vector_3 operator *(float f) const { return Ox93_Vector_3(x * f, y * f, z * f); }
	Ox93_Vector_3 operator /(float f) const { return Ox93_Vector_3(x / f, y / f, z / f); }

	void Normalize();
	float Length() const { return sqrt(x*x + y*y + z*z); }

	float operator*(Ox93_Vector_3 xVector) const { return (x * xVector.x + y * xVector.y + z * xVector.z); }
	Ox93_Vector_3 operator^(Ox93_Vector_3 xVector) const;

	float x;
	float y;
	float z;
};

#endif __OX93_MATH_VECTORS_H__
