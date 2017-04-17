#ifndef __OX93_MATRICES_H__
#define __OX93_MATRICES_H__

// Includes...
#include "Ox93_Core.h"

class Ox93_Matrix3x3
{
public:
	Ox93_Matrix3x3() {};
	Ox93_Matrix3x3(const Ox93_Matrix3x3& xMatrix) { e00 = xMatrix.e00; e01 = xMatrix.e01; e02 = xMatrix.e02;
													e10 = xMatrix.e10; e11 = xMatrix.e11; e12 = xMatrix.e12;
													e20 = xMatrix.e20; e21 = xMatrix.e21; e22 = xMatrix.e22; };
	Ox93_Matrix3x3(float f00, float f01, float f02, float f10, float f11, float f12, float f20, float f21, float f22) {
		e00 = f00; e01 = f01; e02 = f02;
		e10 = f10; e11 = f11; e12 = f12;
		e20 = f20; e21 = f21; e22 = f22;
	};
	~Ox93_Matrix3x3() {};

	Ox93_Matrix3x3 operator*(const Ox93_Matrix3x3& xMatrix) const;

	void RotateLocalX(float fDeltaTheta);
	void RotateWorldY(float fDeltaPhi);

	float e00, e01, e02;
	float e10, e11, e12;
	float e20, e21, e22;
};

#endif // ifndef __OX93_MATRICES_H__
