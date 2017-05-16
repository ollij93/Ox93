#pragma once

#ifndef __OX93_PHYSICSSYSTEM_H__
#define __OX93_PHYSICSSYSTEM_H__

// Includes...
#include "Ox93_Core.h"
#include "ReactPhysics3D/reactphysics3d.h"

// Forward Declarations...
class Ox93_Entity;

// Enums...
enum Ox93_CollisionGroup {
	OX93_NONE_COLLIDING = 0,
	OX93_DEFAULT_COLLIDING = 1,
};

class Ox93_PhysicsSystem
{
public:
	static bool Create();
	static void Destroy();

	static void Update(float fTime);

	static void AddEntity(Ox93_Entity* pxEntity);
protected:
	Ox93_PhysicsSystem();
	~Ox93_PhysicsSystem() {};
	bool Init();
	void Shutdown();

private:
	rp3d::DynamicsWorld xWorld;

	static Ox93_PhysicsSystem* s_pxThis;
};

#endif // __OX93_PHYSICSSYSTEM_H__
