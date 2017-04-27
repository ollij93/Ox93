// Includes...
#include "Ox93_InputSystem.h"
#include "Game/Entity/Camera/Ox93_Camera.h"
#include "Game/Entity/PhysicalObject/Character/Ox93_Character.h"
#include "Game/Menu/Ox93_MenuSystem.h"
#include "ROOT/Graphics/Ox93_GraphicsSystem.h"

// Global Variables...
static const u_int uOX93_MOUSE_POS_X = 800;
static const u_int uOX93_MOUSE_POS_Y = 400;

// Static Variables...
Ox93_InputSystem* Ox93_InputSystem::s_pxThis;

Ox93_InputSystem::Ox93_InputSystem()
: m_abKeys()
, m_abFreshKeys()
, m_xMousePos(uOX93_MOUSE_POS_X, uOX93_MOUSE_POS_Y)
, m_xMouseMove(0,0)
, m_xLMBClick(0,0)
, m_bMouseMoved(false)
, m_bLMButtonDown(false)
, m_bRMButtonDown(false)
, m_bLMButtonClicked(false)
, m_bRMButtonClicked(false)
{
	// Initialize all the keys to being released and not pressed.
	for (int i = 0; i < 256; i++)
	{
		m_abKeys[i] = false;
		m_abFreshKeys[i] = false;
	}

	SetCursorPos(uOX93_MOUSE_POS_X, uOX93_MOUSE_POS_Y);
}

bool Ox93_InputSystem::Create()
{
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_InputSystem;
	return true;
}

void Ox93_InputSystem::Destroy()
{
	if (!s_pxThis) { return; }
	delete s_pxThis;
	s_pxThis = nullptr;
}


void Ox93_InputSystem::KeyDown(u_int uInput)
{
	if (!s_pxThis) { return; }
	s_pxThis->m_abFreshKeys[uInput] = !IsKeyDown(uInput);
	s_pxThis->m_abKeys[uInput] = true;
}


void Ox93_InputSystem::KeyUp(u_int uInput)
{
	if (!s_pxThis) { return; }
	s_pxThis->m_abKeys[uInput] = false;
}


bool Ox93_InputSystem::IsKeyDown(u_int uKey)
{
	if (!s_pxThis) { return false; }
	return s_pxThis->m_abKeys[uKey];
}

bool Ox93_InputSystem::KeyJustPressed(u_int uKey)
{
	if (!s_pxThis) { return false; }
	return s_pxThis->m_abFreshKeys[uKey];
}

void Ox93_InputSystem::MouseMove(int iPosX, int iPosY)
{
	if (!s_pxThis) { return; }
	s_pxThis->SetMouseMoved(iPosX, iPosY);

	s_pxThis->m_xMousePos = Ox93_Vector_2(iPosX, iPosY);
}

void Ox93_InputSystem::OnLMButtonPressed()
{
	if (!s_pxThis) { return; }
	s_pxThis->m_bLMButtonDown = true;
	s_pxThis->m_bLMButtonClicked = true;

	POINT xPoint;
	GetCursorPos(&xPoint);

	s_pxThis->m_xLMBClick = Ox93_Vector_2(xPoint.x, xPoint.y);
}

void Ox93_InputSystem::OnRMButtonPressed()
{
	if (!s_pxThis) { return; }
	s_pxThis->m_bRMButtonDown = true;
	s_pxThis->m_bRMButtonClicked = true;
}

void Ox93_InputSystem::OnLMButtonReleased()
{
	if (!s_pxThis) { return; }
	s_pxThis->m_bLMButtonDown = false;
}

void Ox93_InputSystem::OnRMButtonReleased()
{
	if (!s_pxThis) { return; }
	s_pxThis->m_bRMButtonDown = false;
}

void Ox93_InputSystem::SetMouseMoved(int iPosX, int iPosY)
{
	m_xMouseMove = Ox93_Vector_2(iPosX, iPosY) - m_xMousePos;	
	m_bMouseMoved = true;
}

void Ox93_InputSystem::HandleInput()
{
	if (!s_pxThis) { return; }

	// Get the current mouse position from windows
	POINT xMousePos;
	GetCursorPos(&xMousePos);
	MouseMove(xMousePos.x, xMousePos.y);

	switch (Ox93_System::GetGameMode())
	{
		case OX93_GAMEMODE_FREECAM:
		{
			// TODO-OJ
			//Ox93_Camera* pxCamera = Ox93_Camera::GetDefault();
			//pxCamera->HandleInput();
			break;
		}
		case OX93_GAMEMODE_PLAYER:
		{
			// Let the HUD handle the input first then the Character
			Ox93_MenuSystem::HandleInput();
			Ox93_Character::HandleInput();
			break;
		}
		case OX93_GAMEMODE_PAUSED:
		{
			Ox93_MenuSystem::HandleInput();
			break;
		}
		case OX93_GAMEMODE_MENU:
		{
			Ox93_MenuSystem::HandleInput();
			break;
		}
	}

	s_pxThis->m_bLMButtonClicked = false;
	s_pxThis->m_bRMButtonClicked = false;
	for (int i = 0; i < 256; i++)
	{
		s_pxThis->m_abFreshKeys[i] = false;
	}
}


void Ox93_InputSystem::SetCursorPosition(const Ox93_Vector_2& xPos)
{
	if (!s_pxThis) { return; }

	SetCursorPos(xPos.x, xPos.y);
	s_pxThis->m_xMousePos = xPos;
}