#pragma once

#ifndef OX93_SAVESYSTEM_H__
#define OX93_SAVESYSTEM_H__

// Includes...
#include "Ox93_Core.h"

// Forward Declarations...
class Ox93_Inventory;

class Ox93_SaveSystem
{
public:
	static bool Create();
	static void Destroy();

	static void SavePlayerSettings(const WCHAR* pwszFilename);
	static void SaveCurrentWorldToFile(const WCHAR* pwszFilename);
	static void SaveInventoryToFile(Ox93_Inventory* pxInv, const WCHAR* pwszFilename);

protected:
	bool Init();
	void Shutdown();

	Ox93_SaveSystem();
	~Ox93_SaveSystem();

private:
	static Ox93_SaveSystem* s_pxThis;
};

#endif // OX93_SAVESYSTEM_H__

