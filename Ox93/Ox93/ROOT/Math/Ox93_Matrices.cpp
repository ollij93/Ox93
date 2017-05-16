#include "Ox93_Matrices.h"

/////////////////////////
// M A T R I X   3 X 3
/////////////////////////

Ox93_Matrix3x3 Ox93_Matrix3x3::operator*(const Ox93_Matrix3x3& xMatrix) const
{
	Ox93_Matrix3x3 xResult;
	xResult.e00 = e00 *xMatrix.e00 + e01 * xMatrix.e10 + e02 * xMatrix.e20;
	xResult.e01 = e00 *xMatrix.e01 + e01 * xMatrix.e11 + e02 * xMatrix.e21;
	xResult.e02 = e00 *xMatrix.e02 + e01 * xMatrix.e12 + e02 * xMatrix.e22;

	xResult.e10 = e10 *xMatrix.e00 + e11 * xMatrix.e10 + e12 * xMatrix.e20;
	xResult.e11 = e10 *xMatrix.e01 + e11 * xMatrix.e11 + e12 * xMatrix.e21;
	xResult.e12 = e10 *xMatrix.e02 + e11 * xMatrix.e12 + e12 * xMatrix.e22;

	xResult.e20 = e20 *xMatrix.e00 + e21 * xMatrix.e10 + e22 * xMatrix.e20;
	xResult.e21 = e20 *xMatrix.e01 + e21 * xMatrix.e11 + e22 * xMatrix.e21;
	xResult.e22 = e20 *xMatrix.e02 + e21 * xMatrix.e12 + e22 * xMatrix.e22;

	return xResult;
}

Ox93_Matrix3x3 Ox93_Matrix3x3::operator=(const rp3d::Quaternion& q)
{
	float sqw = q.w*q.w;
	float sqx = q.x*q.x;
	float sqy = q.y*q.y;
	float sqz = q.z*q.z;

	// invs (inverse square length) is only required if quaternion is not already normalised
	float invs = 1 / (sqx + sqy + sqz + sqw);
	e00 = (sqx - sqy - sqz + sqw)*invs; // since sqw + sqx + sqy + sqz =1/invs*invs
	e11 = (-sqx + sqy - sqz + sqw)*invs;
	e22 = (-sqx - sqy + sqz + sqw)*invs;

	float tmp1 = q.x*q.y;
	float tmp2 = q.z*q.w;
	e10 = 2.0 * (tmp1 + tmp2)*invs;
	e01 = 2.0 * (tmp1 - tmp2)*invs;

	tmp1 = q.x*q.z;
	tmp2 = q.y*q.w;
	e20 = 2.0 * (tmp1 - tmp2)*invs;
	e02 = 2.0 * (tmp1 + tmp2)*invs;
	tmp1 = q.y*q.z;
	tmp2 = q.x*q.w;
	e21 = 2.0 * (tmp1 + tmp2)*invs;
	e12 = 2.0 * (tmp1 - tmp2)*invs;

	return *this;
}

Ox93_Matrix3x3 Ox93_Matrix3x3::RotateLocalX(float fDeltaTheta) const
{
	Ox93_Matrix3x3 xRotateMat = { 1.f, 0.f, 0.f,
									0.f, cos(fDeltaTheta), -sin(fDeltaTheta),
									0.f, sin(fDeltaTheta), cos(fDeltaTheta) };

	Ox93_Matrix3x3 xResult = xRotateMat * (*this);

	return xResult;
}

Ox93_Matrix3x3 Ox93_Matrix3x3::RotateWorldY(float fDeltaPhi) const
{
	Ox93_Matrix3x3 xRotateMat = { cos(fDeltaPhi), 0.f, sin(fDeltaPhi),
									0.f,  1.f, 0.f,
									-sin(fDeltaPhi), 0.f, cos(fDeltaPhi) };

	Ox93_Matrix3x3 xResult = (*this) * xRotateMat;

	return xResult;
}