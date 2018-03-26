#pragma once

#ifndef OX93_ENTITYCLASSIFICATION_H__
#define OX93_ENTITYCLASSIFICATION_H__

// Includes...
#include <vector>
#include "Types/Ox93_Hash.h"

// Forward Declarations...
class Ox93_Entity;

// DO NOT REORDER THIS ENUM
enum Ox93_EntityClassification
{
	OX93_CLASS_PHYSICALOBJECT = 0,
	OX93_CLASS_UNUSED = 1,
	OX93_CLASS_CHARACTER = 2,
	OX93_CLASS_SELECTIONOBJECT = 3,
	OX93_CLASS_LIGHT = 4,
	OX93_CLASS_CAMERA = 5,

	OX93_CLASS_MAX,
};

struct Ox93_EntityRegistry_Entry
{
	u_int m_uClassification;
	Ox93_Entity*(*m_pfnCreate)();
};


class Ox93_EntityRegistry
{
	typedef Ox93_Entity* (*pfnCreateFunction)();

public:
	static void RegisterEntityClasses();
	static Ox93_Entity* CreateEntityFromClassification(u_int uClassification);

protected:
	static void RegisterEntity(u_int uClassification, pfnCreateFunction pfnCreate);

private:
	static std::vector<Ox93_EntityRegistry_Entry> s_xRegistry;
};


#endif OX93_ENTITYCLASSIFICATION_H__