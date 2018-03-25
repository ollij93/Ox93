// Includes...
#include "Ox93_Entity.h"
#include "Game/LoadSave/ChunkStream/Ox93_ChunkStream.h"
#include "ROOT/Specification/Ox93_Specification.h"

// Statics...
std::list<Ox93_Entity*> Ox93_Entity::s_lpxEntityList;

Ox93_Entity::Ox93_Entity(u_int uClassification)
: m_xPosition(Ox93_Math::ZeroVector3)
, m_xOrientation(Ox93_Math::IdentityMatrix3x3)
, m_uClassification(uClassification)
, m_uSpecificationHash(OX93_HASH_UNSET)
, m_eCollisionGroup(OX93_NONE_COLLIDING)
, m_pxRigidBody(nullptr)
, m_bLockPosition(false)
, m_bLockOrientation(false)
{
	AddToEntityList(this);
	Ox93_PhysicsSystem::AddEntity(this);
}

Ox93_Entity::~Ox93_Entity()
{
	RemoveFromEntityList(this);
}

void Ox93_Entity::ProcessUpdates(float fTime)
{
	std::list<Ox93_Entity*>::const_iterator xIter;
	for (xIter = s_lpxEntityList.begin(); xIter != s_lpxEntityList.end(); ++xIter)
	{
		Ox93_Entity* pxEntity = *xIter;
		if (pxEntity)
		{
			pxEntity->Update();
		}
	}
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

	// Ensure position and orientation setters are called to correctly set matrices
	SetPosition(m_xPosition);
	SetOrientation(m_xOrientation);
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
	if (m_pxRigidBody) {
		// Update position of rigidbody or of our entity
		if (m_bLockPosition) {
			m_pxRigidBody->setType(rp3d::STATIC);
			m_pxRigidBody->setTransform(rp3d::Transform(m_xPosition, m_pxRigidBody->getTransform().getOrientation()));
		} else {
			m_pxRigidBody->setType(rp3d::DYNAMIC);
			m_xPosition = m_pxRigidBody->getTransform().getPosition();
		}
		// Update orientation of rigidbody or of our entity
		if (m_bLockOrientation) {
			m_pxRigidBody->setTransform(rp3d::Transform(m_xPosition, m_xOrientation));
		} else {
			m_xOrientation = m_pxRigidBody->getTransform().getOrientation();
		}
	}
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

void Ox93_Entity::SetPosition(float fX, float fY, float fZ)
{
	SetPosition(Ox93_Vector_3(fX, fY, fZ));
}
void Ox93_Entity::SetPosition(Ox93_Vector_3 xPosition)
{
	Ox93_Assert(m_pxRigidBody, "Setting position of entity without a rigidbody");
	rp3d::Transform xTransform(xPosition, m_xOrientation);
	m_pxRigidBody->setTransform(xTransform);
	m_xPosition = xPosition;
}
void Ox93_Entity::SetOrientation(Ox93_Matrix3x3 xOrientation)
{
	Ox93_Assert(m_pxRigidBody, "Setting orientation of entity without a rigidbody");
	rp3d::Transform xTransform(m_xPosition, xOrientation);
	m_pxRigidBody->setTransform(xTransform);
	m_xOrientation= xOrientation;
}

Ox93_Vector_3 Ox93_Entity::GetVelocity() const
{
	Ox93_Assert(m_pxRigidBody, "Getting velocity of entity without a rigidbody");
	return m_pxRigidBody->getLinearVelocity();
}
void Ox93_Entity::AddVelocity(Ox93_Vector_3 xVelocity)
{
	Ox93_Assert(m_pxRigidBody, "Adding velocity of entity without a rigidbody");
	m_pxRigidBody->setLinearVelocity(m_pxRigidBody->getLinearVelocity() + xVelocity);
}
void Ox93_Entity::SetVelocity(Ox93_Vector_3 xVelocity)
{
	Ox93_Assert(m_pxRigidBody, "Setting velocity of entity without a rigidbody");
	m_pxRigidBody->setLinearVelocity(xVelocity);
}
