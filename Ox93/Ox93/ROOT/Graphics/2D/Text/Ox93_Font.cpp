// TinyXML...
#include "TinyXML/tinyxml2.h"

// Includes...
#include "Ox93_Font.h"
#include "ROOT/Assert/Ox93_Assert.h"
#include "ROOT/Graphics/2D/Ox93_BitMap.h"

// Statics...
std::list<Ox93_Font*> Ox93_Font::s_lpxFontList;

Ox93_Font::Ox93_Font()
: m_uHash(OX93_HASH_UNSET)
, m_uTextureHash(OX93_HASH_UNSET)
, m_uHeight(0)
, m_axCharacterHandles()
{
}

void Ox93_Font::CreateFontFromFile(const WCHAR* pwszFontname)
{
	Ox93_Font* pxNewFont = new Ox93_Font;
	bool bSuccess = pxNewFont->LoadFontFromFile(pwszFontname);
	Ox93_Assert(bSuccess, "Failed to load a font.");
	if (bSuccess)
	{
		s_lpxFontList.push_back(pxNewFont);
	}
	else
	{
		delete pxNewFont;
		pxNewFont = nullptr;
	}
}

bool Ox93_Font::LoadFontFromFile(const WCHAR* pwszFontname)
{
	FILE* pxFile = nullptr;
	_wfopen_s(&pxFile, pwszFontname, L"rb");
	Ox93_Assert(pxFile, "Failed to open file for loading entities.");
	if (pxFile)
	{
		float fCurrentLeft = 0.f;

		tinyxml2::XMLDocument xFontDoc;
		xFontDoc.LoadFile(pxFile);

		tinyxml2::XMLElement* pxFontElement = nullptr;
		pxFontElement = xFontDoc.FirstChildElement("Font");
		if (pxFontElement)
		{
			const char* pszTextureName = pxFontElement->Attribute("texture");
			m_uTextureHash = GetHash(pszTextureName);

			const char* pszName = pxFontElement->Attribute("name");
			m_uHash = GetHash(pszName);

			m_uHeight = pxFontElement->UnsignedAttribute("height");
			
			u_int uTextureWidth = pxFontElement->UnsignedAttribute("texturewidth");
			Ox93_Assert(uTextureWidth > 0, "Zero width for font texture is not allowed");
			if (uTextureWidth > 0)
			{
				tinyxml2::XMLElement* pxCharElement = nullptr;
				pxCharElement = pxFontElement->FirstChildElement("Character");
				while (pxCharElement)
				{
					const u_int uFontChar = pxCharElement->UnsignedAttribute("char");

					const u_int uCharWidth = pxCharElement->UnsignedAttribute("width");
					if (uFontChar >= OX93_FONT_CHARACTER_SPACE && uFontChar < OX93_NUM_FONT_CHARACTERS)
					{
						float fLeft = fCurrentLeft;
						float fRight = fLeft + static_cast<float>(uCharWidth) / static_cast<float>(uTextureWidth);

						m_axCharacterHandles[uFontChar].m_fLeft = fLeft;
						m_axCharacterHandles[uFontChar].m_fRight = fRight;
						m_axCharacterHandles[uFontChar].m_uWidth = uCharWidth;

						if ( (fRight - fLeft) > 0.f)
						{
							fCurrentLeft = fRight + 1.f / static_cast<float>(uTextureWidth);
						}
					}
					else
					{
						Ox93_Assert(false, "Found invalid character in font file.");
					}

					pxCharElement = pxCharElement->NextSiblingElement("Character");
				}
				return true;
			}
		}

		fclose(pxFile);
	}

	return false;
}

const Ox93_Font* Ox93_Font::GetFontByHash(Ox93_Hash uHash)
{
	std::list<Ox93_Font*>::const_iterator xIter;
	for (xIter = s_lpxFontList.begin(); xIter != s_lpxFontList.end(); ++xIter)
	{
		Ox93_Font* pxFont = *xIter;
		if (pxFont)
		{
			if (pxFont->m_uHash == uHash)
			{
				return pxFont;
			}
		}
	}
	return nullptr;
}

Ox93_BitMap* Ox93_Font::CreateBitMapForCharacter(u_int uFontCharacter) const
{
	Ox93_Assert(uFontCharacter < OX93_NUM_FONT_CHARACTERS, "Invalid character when requesting enum.");
	if (uFontCharacter >= OX93_NUM_FONT_CHARACTERS) { return nullptr; }

	Ox93_BitMap* pxNewBitMap = new Ox93_BitMap;

	Ox93_Font_CharacterHandle xFontHandle = m_axCharacterHandles[uFontCharacter];
	bool bSuccess = pxNewBitMap->Init(m_uTextureHash, xFontHandle.m_uWidth, m_uHeight, xFontHandle.m_fLeft, xFontHandle.m_fRight);
	if (bSuccess)
	{
		return pxNewBitMap;
	}

	Ox93_Assert(false, "Failed to initialise a character bitmap.");
	pxNewBitMap->Shutdown();
	delete pxNewBitMap;
	return nullptr;
}