#pragma once

#ifndef OX93_AABB_H__
#define OX93_AABB_H__

// Includes...
#include "ROOT/Physics/Ox93_CollisionObject.h"

class Ox93_AABB : public Ox93_CollisionObject
{
public:
	Ox93_AABB(float fHalfWidth, float fHalfHeight, float fHalfDepth);
	~Ox93_AABB();

	float operator [](char i) const { switch (i) { case 0: return m_fHalfWidth; case 1: return m_fHalfHeight; case 2: return m_fHalfDepth; default: Ox93_Assert(false, "Access violation in Ox93_AABB"); return 0.f; } }

	static Ox93_CollisionObject* InitFromSpecification(const Ox93_Specification* pxSpecification);

	// Getters & Setters...
	float GetHalfWidth() const { return m_fHalfWidth; }
	float GetHalfHeight() const { return m_fHalfHeight; }
	float GetHalfDepth() const { return m_fHalfDepth; }

private:
	float m_fHalfWidth;
	float m_fHalfHeight;
	float m_fHalfDepth;

	typedef Ox93_CollisionObject PARENT;
};

#endif // OX93_AABB_H__
