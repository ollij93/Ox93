#pragma once

#ifndef OX93_TEXT_H__
#define OX93_TEXT_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Math/Ox93_Math.h"

// Forward Declarations...
class Ox93_BitMap;

class Ox93_Text
{
public:
	Ox93_Text(const char* pszString, Ox93_Hash uFontHash);
	~Ox93_Text();

	void SetPosition(int iX, int iY);
	void SetString(const char* pszString);

	u_int GetWidth();
	u_int GetHeight();

private:
	void FillBitMaps();

	Ox93_Hash m_uFontHash;
	const char* m_pszString;
	Ox93_Vector_2 m_xPosition;
	std::list<Ox93_BitMap*> m_lpxBitMaps;
};

#endif // OX93_TEXT_H__

