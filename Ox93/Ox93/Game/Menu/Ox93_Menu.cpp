// External Includes...
#include "TinyXML/tinyxml2.h"

// Includes...
#include "Ox93_Menu.h"
#include "Game/Ox93_MiniMap.h"
#include "ROOT/Graphics/2D/Ox93_BitMap.h"
#include "ROOT/Ox93_InputSystem.h"

Ox93_Menu::Ox93_Menu()
: m_uNameHash(OX93_HASH_UNSET)
, m_pxBackgroundImage(nullptr)
, m_pxMiniMap(nullptr)
, m_lpxButtons()
{
}

Ox93_Menu::~Ox93_Menu()
{
	while (m_lpxButtons.size() > 0)
	{
		Ox93_MenuButton* pxButton = m_lpxButtons.back();
		pxButton->Shutdown();
		delete pxButton;
		m_lpxButtons.pop_back();
	}

	if (m_pxBackgroundImage)
	{
		m_pxBackgroundImage->Shutdown();
		delete m_pxBackgroundImage;
		m_pxBackgroundImage = nullptr;
	}

	if (m_pxMiniMap)
	{
		delete m_pxMiniMap;
		m_pxMiniMap = nullptr;
	}
}

void Ox93_Menu::Update()
{
	if (m_pxMiniMap)
	{
		m_pxMiniMap->Update();
	}
}

bool Ox93_Menu::LoadFromFile(const WCHAR* pwszFilename)
{
	FILE* pxFile = nullptr;
	_wfopen_s(&pxFile, pwszFilename, L"rb");
	Ox93_Assert(pxFile, "Failed to open file for loading menu.");
	if (pxFile)
	{
		tinyxml2::XMLDocument xDoc;
		xDoc.LoadFile(pxFile);

		tinyxml2::XMLElement* pxMenuElement = nullptr;
		pxMenuElement = xDoc.FirstChildElement("Menu");
		if (pxMenuElement)
		{
			const char* pszName = pxMenuElement->Attribute("name");
			m_uNameHash = GetHash(pszName);

			tinyxml2::XMLElement* pxBackgroundElement = nullptr;
			pxBackgroundElement = pxMenuElement->FirstChildElement("BackgroundImage");
			if (pxBackgroundElement)
			{
				const char* pszBackgroundTexture = pxBackgroundElement->Attribute("texture");
				Ox93_Hash uBackgroundHash = GetHash(pszBackgroundTexture);
				u_int uBackgroundWidth = pxBackgroundElement->UnsignedAttribute("width");
				u_int uBackgroundHeight = pxBackgroundElement->UnsignedAttribute("height");

				if (m_pxBackgroundImage)
				{
					m_pxBackgroundImage->Shutdown();
					delete m_pxBackgroundImage;
					m_pxBackgroundImage = nullptr;
				}
				m_pxBackgroundImage = new Ox93_BitMap;
				if (m_pxBackgroundImage->Init(uBackgroundHash, uBackgroundWidth, uBackgroundHeight))
				{
					// OJ - Centre the background image
					m_pxBackgroundImage->SetPosition(-static_cast<int>(uBackgroundWidth / 2), static_cast<int>(uBackgroundHeight / 2));
				}
				else
				{
					Ox93_Assert(false, "Failed to initialise the background image when loading a menu.");
					m_pxBackgroundImage->Shutdown();
					delete m_pxBackgroundImage;
					m_pxBackgroundImage = nullptr;
					return false;
				}				
			}

			tinyxml2::XMLElement* pxMiniMapElement = nullptr;
			pxMiniMapElement = pxMenuElement->FirstChildElement("MiniMap");
			if (pxMiniMapElement)
			{
				// TODO-OJ : The size and position should be specified in the menu file rather than hard coded
				m_pxMiniMap = new Ox93_MiniMap;
				bool bSuccess = m_pxMiniMap->Init();
				Ox93_Assert(bSuccess, "Initialization of minimap failed.");
			}

			tinyxml2::XMLElement* pxButtonElement = nullptr;
			pxButtonElement = pxMenuElement->FirstChildElement("Button");
			while (pxButtonElement)
			{
				const char* pszString = pxButtonElement->Attribute("string");
				const char* pszFunctionString = pxButtonElement->Attribute("function");
				VoidFunction pfnCommand = Ox93_System::GetCommand(pszFunctionString);

				const char* pszTexture = pxButtonElement->Attribute("image");
				Ox93_Hash uTextureHash = GetHash(pszTexture);
				
				u_int uWidth = pxButtonElement->UnsignedAttribute("width");
				u_int uHeight = pxButtonElement->UnsignedAttribute("height");

				const char* pszValueString = pxButtonElement->Attribute("value");
				const void* pValue = Ox93_System::GetValue(pszValueString);

				Ox93_MenuButton* pxNewButton = new Ox93_MenuButton(pszString, pValue, pfnCommand, uWidth, uHeight, uTextureHash);

				tinyxml2::XMLElement* pxPositionElement = nullptr;
				pxPositionElement = pxButtonElement->FirstChildElement("Position");
				if (pxPositionElement)
				{
					const int iPosX = pxPositionElement->IntAttribute("x");
					const int iPosY = pxPositionElement->IntAttribute("y");

					pxNewButton->SetPosition(iPosX, iPosY);
				}

				m_lpxButtons.push_back(pxNewButton);

				pxButtonElement = pxButtonElement->NextSiblingElement("Button");
			}

			tinyxml2::XMLElement* pxListElement = nullptr;
			pxListElement = pxMenuElement->FirstChildElement("List");
			while (pxListElement)
			{
				const char* pszString = pxListElement->Attribute("string");

				const char* pszTexture = pxListElement->Attribute("image");
				Ox93_Hash uTextureHash = GetHash(pszTexture);

				u_int uWidth = pxListElement->UnsignedAttribute("width");
				u_int uHeight = pxListElement->UnsignedAttribute("height");

				const char* pszFunctionString = pxListElement->Attribute("function");
				ListFunction pfnFunction = Ox93_System::GetListFunction(pszFunctionString);

				const char* pszPopFuncString = pxListElement->Attribute("populatefunction");
				PopulationFunction pfnPopulationFunction = Ox93_System::GetPopulationFunction(pszPopFuncString);

				tinyxml2::XMLElement* pxPositionElement = nullptr;
				pxPositionElement = pxListElement->FirstChildElement("Position");
				if (pxPositionElement)
				{
					const int iPosX = pxPositionElement->IntAttribute("x");
					int iPosY = pxPositionElement->IntAttribute("y");

					u_int u = 0;
					const void* pValue = pfnPopulationFunction(u);

					while (pValue != nullptr)
					{
						Ox93_MenuButton* pxNewButton = new Ox93_MenuButton(pszString, pValue, pfnFunction, uWidth, uHeight, uTextureHash, u);

						pxNewButton->SetPosition(iPosX, iPosY);

						m_lpxButtons.push_back(pxNewButton);

						iPosY -= uHeight;
						u++;
						pValue = pfnPopulationFunction(u);
					}
				}

				pxListElement = pxListElement->NextSiblingElement("List");
			}

			return true;
		}
	}
	return false;
}

void Ox93_Menu::HandleInput()
{
	if (Ox93_System::GetGameMode() != OX93_GAMEMODE_PLAYER && Ox93_InputSystem::GetLMBClicked())
	{
		Ox93_Vector_2 xClickPos = Ox93_InputSystem::GetMouseClickLocation();
		// Translate the screen coords into the window coords
		HWND hWnd = Ox93_System::GetWindow();
		RECT xRect;
		GetWindowRect(hWnd, &xRect);
		// OJ - find the client size of the window
		RECT xZeroRect = { 0,0,0,0 };
		AdjustWindowRect(&xZeroRect, WS_OVERLAPPEDWINDOW, false);
		xRect.left -= xZeroRect.left;
		xRect.right -= xZeroRect.right;
		xRect.top -= xZeroRect.top;
		xRect.bottom -= xZeroRect.bottom;

		xClickPos.x -= xRect.left + (xRect.right - xRect.left)/2;
		xClickPos.y -= xRect.top + (xRect.bottom - xRect.top)/2;
		xClickPos.y = -xClickPos.y;

		// Loop through all buttons and see if they've been clicked on
		std::list<Ox93_MenuButton*>::const_iterator xIter;
		for (xIter = m_lpxButtons.begin(); xIter != m_lpxButtons.end(); ++xIter)
		{
			Ox93_MenuButton* pxButton = *xIter;
			if (pxButton)
			{
				if (xClickPos.x >= pxButton->GetPosition().x && xClickPos.x <= pxButton->GetPosition().x + pxButton->GetWidth()
					&& xClickPos.y <= pxButton->GetPosition().y && xClickPos.y >= pxButton->GetPosition().y - pxButton->GetHeight())
				{
					pxButton->OnPress();
				}
			}
		}
	}
	// Loop through again and update all the buttons
	std::list<Ox93_MenuButton*>::const_iterator xIter;
	for (xIter = m_lpxButtons.begin(); xIter != m_lpxButtons.end(); ++xIter)
	{
		Ox93_MenuButton* pxButton = *xIter;
		if (pxButton)
		{
			pxButton->Update();
		}
	}
}
