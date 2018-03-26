#pragma once

#ifndef OX93_MENUSYSTEM_H__
#define OX93_MENUSYSTEM_H__

// Includes...
#include <Windows.h>
#include <list>

#include "ROOT/Ox93_System.h"

// Forward Declarations...
class Ox93_Menu;

class Ox93_MenuSystem
{
public:
	static bool Create();
	static void Destroy();

	static void CloseOpenMenu();
	static void OpenPreviousMenu();
	static void ClearMenuStack() { if (s_pxThis) { s_pxThis->m_lpwszMenuStack.clear(); } }

	static void OpenTitleMenu()		{ OpenMenu(L"Menus/Title.mnu"); Ox93_System::SetGameMode(OX93_GAMEMODE_MENU); }
	static void OpenOptionsMenu()	{ OpenMenu(L"Menus/Options.mnu"); }
	static void OpenHUD()			{ OpenMenu(L"Menus/HUD.mnu", true); }
	static void OpenPauseMenu()		{ OpenMenu(L"Menus/Pause.mnu"); }
	static void OpenInventoryMenu() { OpenMenu(L"Menus/Inventory.mnu"); }

	static void HandleInput();
	static void UpdateMenus();

protected:
	Ox93_MenuSystem();
	~Ox93_MenuSystem();

	static void OpenMenu(const WCHAR* pwszFilename, bool bDontStack = false);
	void InitMenu(const WCHAR* pwszFilename);

private:
	Ox93_Menu* m_pxOpenMenu;
	const WCHAR* m_pwszQueuedMenu;
	bool m_bDontStackQueued;
	bool m_bCloseMenu;
	
	std::list<const WCHAR*> m_lpwszMenuStack;

	static Ox93_MenuSystem* s_pxThis;
};

#endif // OX93_MENUSYSTEM_H__

