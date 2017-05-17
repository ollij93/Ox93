// Includes...
#include "Ox93_MenuButton.h"
#include "ROOT/Graphics/2D/Text/Ox93_Font.h"
#include "ROOT/Graphics/2D/Ox93_BitMap.h"

Ox93_MenuButton::Ox93_MenuButton(const char* pszString, const void* pValue, VoidFunction pfnFunction, u_int uWidth, u_int uHeight, Ox93_Hash uTextureHash)
: m_xPosition(0, 0)
, m_xText(pszString, uOX93_FONT_DEFAULT)
, m_pfnFunction(pfnFunction)
, m_pfnListFunction(nullptr)
, m_uWidth(uWidth)
, m_uHeight(uHeight)
, m_pxImage(nullptr)
, m_pValue(pValue)
, m_pszString()
, m_uListIndex(0)
{
	if (uTextureHash != OX93_HASH_UNSET)
	{
		m_pxImage = new Ox93_BitMap;
		m_pxImage->Init(uTextureHash, uWidth, uHeight);
	}

	for (int i = 0; pszString[i] != 0 && i < 256; i++)
	{
		m_pszString[i] = pszString[i];
	}

	Update();
}

Ox93_MenuButton::Ox93_MenuButton(const char* pszString, const void* pValue, ListFunction pfnFunction, u_int uWidth, u_int uHeight, Ox93_Hash uTextureHash, u_int uListIndex)
: m_xPosition(0, 0)
, m_xText(pszString, uOX93_FONT_DEFAULT)
, m_pfnFunction(nullptr)
, m_pfnListFunction(pfnFunction)
, m_uWidth(uWidth)
, m_uHeight(uHeight)
, m_pxImage(nullptr)
, m_pValue(pValue)
, m_pszString()
, m_uListIndex(uListIndex)
{
	if (uTextureHash != OX93_HASH_UNSET)
	{
		m_pxImage = new Ox93_BitMap;
		m_pxImage->Init(uTextureHash, uWidth, uHeight);
	}

	for (int i = 0; pszString[i] != 0 && i < 256; i++)
	{
		m_pszString[i] = pszString[i];
	}

	Update();
}

void Ox93_MenuButton::Update()
{
		// Check for string formatting 
		char cType = 0;
		for (int i = 0; i < 256 && m_pszString[i] != 0; i++)
		{
			if (m_pszString[i] == '%')
			{
				if (m_pszString[i + 1] == '.')
				{
					for (short j = 2; j < 10; j++)
					{
						if (m_pszString[i + j] < '0' || m_pszString[i + j] > '9')
						{
							cType = m_pszString[i + j];
							break;
						}
					}
				}
				else
				{
					cType = m_pszString[i + 1];
				}
			}
		}

		// If formatting update the value
		if (cType != 0)
		{
			char* pszNewString = new char[256];
			switch (cType)
			{
				case 'i':
				{
					sprintf_s(pszNewString, 256, m_pszString, *static_cast<const int*>(m_pValue));
					break;
				}
				case 'f':
				{
					sprintf_s(pszNewString, 256, m_pszString, *static_cast<const float*>(m_pValue));
					break;
				}
				case 's':
				{
					sprintf_s(pszNewString, 256, m_pszString, *static_cast<char* const *>(m_pValue));
					break;
				}
				default:
					break;
			}
			m_xText.SetString(pszNewString);
		}
}

void Ox93_MenuButton::Shutdown()
{
	if (m_pxImage)
	{
		m_pxImage->Shutdown();
		delete m_pxImage;
		m_pxImage = nullptr;
	}
}

void Ox93_MenuButton::OnPress()
{
	if (m_pfnFunction)
	{
		m_pfnFunction();
	}

	if (m_pfnListFunction)
	{
		m_pfnListFunction(m_uListIndex);
	}
}

void Ox93_MenuButton::SetPosition(int iX, int iY)
{
	m_xText.SetPosition(iX + (m_uWidth - m_xText.GetWidth()) / 2, iY - (m_uHeight - m_xText.GetHeight()) / 2);
	if (m_pxImage)
	{
		m_pxImage->SetPosition(iX, iY);
	}
	m_xPosition = Ox93_Vector_2((float)iX, (float)iY);
}
