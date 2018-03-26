// Includes...
#include "Ox93_AABB.h"
#include "ROOT/Math/Ox93_Math.h"
#include "ROOT/Math/Ox93_Math_Vectors.h"

Ox93_AABB::Ox93_AABB(float width, float height, float depth)
: m_xPosition(Ox93_Math::ZeroVector3)
, m_fWidth(width)
, m_fHeight(height)
, m_fDepth(depth)
, m_pfOnCollide(nullptr)
{
}

Ox93_AABB::~Ox93_AABB()
{
}

bool Ox93_AABB::TestCollide(Ox93_AABB xAABB1, Ox93_AABB xAABB2)
{
	bool bRetVal = true;

	bRetVal &= fabsf(xAABB1.m_xPosition.x - xAABB2.m_xPosition.x) * 2 > xAABB1.m_fWidth + xAABB2.m_fWidth;
	bRetVal &= fabsf(xAABB1.m_xPosition.y - xAABB2.m_xPosition.y) * 2 > xAABB1.m_fHeight + xAABB2.m_fHeight;
	bRetVal &= fabsf(xAABB1.m_xPosition.z - xAABB2.m_xPosition.z) * 2 > xAABB1.m_fDepth + xAABB2.m_fDepth;

	return bRetVal;
}
