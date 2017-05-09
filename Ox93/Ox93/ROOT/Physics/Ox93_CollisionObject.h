#pragma once

#ifndef OX93_COLLISIONOBJECT_H__
#define OX93_COLLISIONOBJECT_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Math/Ox93_Math.h"

// MACRO for registering the comparison function between two collision objects...
#define REGISTER_TEST_COLLIDE(__type1, __type2, __func) \
	do { \
		if (pxObject1->m_eType == __type1 && pxObject2->m_eType == __type2) { \
			return (__func) ? __func(pxObject1, pxObject2, fDeltaTime) : xResult ; \
		} else if (pxObject2->m_eType == __type1 && pxObject1->m_eType == __type2) { \
			return (__func) ? __func(pxObject2, pxObject1, fDeltaTime) : xResult ; \
		} \
	} while (false)

// Enum for collider types...
enum Ox93_ColliderType {
	OX93_AABB_COLLIDER = 0,
	OX93_INVALID_COLLIDERTYPE,
};

// Struct for collision return...
struct Ox93_CollisionResult {
	bool bCollide;
	float fTime;
	//Ox93_Vector_3 xPosition1;
	//Ox93_Vector_3 xPosition2;
};

// Forward Declarations...
class Ox93_Entity;
class Ox93_Specification;

// Abstract Class...
class Ox93_CollisionObject
{
public:
	Ox93_CollisionObject(Ox93_ColliderType eType);
	~Ox93_CollisionObject() {}

	// TODO-OJ:
	// virtual bool TestCollide(const Ox93_Raycast* pxRay) = 0;

	// Statics...
	// TODO-OJ : Decide which system is responsible for keeping track of these collisions
	static Ox93_CollisionObject* CreateFromSpecification(const Ox93_Specification* pxSpecification);

	// Specification registry...
	static Ox93_ColliderType GetColliderTypeFromHash(Ox93_Hash uHash)
	{
		switch (uHash)
		{
		case uOX93_COLLIDER_HASH_AABB:
			return OX93_AABB_COLLIDER;
		default:
			return OX93_INVALID_COLLIDERTYPE;
		}
	}

	// Registry of test collide functions...
	static Ox93_CollisionResult TestCollide(const Ox93_CollisionObject* pxObject1, const Ox93_CollisionObject* pxObject2, float fDeltaTime)
	{
		Ox93_CollisionResult xResult;
		xResult.bCollide = false;
		/* Don't set the other values as if bCollide is false then the other values should not be accessed */
		if (pxObject1 == nullptr || pxObject2 == nullptr) { return xResult; }

		REGISTER_TEST_COLLIDE(OX93_INVALID_COLLIDERTYPE, OX93_INVALID_COLLIDERTYPE, ((Ox93_TestCollide_Func)nullptr));
		REGISTER_TEST_COLLIDE(OX93_AABB_COLLIDER, OX93_AABB_COLLIDER, TestCollide_AABB_AABB);

		Ox93_Assert(false, "Missing collision type!"); // TODO-OJ
		return xResult;
	}

	// Getters & Setters...
	Ox93_ColliderType GetType() const { return m_eType; }
	void SetEntity(const Ox93_Entity* pxEntity) { m_pxEntity = pxEntity; }

	// Static Getters...
	static const std::list<Ox93_CollisionObject*>* GetList() { return &s_lpxColliderList; }

private:
	Ox93_ColliderType m_eType;
	const Ox93_Entity* m_pxEntity;

	// Statics...
	static std::list<Ox93_CollisionObject*> s_lpxColliderList;

	static Ox93_CollisionResult TestCollide_AABB_AABB(const Ox93_CollisionObject* pxObject1, const Ox93_CollisionObject* pxObject2, float fDeltaTime);

	typedef Ox93_CollisionResult (*Ox93_TestCollide_Func)(const Ox93_CollisionObject*, const Ox93_CollisionObject*, float fDeltaTime);
};

#endif // OX93_COLLISIONOBJECT_H__

