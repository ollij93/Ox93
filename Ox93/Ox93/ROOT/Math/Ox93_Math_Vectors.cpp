#include "Ox93_Math_Vectors.h"

///////////////////////////////////////////////////
// V E C T O R  2
///////////////////////////////////////////////////

Ox93_Vector_2::Ox93_Vector_2()
: x(0)
, y(0)
{
}

Ox93_Vector_2::Ox93_Vector_2(float fX, float fY)
: x(fX)
, y(fY)
{
}

///////////////////////////////////////////////////
// V E C T O R  3
///////////////////////////////////////////////////

Ox93_Vector_3::Ox93_Vector_3()
: x(0)
, y(0)
, z(0)
{
}

Ox93_Vector_3::Ox93_Vector_3(float fX, float fY, float fZ)
: x(fX)
, y(fY)
, z(fZ)
{
}

void Ox93_Vector_3::Normalize()
{
	float fMagnitude = sqrt(x*x + y*y + z*z);

	x /= fMagnitude;
	y /= fMagnitude;
	z /= fMagnitude;
}

Ox93_Vector_3 Ox93_Vector_3::operator^(Ox93_Vector_3 xVector) const
{
	Ox93_Vector_3 xResultVector;

	xResultVector.x = y * xVector.z - z * xVector.y;
	xResultVector.y = z * xVector.x - x * xVector.z;
	xResultVector.z = x * xVector.y - y * xVector.x;

	return xResultVector;
}
