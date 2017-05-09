// Includes...
#include "Ox93_CollisionObject.h"
#include "Game/Entity/PhysicalObject/Ox93_PhysicalObject.h"
#include "ROOT/Physics/Ox93_AABB.h"
#include "ROOT/Specification/Ox93_Specification.h"

// Statics...
std::list<Ox93_CollisionObject*> Ox93_CollisionObject::s_lpxColliderList;

Ox93_CollisionObject::Ox93_CollisionObject(Ox93_ColliderType eType)
: m_eType(eType)
{
	s_lpxColliderList.push_back(this);
}

Ox93_CollisionObject* Ox93_CollisionObject::CreateFromSpecification(const Ox93_Specification* pxSpecification)
{
	if (pxSpecification)
	{
		switch (pxSpecification->GetValueAsHash(uOX93_SPEC_PARAM_COLLIDER_TYPE, OX93_HASH_UNSET))
		{
		case uOX93_COLLIDER_HASH_AABB:
			return Ox93_AABB::InitFromSpecification(pxSpecification);
		}
	}
	return nullptr;
}

Ox93_CollisionResult Ox93_CollisionObject::TestCollide_AABB_AABB(const Ox93_CollisionObject* pxObject1, const Ox93_CollisionObject* pxObject2, float fDeltaTime)
{
	Ox93_CollisionResult xResult;
	xResult.bCollide = false;
	if (!pxObject1 || !pxObject2 || !pxObject1->m_pxEntity || !pxObject2->m_pxEntity) { return xResult; }

	const Ox93_AABB* pxAABB1 = static_cast<const Ox93_AABB*>(pxObject1);
	const Ox93_AABB* pxAABB2 = static_cast<const Ox93_AABB*>(pxObject2);

	const Ox93_Vector_3 xPos1 = pxAABB1->m_pxEntity->GetPosition();
	const Ox93_Vector_3 xVel1 = pxAABB1->m_pxEntity->GetVelocity();

	const Ox93_Vector_3 xPos2 = pxAABB2->m_pxEntity->GetPosition();
	const Ox93_Vector_3 xVel2 = pxAABB2->m_pxEntity->GetVelocity();

	const Ox93_Vector_3 xVelD = xVel1 - xVel2;

	const float fEpsilon = 0.0001f;

	if (xVelD.Length() > 1.f)
	{
		xResult.bCollide = false; // NO-OP
	}

	// Loop through the 3 dimensions
	for (char i = 0; i < 3; i++) {
		char cSign = (xPos2[i] > xPos1[i]) ? 1 : -1;

		// Time at which the facing 'i' plans are just touching
		const float fDeltaTimei = (xPos2[i] - xPos1[i] - cSign * ((*pxAABB1)[i] + (*pxAABB2)[i])) / xVelD[i];
		if (fDeltaTimei > fDeltaTime || fDeltaTimei < -fEpsilon) { continue; }

		// Object positions at this time
		const Ox93_Vector_3 xPos1Coll = xPos1 + xVel1 * fDeltaTimei;
		const Ox93_Vector_3 xPos2Coll = xPos2 + xVel2 * fDeltaTimei;

		// Check if other directions overlap
		const char j = (i + 1) % 3;
		const char k = (i + 1) % 3;
		if (xPos1Coll[j] - cSign * (*pxAABB1)[j] > xPos2Coll[j] + cSign * (*pxAABB2)[j]
			|| xPos1Coll[k] - cSign * (*pxAABB1)[k] > xPos2Coll[j] + cSign * (*pxAABB2)[k])
		{ continue; }

		// Objects touching at found time so construct the result
		xResult.bCollide = true;
		xResult.fTime = fDeltaTimei;
		//xResult.xPosition1 = xPos1Coll;
		//xResult.xPosition2 = xPos2Coll;
		break;
	}
	
	return xResult;
}
