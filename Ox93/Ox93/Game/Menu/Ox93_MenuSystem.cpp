// Includes...
#include "Ox93_MenuSystem.h"
#include "Game/LoadSave/Ox93_SaveSystem.h"
#include "Game/Menu/Ox93_Menu.h"
#include "ROOT/Ox93_InputSystem.h"

// Statics...
Ox93_MenuSystem* Ox93_MenuSystem::s_pxThis = nullptr;

Ox93_MenuSystem::Ox93_MenuSystem()
: m_pxOpenMenu(nullptr)
, m_pwszQueuedMenu(nullptr)
, m_bDontStackQueued(false)
, m_bCloseMenu(false)
, m_lpwszMenuStack()
{
}

Ox93_MenuSystem::~Ox93_MenuSystem()
{
}

bool Ox93_MenuSystem::Create()
{
	Ox93_Assert(!s_pxThis, "Attempting to recreate the Menu System.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_MenuSystem;
	return true;
}

void Ox93_MenuSystem::Destroy()
{
	if (!s_pxThis) { return; }

	delete s_pxThis;
	s_pxThis = nullptr;
}

void Ox93_MenuSystem::OpenMenu(const WCHAR* pwszFilename, bool bDontStack /* = false */)
{
	if (!s_pxThis) { return; }

	if (s_pxThis->m_pxOpenMenu)
	{
		CloseOpenMenu();
		s_pxThis->m_pwszQueuedMenu = pwszFilename;
		s_pxThis->m_bDontStackQueued = bDontStack;
	}
	else
	{
		if (s_pxThis->m_pwszQueuedMenu == nullptr)
		{
			s_pxThis->InitMenu(pwszFilename);
			s_pxThis->m_lpwszMenuStack.clear();
			if (!bDontStack) { s_pxThis->m_lpwszMenuStack.push_back(pwszFilename); }
		}
		else
		{
			Ox93_Assert(s_pxThis->m_pwszQueuedMenu == pwszFilename, "Opening a menu while a different one is queued. This will produce odd results!");

			s_pxThis->InitMenu(pwszFilename);
			if (s_pxThis->m_lpwszMenuStack.size() == 0 || s_pxThis->m_lpwszMenuStack.back() != pwszFilename)
			{
				if (!bDontStack) { s_pxThis->m_lpwszMenuStack.push_back(pwszFilename); }
			}
		}
	}
}

void Ox93_MenuSystem::InitMenu(const WCHAR* pwszFilename)
{
	Ox93_Assert(!m_pxOpenMenu, "Already have an active menu, this should have been shut down already.");

	m_pxOpenMenu = new Ox93_Menu;
	m_pxOpenMenu->LoadFromFile(pwszFilename);
}

void Ox93_MenuSystem::CloseOpenMenu()
{
	if (s_pxThis)
	{
		// OJ - Don't actually do the closing of the menu until the next input update
		//      otherwise the open menu could have called this command and we end up deleting the active object
		s_pxThis->m_bCloseMenu = true;
	}
}

void Ox93_MenuSystem::OpenPreviousMenu()
{
	if (!s_pxThis || s_pxThis->m_lpwszMenuStack.size() == 0) { return; }

	s_pxThis->m_lpwszMenuStack.pop_back();
	const WCHAR* pwszFilename = s_pxThis->m_lpwszMenuStack.back();

	OpenMenu(pwszFilename);
}

void Ox93_MenuSystem::HandleInput()
{
	if (!s_pxThis) { return; }

	if (s_pxThis->m_pxOpenMenu)
	{
		s_pxThis->m_pxOpenMenu->HandleInput();
	}

	if (Ox93_InputSystem::IsKeyDown(ESC_KEY) && Ox93_InputSystem::KeyJustPressed(ESC_KEY))
	{
		if (s_pxThis->m_lpwszMenuStack.size() > 1)
		{
			OpenPreviousMenu();
		}
		else if (Ox93_System::GetGameMode() == OX93_GAMEMODE_PAUSED)
		{
			Ox93_System::Resume();
			s_pxThis->m_bCloseMenu = true;
		}
	}

	// Close the menu if its being closed.
	if (s_pxThis->m_bCloseMenu)
	{
		if (s_pxThis->m_pxOpenMenu)
		{
			delete s_pxThis->m_pxOpenMenu;
			s_pxThis->m_pxOpenMenu = nullptr;
		}
		s_pxThis->m_bCloseMenu = false;

		if (s_pxThis->m_pwszQueuedMenu)
		{
			OpenMenu(s_pxThis->m_pwszQueuedMenu, s_pxThis->m_bDontStackQueued);
			if (s_pxThis->m_pxOpenMenu)
			{
				s_pxThis->m_pwszQueuedMenu = nullptr;
				s_pxThis->m_bDontStackQueued = false;
			}
		}
	}
}

void Ox93_MenuSystem::UpdateMenus()
{
	if (!s_pxThis || !s_pxThis->m_pxOpenMenu) { return; }
	
	s_pxThis->m_pxOpenMenu->Update();
}