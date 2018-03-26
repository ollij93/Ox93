#pragma once

#ifndef OX93_MENUBUTTON_H__
#define OX93_MENUBUTTON_H__

// Includes...
#include "ROOT/Ox93_System.h"
#include "ROOT/Graphics/2D/Text/Ox93_Text.h"

class Ox93_MenuButton
{
public:
	Ox93_MenuButton(const char* pszString, const void* pValue, VoidFunction pfnFunction, u_int uWidth, u_int uHeight, Ox93_Hash uTextureHash);
	Ox93_MenuButton(const char* pszString, const void* pValue, ListFunction pfnFunction, u_int uWidth, u_int uHeight, Ox93_Hash uTextureHash, u_int uIndex);
	~Ox93_MenuButton() {}

	void Update();
	void Shutdown();

	void OnPress();
	void SetPosition(int iX, int iY);
	Ox93_Vector_2 GetPosition() const { return m_xPosition; }
	u_int GetWidth() const { return m_uWidth; }
	u_int GetHeight() const { return m_uHeight; }

private:
	Ox93_Vector_2 m_xPosition;
	Ox93_Text m_xText;
	VoidFunction m_pfnFunction;
	ListFunction m_pfnListFunction;
	u_int m_uWidth;
	u_int m_uHeight;
	Ox93_BitMap* m_pxImage;
	const void* m_pValue;
	char m_pszString[256];
	u_int m_uListIndex;
};

#endif // OX93_MENUBUTTON_H__

