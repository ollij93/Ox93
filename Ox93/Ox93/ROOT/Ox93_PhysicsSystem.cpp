// Includes...
#include "Ox93_PhysicsSystem.h"
#include "Game/Entity/PhysicalObject/Ox93_PhysicalObject.h"

// Global Variables...
static const float fTIMESTEPMIN = 0.2f;
static const rp3d::Vector3 xGRAVITY(0.0, -9.81f, 0.0);

// Static variables...
Ox93_PhysicsSystem* Ox93_PhysicsSystem::s_pxThis = nullptr;

Ox93_PhysicsSystem::Ox93_PhysicsSystem()
: xWorld(xGRAVITY)
{
}

bool Ox93_PhysicsSystem::Create()
{
	Ox93_Assert(!s_pxThis, "Ox93_PhysicsSystem is already created.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_PhysicsSystem;
	return s_pxThis->Init();
}

void Ox93_PhysicsSystem::Destroy()
{
	if (!s_pxThis) { return; }

	s_pxThis->Shutdown();
	delete s_pxThis;
	s_pxThis = nullptr;
}

void Ox93_PhysicsSystem::AddEntity(Ox93_Entity* pxEntity)
{
	if (!s_pxThis) { return; }

	rp3d::Transform xTransform((rp3d::Vector3)pxEntity->GetPosition(), (rp3d::Matrix3x3)pxEntity->GetOrientation());
	rp3d::RigidBody* pxRigidBody = s_pxThis->xWorld.createRigidBody(xTransform);
	rp3d::BoxShape* xBoxShape = new rp3d::BoxShape(Ox93_Vector_3(0.5f, 0.5f, 0.5f));
	pxRigidBody->addCollisionShape(xBoxShape, rp3d::Transform::identity(), 1.f);
	pxEntity->SetRigidBody(pxRigidBody);
}

bool Ox93_PhysicsSystem::Init()
{
	return true;
}

void Ox93_PhysicsSystem::Shutdown()
{
}

void Ox93_PhysicsSystem::Update(float fTime)
{
	if (!s_pxThis) { return; }

	while (fTime >= fTIMESTEPMIN)
	{
		s_pxThis->xWorld.update(fTIMESTEPMIN);
		fTime -= fTIMESTEPMIN;
	}
	s_pxThis->xWorld.update(fTime);
}
