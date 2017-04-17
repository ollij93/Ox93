#ifndef __OX93_MATH_H__
#define __OX93_MATH_H__

// Includes...
#include "Ox93_Math_Vectors.h"
#include "Ox93_Matrices.h"

namespace Ox93_Math
{
	// C O N S T A N T S
	static const float PI       = 3.141592654f;
	static const float Sqrt2    = 1.414213562f;
	static const float Sqrt3    = 1.732050808f;
	static const float InvSqrt2 = 0.707106781f;
	static const float InvSqrt3 = 0.577350269f;

	// V E C T O R S
	static const Ox93_Vector_3 ZeroVector3( 0.f, 0.f, 0.f );

	// M A T R I C E S
	static const Ox93_Matrix3x3 IdentityMatrix3x3( 1.f, 0.f, 0.f,
													0.f, 1.f, 0.f,
													0.f, 0.f, 1.f );
}

#endif // ifndef __OX93_MATH_H__
