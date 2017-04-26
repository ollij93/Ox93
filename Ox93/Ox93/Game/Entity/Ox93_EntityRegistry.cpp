// Includes...
#include "Ox93_EntityRegistry.h"
#include "Game/Entity/PhysicalObject/Character/Ox93_Character.h"
#include "Game/Entity/PhysicalObject/Ox93_PhysicalObject.h"
#include "Game/World/Terrain/Ox93_Terrain.h"

// Statics...
std::vector<Ox93_EntityRegistry_Entry> Ox93_EntityRegistry::s_xRegistry;

// Functions...
void Ox93_EntityRegistry::RegisterEntityClasses()
{
	RegisterEntity(OX93_CLASS_PHYSICALOBJECT, Ox93_PhysicalObject::Create);
	RegisterEntity(OX93_CLASS_CHARACTER, Ox93_Character::Create);
	RegisterEntity(OX93_CLASS_LIGHT, Ox93_Light::Create);
}

void Ox93_EntityRegistry::RegisterEntity(u_int uClassification, pfnCreateFunction pfnCreate )
{
	for (u_int uEntry = 0; uEntry < s_xRegistry.size(); uEntry++)
	{
		if (s_xRegistry[uEntry].m_uClassification == uClassification)
		{
			s_xRegistry[uEntry].m_pfnCreate = pfnCreate;
			return;
		}
	}
	// OJ - If we get here then this is a new entry so append it too the vector
	Ox93_EntityRegistry_Entry xNewEntry;
	xNewEntry.m_pfnCreate = pfnCreate;
	xNewEntry.m_uClassification = uClassification;

	s_xRegistry.push_back(xNewEntry);
}

Ox93_Entity* Ox93_EntityRegistry::CreateEntityFromClassification(u_int uClassification)
{
	for (u_int uEntry = 0; uEntry < s_xRegistry.size(); uEntry++)
	{
		if (s_xRegistry[uEntry].m_uClassification == uClassification)
		{
			return s_xRegistry[uEntry].m_pfnCreate();
		}
	}

	return nullptr;
}