#pragma once

#ifndef OX93_LOADSYSTEM_H__
#define OX93_LOADSYSTEM_H__

// Includes...
#include "Ox93_Core.h"

// Forward Declarations...
class Ox93_Inventory;
namespace tinyxml2 { class XMLElement; }

class Ox93_LoadSystem
{
public:
	static bool Create();
	static void Destroy();

	// OJ - Temporary function to load the default save
	static void LoadDefault();

	static void LoadPlayerSettings(const WCHAR* pwszFilename);
	static void LoadInventoryFromFile(Ox93_Inventory* pxInv, const WCHAR* pwszFilename);

protected:
	bool Init();
	void Shutdown();
	void LoadEntitiesFromWorld(tinyxml2::XMLElement* pxWorldElement);

	Ox93_LoadSystem();
	~Ox93_LoadSystem();
private:
	static Ox93_LoadSystem* s_pxThis;
};

#endif // OX93_LOADSYSTEM_H__
