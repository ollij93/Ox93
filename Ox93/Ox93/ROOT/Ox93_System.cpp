// Includes...
#include "Ox93_System.h"
#include "Game/Entity/Ox93_Entity.h"
#include "Game/Entity/PhysicalObject/Character/Ox93_Character.h"
#include "Game/Inventory/Ox93_Inventory.h"
#include "Game/LoadSave/Ox93_LoadSystem.h"
#include "Game/LoadSave/Ox93_SaveSystem.h"
#include "Game/Menu/Ox93_MenuSystem.h"
#include "Game/World/Terrain/Ox93_TerrainSystem.h"
#include "ROOT/Graphics/2D/Text/Ox93_Font.h"
#include "ROOT/Graphics/2D/Text/Ox93_Text.h"
#include "ROOT/Graphics/Ox93_GraphicsSystem.h"
#include "ROOT/Physics/Ox93_CollisionObject.h"
#include "ROOT/Specification/Ox93_Specification_System.h"
#include "ROOT/Ox93_InputSystem.h"

// Global Variables...
static const u_int uMOUSE_POS_X = 800;
static const u_int uMOUSE_POS_Y = 400;

// Static variables...
Ox93_System* Ox93_System::s_pxThis = nullptr;

Ox93_System::Ox93_System()
: m_eGameMode(OX93_GAMEMODE_FREECAM)
{
}

bool Ox93_System::Create()
{
	Ox93_Assert(!s_pxThis, "Ox93_System is already created.");
	if (s_pxThis) { return false; }
	
	s_pxThis = new Ox93_System;
	return s_pxThis->Init();
}

void Ox93_System::Destroy()
{
	if (!s_pxThis) { return; }

	s_pxThis->Shutdown();
	delete s_pxThis;
	s_pxThis = nullptr;
}


bool Ox93_System::Init()
{
	u_int uScreenWidth = 0;
	u_int uScreenHeight = 0;
	InitWindows(uScreenWidth, uScreenHeight);

	bool bResult = Ox93_GraphicsSystem::Create(uScreenWidth, uScreenHeight, m_hWnd);
	Ox93_Assert(bResult, "Failed to create Ox93_GraphicsSystem.");
	if (!bResult) { return false; }

	bResult = Ox93_InputSystem::Create();
	Ox93_Assert(bResult, "Failed to create Ox93_InputSystem.");
	if (!bResult) { return false; }

	Ox93_EntityRegistry::RegisterEntityClasses();

	bResult = Ox93_Specification_System::Create();
	Ox93_Assert(bResult, "Failed to create Ox93_Specification_System.");
	if (!bResult) { return false; }

	bResult = Ox93_LoadSystem::Create();
	Ox93_Assert(bResult, "Failed to create Ox93_LoadSystem.");
	if (!bResult) { return false; }

	bResult = Ox93_SaveSystem::Create();
	Ox93_Assert(bResult, "Failed to create Ox93_SaveSystem.");
	if (!bResult) { return false; }

	bResult = Ox93_MenuSystem::Create();
	Ox93_Assert(bResult, "Failed to create Ox93_MenuSystem.");
	if (!bResult) { return false; }

	Ox93_MenuSystem::OpenTitleMenu();
	Ox93_LoadSystem::LoadPlayerSettings(L"Saves/Olli.plyr");

	return true;
}


void Ox93_System::Shutdown()
{
	Ox93_GraphicsSystem::Destroy();
	Ox93_InputSystem::Destroy();
	Ox93_Specification_System::Destroy();
	Ox93_LoadSystem::Destroy();
	Ox93_SaveSystem::Destroy();
	//Ox93_TerrainSystem::Destroy();
	Ox93_MenuSystem::Destroy();

	ShutdownWindows();
}


void Ox93_System::Run()
{
	if (!s_pxThis) { return; }

	MSG xMsg;
	ZeroMemory(&xMsg, sizeof(MSG));

	///////////////////////////////////
	// M A I N L O O P
	///////////////////////////////////
	bool bDone = false;
	while (!bDone)
	{
		// Handle the windows messages.
		if (PeekMessage(&xMsg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&xMsg);
			DispatchMessage(&xMsg);
		}

		if (xMsg.message == WM_QUIT)
		{
			bDone = true;
		}
		else
		{
			// No message to quit from windows so do our frame processing
			bool bResult = s_pxThis->Frame();
			if (!bResult) { bDone = true; }
		}
	}
}


bool Ox93_System::Frame()
{
	// TODO-OJ : Some of these will need only to be done every N frames or a limited number of times a second for speed
	Ox93_InputSystem::HandleInput();
	Ox93_Entity::ProcessUpdates();
	Ox93_MenuSystem::UpdateMenus();
	//Ox93_TerrainSystem::Update();

	// Do the graphics frame processing
	bool bResult = Ox93_GraphicsSystem::Frame();
	Ox93_Assert(bResult, "The frame processing of Ox93_GraphicsSystem failed.");
	return bResult;
}


LRESULT CALLBACK Ox93_System::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// Manage Keyboard input
		case WM_LBUTTONDOWN:
		{
			Ox93_InputSystem::OnLMButtonPressed();
			return 0;
		}
		case WM_LBUTTONUP:
		{
			Ox93_InputSystem::OnLMButtonReleased();
			return 0;
		}
		case WM_RBUTTONDOWN:
		{
			Ox93_InputSystem::OnRMButtonPressed();
			return 0;
		}
		case WM_RBUTTONUP:
		{
			Ox93_InputSystem::OnRMButtonReleased();
			return 0;
		}
		case WM_KEYDOWN:
		{
			Ox93_InputSystem::KeyDown(static_cast<u_int>(wParam));
			return 0;
		}
		case WM_KEYUP:
		{
			Ox93_InputSystem::KeyUp(static_cast<u_int>(wParam));
			return 0;
		}
		// Default to let windows handle the message
		default:
		{
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
}


void Ox93_System::InitWindows(u_int& uScreenWidth, u_int& uScreenHeight)
{
	// Get the instance of this application.
	m_hInstance = GetModuleHandle(NULL);

	// Name the program
	m_pszAppName = L"Ox93";

	// Setup the windows class with default settings.
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_pszAppName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients monitor.
	uScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	uScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	u_int uPosX, uPosY;
	if (Ox93_GraphicsSystem::s_bFullscreen)
	{
		// If full screen set the screen to maximum size of the users monitor and 32bit color
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (u_long)uScreenWidth;
		dmScreenSettings.dmPelsHeight = (u_long)uScreenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		uPosX = uPosY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		uScreenWidth = 800;
		uScreenHeight = 600;

		// Place the window in the middle of the screen.
		uPosX = (GetSystemMetrics(SM_CXSCREEN) - uScreenWidth) / 2;
		uPosY = (GetSystemMetrics(SM_CYSCREEN) - uScreenHeight) / 2;
	}

	RECT xWindowRect = { 0, 0, uScreenWidth, uScreenHeight };
	AdjustWindowRect(&xWindowRect, WS_OVERLAPPEDWINDOW, false);

	// Create the window with the screen settings and get the handle to it.
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_pszAppName, m_pszAppName,
		WS_OVERLAPPEDWINDOW,
		uPosX, uPosY, xWindowRect.right - xWindowRect.left, xWindowRect.bottom - xWindowRect.top, NULL, NULL, m_hInstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
}


void Ox93_System::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (Ox93_GraphicsSystem::s_bFullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;

	// Remove the application instance.
	UnregisterClass(m_pszAppName, m_hInstance);
	m_hInstance = nullptr;

	return;
}

void Ox93_System::StartGame()
{
	if (!s_pxThis) { return; }

	const u_int uGameMode = s_pxThis->m_eGameMode;
	switch(uGameMode)
	{
		case OX93_GAMEMODE_INVALID:
		{ 
			break;
		}
		case OX93_GAMEMODE_MENU:
		{
			Ox93_MenuSystem::CloseOpenMenu();
			Ox93_LoadSystem::LoadDefault();
			Ox93_Entity::InitUpdates();
			break;
		}
		default:
			Ox93_Assert(false, "Attempting to StartGame from an invalid game mode!");
			return;
	}

	Ox93_Character::CreateLocalPlayer();
	Ox93_MenuSystem::OpenHUD();
	Ox93_MenuSystem::ClearMenuStack();
	s_pxThis->m_eGameMode = OX93_GAMEMODE_PLAYER;
	ShowCursor(false);
}

void Ox93_System::Resume()
{
	if (!s_pxThis) { return; }

	Ox93_Assert(s_pxThis->m_eGameMode == OX93_GAMEMODE_PAUSED, "Attempting to Resume() from invalid gamemode");
	if (s_pxThis->m_eGameMode == OX93_GAMEMODE_PAUSED)
	{
		Ox93_MenuSystem::CloseOpenMenu();
		Ox93_MenuSystem::OpenHUD();
		Ox93_MenuSystem::ClearMenuStack();
		s_pxThis->m_eGameMode = OX93_GAMEMODE_PLAYER;
		ShowCursor(false);
	}
}

void Ox93_System::Pause()
{
	if (!s_pxThis) { return; }

	Ox93_Assert(s_pxThis->m_eGameMode == OX93_GAMEMODE_PLAYER, "Attempting to Pause() from invalid gamemode");
	if (s_pxThis->m_eGameMode == OX93_GAMEMODE_PLAYER)
	{
		s_pxThis->m_eGameMode = OX93_GAMEMODE_PAUSED;
		ShowCursor(true);
	}
}

void Ox93_System::QuitGame()
{
	if (s_pxThis->m_eGameMode == OX93_GAMEMODE_PAUSED)
	{
		Ox93_SaveSystem::SaveCurrentWorldToFile(L"Saves/DefaultSave.ent");
		Ox93_SaveSystem::SavePlayerSettings(L"Saves/Olli.plyr");
		//Ox93_TerrainSystem::Destroy();
		Ox93_Entity::ShutdownAll();
	}


	if (!s_pxThis) { return; }
	Ox93_Assert(s_pxThis->m_eGameMode == OX93_GAMEMODE_PAUSED || s_pxThis->m_eGameMode == OX93_GAMEMODE_MENU,
		"Attempting to quit the game from an unexpected GameMode, will continue anyway but this should not be happening");

	PostMessage(s_pxThis->m_hWnd, WM_QUIT, 0, 0);
}

///////////////////////////////////////////////////////////////////
// C O M M A N D S 
///////////////////////////////////////////////////////////////////

struct Ox93_CommandRegisterEntry
{
	Ox93_Hash m_uCommandHash;
	VoidFunction m_pfnFunction;
};

Ox93_CommandRegisterEntry axRegistry[]
{
	GetHash("Menu.Title.StartGame"), Ox93_System::StartGame,
	GetHash("Menu.Open.Title"), Ox93_MenuSystem::OpenTitleMenu,
	GetHash("Menu.Open.Options"), Ox93_MenuSystem::OpenOptionsMenu,
	GetHash("Menu.ReturnToPrevious"), Ox93_MenuSystem::OpenPreviousMenu,
	GetHash("FOV.Increase"), Ox93_D3D::IncreaseFOV,
	GetHash("FOV.Decrease"), Ox93_D3D::DecreaseFOV,
	GetHash("Game.Resume"), Ox93_System::Resume,
	GetHash("Game.Quit"), Ox93_System::QuitGame,
};

VoidFunction Ox93_System::GetCommand(const char* pszCommand)
{
	Ox93_Hash uCommandHash = GetHash(pszCommand);
	for (u_int u = 0; u < sizeof(axRegistry) / sizeof(axRegistry[0]); u++)
	{
		if (axRegistry[u].m_uCommandHash == uCommandHash)
		{
			return axRegistry[u].m_pfnFunction;
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////
// P O P U L A T I O N   F U N C T I O N S
///////////////////////////////////////////////////////////////////

struct Ox93_PopulationRegisterEntry
{
	Ox93_Hash m_uCommandHash;
	PopulationFunction m_pfnFunction;
};

Ox93_PopulationRegisterEntry axPopFuncRegistry[]
{
	GetHash("Player.Inventory.Name"), Ox93_Inventory::PlayerInventoryItemName,
	GetHash("Player.Inventory.Quantity"), Ox93_Inventory::PlayerInventoryItemQuantity,
};

PopulationFunction Ox93_System::GetPopulationFunction(const char* pszFunctionName)
{
	Ox93_Hash uHash = GetHash(pszFunctionName);
	for (u_int u = 0; u < sizeof(axPopFuncRegistry) / sizeof(axPopFuncRegistry[0]); u++)
	{
		if (axPopFuncRegistry[u].m_uCommandHash == uHash)
		{
			return axPopFuncRegistry[u].m_pfnFunction;
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////
// P O P U L A T I O N   F U N C T I O N S
///////////////////////////////////////////////////////////////////

struct Ox93_ListFunctionRegisterEntry
{
	Ox93_Hash m_uCommandHash;
	ListFunction m_pfnFunction;
};

Ox93_ListFunctionRegisterEntry axListFuncRegistry[]
{
	GetHash("Player.Inventory.SelectItem"), Ox93_Inventory::PlayerSelectItem,
};

ListFunction Ox93_System::GetListFunction(const char* pszFunctionName)
{
	Ox93_Hash uHash = GetHash(pszFunctionName);
	for (u_int u = 0; u < sizeof(axListFuncRegistry) / sizeof(axListFuncRegistry[0]); u++)
	{
		if (axListFuncRegistry[u].m_uCommandHash == uHash)
		{
			return axListFuncRegistry[u].m_pfnFunction;
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////
// V A L U E S
///////////////////////////////////////////////////////////////////

struct Ox93_ValueRegisterEntry
{
	Ox93_Hash m_uValueHash;
	const void* m_pValue;
};

Ox93_ValueRegisterEntry axValueRegistry[]
{
	GetHash("FOV.Value"), &Ox93_D3D::s_fReadOnlyDegreesFOV,
	GetHash("Graphics.FPS"), &Ox93_GraphicsSystem::s_fFPS,
};

const void* Ox93_System::GetValue(const char* pszValueName)
{
	Ox93_Hash uValueHash = GetHash(pszValueName);
	for (u_int u = 0; u < sizeof(axValueRegistry) / sizeof(axValueRegistry[0]); u++)
	{
		if (axValueRegistry[u].m_uValueHash == uValueHash)
		{
			return axValueRegistry[u].m_pValue;
		}
	}

	return nullptr;
}

///////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Handle the window specific messages
		case WM_DESTROY:
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		// All other messages let the settings class deal with
		default:
		{
			return Ox93_System::MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
