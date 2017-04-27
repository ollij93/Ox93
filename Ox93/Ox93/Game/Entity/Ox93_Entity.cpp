// Includes...
#include "Ox93_Entity.h"
#include "Game/LoadSave/ChunkStream/Ox93_ChunkStream.h"
#include "ROOT/Specification/Ox93_Specification.h"

// Statics...
u_long Ox93_Entity::s_ulPreviousUpdateTime = 0;
u_long Ox93_Entity::s_ulCurrentUpdateTime = 0;
std::list<Ox93_Entity*> Ox93_Entity::s_lpxEntityList;

Ox93_Entity::Ox93_Entity(u_int uClassification)
: m_xPosition(Ox93_Math::ZeroVector3)
, m_xVelocity(Ox93_Math::ZeroVector3)
, m_xOrientation(Ox93_Math::IdentityMatrix3x3)
, m_uClassification(uClassification)
, m_uSpecificationHash(OX93_HASH_UNSET)
{
	AddToEntityList(this);
}

Ox93_Entity::~Ox93_Entity()
{
	RemoveFromEntityList(this);
}

void Ox93_Entity::ProcessUpdates()
{
	s_ulCurrentUpdateTime = timeGetTime();
	std::list<Ox93_Entity*>::const_iterator xIter;
	for (xIter = s_lpxEntityList.begin(); xIter != s_lpxEntityList.end(); ++xIter)
	{
		Ox93_Entity* pxEntity = *xIter;
		if (pxEntity)
		{
			pxEntity->Update();
		}
	}

	s_ulPreviousUpdateTime = s_ulCurrentUpdateTime;
}

void Ox93_Entity::ReadFromChunkStream(const Ox93_ChunkStream& xChunkStream)
{
	xChunkStream >> m_xPosition.x;
	xChunkStream >> m_xPosition.y;
	xChunkStream >> m_xPosition.z;

	xChunkStream >> m_xOrientation.e00;
	xChunkStream >> m_xOrientation.e01;
	xChunkStream >> m_xOrientation.e02;
	xChunkStream >> m_xOrientation.e10;
	xChunkStream >> m_xOrientation.e11;
	xChunkStream >> m_xOrientation.e12;
	xChunkStream >> m_xOrientation.e20;
	xChunkStream >> m_xOrientation.e21;
	xChunkStream >> m_xOrientation.e22;
}

void Ox93_Entity::WriteToChunkStream(Ox93_ChunkStream& xChunkStream) const
{
	xChunkStream << m_xPosition.x;
	xChunkStream << m_xPosition.y;
	xChunkStream << m_xPosition.z;

	xChunkStream << m_xOrientation.e00;
	xChunkStream << m_xOrientation.e01;
	xChunkStream << m_xOrientation.e02;
	xChunkStream << m_xOrientation.e10;
	xChunkStream << m_xOrientation.e11;
	xChunkStream << m_xOrientation.e12;
	xChunkStream << m_xOrientation.e20;
	xChunkStream << m_xOrientation.e21;
	xChunkStream << m_xOrientation.e22;
}

void Ox93_Entity::InitFromSpecification(const Ox93_Specification* pxSpecification)
{
	if (pxSpecification)
	{
		m_uSpecificationHash = pxSpecification->GetHashID();
	}
}

void Ox93_Entity::Update()
{
	m_xPosition += m_xVelocity * (GetRunningUpdateTime() / 1000.f);
}

void Ox93_Entity::ShutdownAll()
{
	while (s_lpxEntityList.size() > 0)
	{
		Ox93_Entity* pxEntity = s_lpxEntityList.front();
		if (pxEntity)
		{
			delete pxEntity;
			pxEntity = nullptr;
		}
	}
}