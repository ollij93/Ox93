// Includes...
#include "Ox93_Text.h"
#include "ROOT/Assert/Ox93_Assert.h"
#include "ROOT/Graphics/2D/Ox93_BitMap.h"
#include "ROOT/Graphics/2D/Text/Ox93_Font.h"
#include "ROOT/Math/Ox93_Math_Vectors.h"

Ox93_Text::Ox93_Text(const char* pszString, Ox93_Hash uFontHash)
: m_uFontHash(uFontHash)
, m_pszString(pszString)
, m_lpxBitMaps()
, m_xPosition()
{
}

Ox93_Text::~Ox93_Text()
{
	while (m_lpxBitMaps.size() > 0)
	{
		Ox93_BitMap* pxBitMap = m_lpxBitMaps.back();
		pxBitMap->Shutdown();
		delete pxBitMap;
		m_lpxBitMaps.pop_back();
	}
}

void Ox93_Text::SetPosition(int iX, int iY)
{
	m_xPosition = Ox93_Vector_2( iX, iY );

	if (m_lpxBitMaps.size() == 0)
	{
		FillBitMaps();
	}

	std::list<Ox93_BitMap*>::const_iterator xIter;
	for (xIter = m_lpxBitMaps.begin(); xIter != m_lpxBitMaps.end(); ++xIter)
	{
		Ox93_BitMap* pxBitMap = *xIter;
		if (pxBitMap)
		{
			pxBitMap->SetPosition(iX, iY);
			iX += pxBitMap->GetWidth();
			iX += 1;
		}
	}
}

void Ox93_Text::SetString(const char* pszString)
{
	// OJ - Shutdown and delete the old bitmaps
	while (m_lpxBitMaps.size() > 0)
	{
		Ox93_BitMap* pxBitMap = m_lpxBitMaps.back();
		pxBitMap->Shutdown();
		delete pxBitMap;
		m_lpxBitMaps.pop_back();
	}

	m_pszString = pszString;

	// OJ - Create the new bitmaps
	FillBitMaps();
	// OJ - Reposition the text
	SetPosition(static_cast<int>(m_xPosition.x), static_cast<int>(m_xPosition.y));
}

void Ox93_Text::FillBitMaps()
{
	Ox93_Assert(m_lpxBitMaps.size() == 0, "Attempting to fill text bit maps without an empty list.");
	if (m_lpxBitMaps.size() != 0) { return; }

	const Ox93_Font* pxFont = Ox93_Font::GetFontByHash(m_uFontHash);
	if (pxFont)
	{
		const char* pcChar = m_pszString;
		while (*pcChar)
		{
			Ox93_BitMap* pxBitMap = pxFont->CreateBitMapForCharacter(*pcChar - ' ');
			if (pxBitMap)
			{
				m_lpxBitMaps.push_back(pxBitMap);
			}

			pcChar++;
		}
	}
}

u_int Ox93_Text::GetWidth()
{
	u_int uWidth = 0;

	std::list<Ox93_BitMap*>::const_iterator xIter;
	for (xIter = m_lpxBitMaps.begin(); xIter != m_lpxBitMaps.end(); ++xIter)
	{
		Ox93_BitMap* pxBitMap = *xIter;
		if (pxBitMap)
		{
			uWidth += pxBitMap->GetWidth();
			uWidth += 1;
		}
	}

	return uWidth;
}

u_int Ox93_Text::GetHeight()
{
	u_int uHeight = 0;

	if (m_lpxBitMaps.size() == 0)
	{
		FillBitMaps();
	}
	// Now we know we've got BitMaps. If the string is empty then the size is zero
	if (!m_lpxBitMaps.size() == 0)
	{
		Ox93_BitMap* pxBitMap = m_lpxBitMaps.front();
		if (pxBitMap)
		{
			uHeight = pxBitMap->GetHeight();
		}
	}
	

	return uHeight;
}