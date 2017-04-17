#pragma once

#ifndef OX93_AABB_H__
#define OX93_AABB_H__

// Includes...
#include "ROOT/Math/Ox93_Math_Vectors.h"

class Ox93_AABB
{
public:
	Ox93_AABB(float fWidth, float fHeight, float fDepth);
	~Ox93_AABB();

	static bool TestCollide(Ox93_AABB xAABB1, Ox93_AABB xAABB2);

	void SetPosition(Ox93_Vector_3 xPosition) { m_xPosition = xPosition; };
private:
	Ox93_Vector_3 m_xPosition;
	float m_fWidth;
	float m_fHeight;
	float m_fDepth;

	void(*m_pfOnCollide)(void);
};

#endif // OX93_AABB_H__
