#pragma once

#ifndef __OX93_ENTITY_H__
#define __OX93_ENTITY_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Math/Ox93_Math.h"
#include "Game/Entity/Ox93_EntityRegistry.h"

// Forward Declarations
class Ox93_Vector_3;
class Ox93_Matrix3x3;
class Ox93_ChunkStream;
class Ox93_Specification;

class Ox93_Entity
{
public:
	Ox93_Entity(u_int uClassification);
	virtual ~Ox93_Entity();

	virtual void Update();
	static void ProcessUpdates();
	static void InitUpdates() { s_ulPreviousUpdateTime = timeGetTime(); }
	
	virtual void InitFromSpecification(const Ox93_Specification* pxSpecification);
	Ox93_Hash GetSpecificationHash() const { return m_uSpecificationHash; }

	u_int GetClassification() const { return m_uClassification; }

	virtual void ReadFromChunkStream(const Ox93_ChunkStream& xChunkStream);
	virtual void WriteToChunkStream(Ox93_ChunkStream& xChunkStream) const;

	/* Some entities need to update other quantities when their position is changed */
	virtual void SetPosition(float fX, float fY, float fZ) { m_xPosition = Ox93_Vector_3(fX,fY,fZ); }
	virtual void SetPosition(Ox93_Vector_3 xPosition) { m_xPosition = xPosition; }
	Ox93_Vector_3 GetPosition() const { return m_xPosition; }

	virtual void SetOrientation(Ox93_Matrix3x3 xOrientation) { m_xOrientation = xOrientation; }
	Ox93_Matrix3x3 GetOrientation() const { return m_xOrientation; }
	Ox93_Vector_3 GetDirection() const { return Ox93_Vector_3(m_xOrientation.e02, m_xOrientation.e12, m_xOrientation.e22); }

	static float GetRunningUpdateTime() { return (s_ulCurrentUpdateTime - s_ulPreviousUpdateTime < 200.f) ? s_ulCurrentUpdateTime - s_ulPreviousUpdateTime : 200.f; }

	static void AddToEntityList(Ox93_Entity* pxEntity) { s_lpxEntityList.push_back(pxEntity); }
	static void RemoveFromEntityList(Ox93_Entity* pxEntity) { s_lpxEntityList.remove(pxEntity); }

	static const std::list<Ox93_Entity*> GetList() { return s_lpxEntityList; }
	static void ShutdownAll();

protected:
	Ox93_Vector_3 m_xPosition;
	Ox93_Vector_3 m_xVelocity;
	Ox93_Matrix3x3 m_xOrientation;

private:
	u_int m_uClassification;
	u_int m_uSpecificationHash;

	static u_long s_ulCurrentUpdateTime;
	static u_long s_ulPreviousUpdateTime;

	static const u_int uVERSION = 0;

	static std::list<Ox93_Entity*> s_lpxEntityList;
};

#endif // ifndef __OX93_ENTITY_H__
