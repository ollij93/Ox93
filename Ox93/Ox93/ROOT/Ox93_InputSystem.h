#pragma once

#ifndef __OX93_INPUT_H__
#define __OX93_INPUT_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Math/Ox93_Math.h"

// CONTROL_MAPPINGS
enum Ox93_ControlMap
{
	TAB_KEY = 0x09,
	SHIFT_KEY = 0x10,
	CTRL_KEY = 0x11,
	ESC_KEY = 0x1B,
	SPACE_KEY = 0x20,

	A_KEY = 0x41,
	D_KEY = 0x44,
	E_KEY = 0x45,
	Q_KEY = 0x51,
	S_KEY = 0x53,
	W_KEY = 0x57,

	F1_KEY = 0x70,
	F2_KEY = 0x71,
	F3_KEY = 0x72,
	F4_KEY = 0x73,
};

class Ox93_InputSystem
{
public:
	static bool Create();
	static void Destroy();

	static void HandleInput();

	static bool IsKeyDown(u_int uKey);
	static bool KeyJustPressed(u_int uKey);
	static void KeyDown(u_int uInput);
	static void KeyUp(u_int uInput);

	static void MouseMove(int iPosX, int iPosY);
	static void OnLMButtonPressed();
	static void OnRMButtonPressed();
	static void OnLMButtonReleased();
	static void OnRMButtonReleased();
	static bool IsLMButtonDown() { return (s_pxThis ? s_pxThis->m_bLMButtonDown : false); }
	static bool IsRMButtonDown() { return (s_pxThis ? s_pxThis->m_bRMButtonDown : false); }
	static bool GetLMBClicked() { return (s_pxThis ? s_pxThis->m_bLMButtonClicked : false); }
	static bool GetRMBClicked() { return (s_pxThis ? s_pxThis->m_bRMButtonClicked : false); }

	static bool GetMouseMoved() { return (s_pxThis ? s_pxThis->m_bMouseMoved : false); }
	static Ox93_Vector_2 GetMouseDisplacement() { return (s_pxThis ? s_pxThis->m_xMouseMove : Ox93_Vector_2(0, 0)); }

	static Ox93_Vector_2 GetMouseClickLocation() { return (s_pxThis ? s_pxThis->m_xLMBClick : Ox93_Vector_2(0, 0)); }

	static void SetCursorPosition(const Ox93_Vector_2& xPos);
protected:
	Ox93_InputSystem();
	~Ox93_InputSystem() {};

	void SetMouseMoved(int iPosX, int iPosY);
private:
	bool m_abKeys[256];
	bool m_abFreshKeys[256];

	Ox93_Vector_2 m_xMousePos;  // The current position of mouse on the screen
	Ox93_Vector_2 m_xMouseMove; // The displacement of the mouse since the previous frame
	Ox93_Vector_2 m_xLMBClick;  // The location of the mouse when the LMButton message is received
	bool m_bMouseMoved;
	bool m_bLMButtonDown;
	bool m_bRMButtonDown;
	bool m_bLMButtonClicked; // Only true for a single update
	bool m_bRMButtonClicked;

	static Ox93_InputSystem* s_pxThis;
};

#endif //ifdef __OX93_INPUT_H__