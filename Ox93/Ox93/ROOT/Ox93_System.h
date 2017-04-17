#pragma once

#ifndef __OX93_SYSTEM_H__
#define __OX93_SYSTEM_H__

#define WIN32_LEAN_AND_MEAN

// Includes...
#include "Ox93_Core.h"

// Forward declarations...
class Ox93_Text;

// Enums...
enum Ox93_GameMode
{
	OX93_GAMEMODE_INVALID,
	OX93_GAMEMODE_FREECAM,
	OX93_GAMEMODE_PLAYER,
	OX93_GAMEMODE_PAUSED,
	OX93_GAMEMODE_MENU,
};

class Ox93_System
{
public:
	static bool Create();
	static void Destroy();

	static void Run();

	static void StartGame();
	static void Pause();
	static void Resume();
	static void QuitGame();

	static HWND GetWindow() { return s_pxThis ? s_pxThis->m_hWnd : nullptr; }
	static VoidFunction GetCommand(const char* pszCommand);
	static PopulationFunction GetPopulationFunction(const char* pszFunctionName);
	static ListFunction GetListFunction(const char* pszFunctionName);
	static const void* GetValue(const char* pszValueName);

	static Ox93_GameMode GetGameMode() { return s_pxThis ? s_pxThis->m_eGameMode : OX93_GAMEMODE_INVALID; }
	static void SetGameMode(Ox93_GameMode eMode) { if (s_pxThis) { s_pxThis->m_eGameMode = eMode; } }

	static LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

protected:
	Ox93_System();
	~Ox93_System() {};
	bool Init();
	void Shutdown();

private:
	bool Frame();
	void InitWindows(u_int&, u_int&);
	void ShutdownWindows();


	LPCWSTR m_pszAppName;
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	Ox93_GameMode m_eGameMode;

	static Ox93_System* s_pxThis;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#endif // ifdef __OX93_SYSTEM_H__