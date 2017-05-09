// Includes...
#include "Ox93_AABB.h"
#include "ROOT/Specification/Ox93_Specification.h"

Ox93_AABB::Ox93_AABB(float fHalfWidth, float fHalfHeight, float fHalfDepth)
: PARENT(OX93_AABB_COLLIDER)
, m_fHalfWidth(fHalfWidth)
, m_fHalfHeight(fHalfHeight)
, m_fHalfDepth(fHalfDepth)
{
}

Ox93_AABB::~Ox93_AABB()
{
}

Ox93_CollisionObject* Ox93_AABB::InitFromSpecification(const Ox93_Specification* pxSpecification)
{
	Ox93_CollisionObject* pxCollider = nullptr;
	if (pxSpecification)
	{
		float fHalfWidth = pxSpecification->GetValueAsFloat(uOX93_SPEC_PARAM_WIDTH, 0.5f);
		float fHalfHeight = pxSpecification->GetValueAsFloat(uOX93_SPEC_PARAM_HEIGHT, 0.5f);
		float fHalfDepth = pxSpecification->GetValueAsFloat(uOX93_SPEC_PARAM_DEPTH, 0.5f);
		pxCollider = new Ox93_AABB(fHalfWidth, fHalfHeight, fHalfDepth);
	}

	return pxCollider;
}
