
// TinyXml
#include "TinyXML/tinyxml2.h"

// Includes...
#include "Ox93_LoadSystem.h"
#include "Game/Entity/Ox93_Entity.h"
#include "Game/Entity/Ox93_EntityRegistry.h"
#include "Game/Entity/PhysicalObject/Ox93_PhysicalObject.h"
#include "Game/Inventory/Ox93_Inventory.h"
#include "Game/Inventory/Ox93_InventoryItem.h"
#include "Game/LoadSave/ChunkStream/Ox93_ChunkStream.h"
#include "Game/World/Terrain/Ox93_TerrainSystem.h"
#include "ROOT/Graphics/2D/Text/Ox93_Font.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Specification/Ox93_Specification_System.h"

// Statics...
Ox93_LoadSystem* Ox93_LoadSystem::s_pxThis = nullptr;

Ox93_LoadSystem::Ox93_LoadSystem()
{
}

Ox93_LoadSystem::~Ox93_LoadSystem()
{
}

bool Ox93_LoadSystem::Create()
{
	Ox93_Assert(!s_pxThis, "Attempting to recreate the loadsystem.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_LoadSystem;
	return s_pxThis->Init();
}

void Ox93_LoadSystem::Destroy()
{
	if (s_pxThis)
	{
		s_pxThis->Shutdown();
		delete s_pxThis;
		s_pxThis = nullptr;
	}
}

bool Ox93_LoadSystem::Init()
{
	// OJ - Find all the font files and load them
	WIN32_FIND_DATA xFontFind;
	HANDLE hFontFile = FindFirstFile(L"Fonts/*.font", &xFontFind);
	if (hFontFile != INVALID_HANDLE_VALUE)
	{
		do {
			if (!(xFontFind.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				WCHAR* pwszFilename = new WCHAR[256];
				pwszFilename[0] = L'F';
				pwszFilename[1] = L'o';
				pwszFilename[2] = L'n';
				pwszFilename[3] = L't';
				pwszFilename[4] = L's';
				pwszFilename[5] = L'/';
				int i;
				for (i = 0; xFontFind.cFileName[i]; i++)
				{
					if (i >= 250)
					{
						Ox93_Assert(false, "String copying failed, filename too long?");
						break;
					}

					pwszFilename[i + 6] = xFontFind.cFileName[i];
				}
				pwszFilename[i + 6] = 0;

				Ox93_Font::CreateFontFromFile(pwszFilename);
			}
		} while (FindNextFile(hFontFile, &xFontFind));
		FindClose(hFontFile);
	}

	return true;
}

void Ox93_LoadSystem::Shutdown()
{
}

void Ox93_LoadSystem::LoadDefault()
{
	if (!s_pxThis) { return; }

	FILE* pxFile;
	_wfopen_s(&pxFile, L"Saves/DefaultSave.ent", L"rb");
	Ox93_Assert(pxFile, "Failed to open file for loading entities.");
	if (pxFile)
	{
		tinyxml2::XMLDocument xEntDoc;
		xEntDoc.LoadFile(pxFile);

		tinyxml2::XMLElement* pxWorldElement = xEntDoc.FirstChildElement("World");
		if (pxWorldElement)
		{
			int iSeed = pxWorldElement->IntAttribute("seed");
			Ox93_TerrainSystem::Create(iSeed);

			s_pxThis->LoadEntitiesFromWorld(pxWorldElement);
		}
	}


	fclose(pxFile);
}

void Ox93_LoadSystem::LoadEntitiesFromWorld(tinyxml2::XMLElement* pxWorldElement)
{
	if (!pxWorldElement) { return; }

	tinyxml2::XMLElement* pxEntElement;
	pxEntElement = pxWorldElement->FirstChildElement("Entity");
	while (pxEntElement)
	{
		const u_int uEntityClassification = pxEntElement->UnsignedAttribute("class");
		const Ox93_Hash uSpecificationHash = pxEntElement->UnsignedAttribute("specification");

		Ox93_Entity* pxEntity = Ox93_EntityRegistry::CreateEntityFromClassification(uEntityClassification);
		if (pxEntity)
		{
			tinyxml2::XMLElement* pxMemberElement = pxEntElement->FirstChildElement("Member");

			Ox93_ChunkStream xChunkStream(pxMemberElement);
			pxEntity->ReadFromChunkStream(xChunkStream);

			const Ox93_Specification* pxSpecification = Ox93_Specification_System::GetSpecificationByHash(uSpecificationHash);
			pxEntity->InitFromSpecification(pxSpecification);

			if (pxEntity->GetClassification() == OX93_CLASS_PHYSICALOBJECT)
			{
				Ox93_TerrainSystem::AddPhysicalObject(static_cast<Ox93_PhysicalObject*>(pxEntity));
			}
		}

		pxEntElement = pxEntElement->NextSiblingElement("Entity");
	}

}

void Ox93_LoadSystem::LoadPlayerSettings(const WCHAR* pwszFilename)
{
	FILE* pxFile;
	_wfopen_s(&pxFile, pwszFilename, L"rb");
	Ox93_Assert(pxFile, "Failed to open file for loading player settings.");
	if (pxFile)
	{
		tinyxml2::XMLDocument xDoc;
		xDoc.LoadFile(pxFile);

		tinyxml2::XMLElement* pxElement;
		pxElement = xDoc.FirstChildElement("FOV");
		if (pxElement)
		{
			Ox93_D3D::SetFOV(pxElement->FloatAttribute("value"));
		}
	}

	fclose(pxFile);
}

void Ox93_LoadSystem::LoadInventoryFromFile(Ox93_Inventory* pxInv, const WCHAR* pwszFilename)
{
	Ox93_Assert(pxInv, "NULL passed as inventory for loading.");
	if (!pxInv) { return; }

	FILE* pxFile;
	_wfopen_s(&pxFile, pwszFilename, L"rb");
	Ox93_Assert(pxFile, "Failed to open file for loading inventory.");
	if (pxFile)
	{
		tinyxml2::XMLDocument xDoc;
		xDoc.LoadFile(pxFile);

		tinyxml2::XMLElement* pxInventoryElement;
		pxInventoryElement = xDoc.FirstChildElement("Inventory");
		if (pxInventoryElement)
		{
			tinyxml2::XMLElement* pxItemElement;
			pxItemElement = pxInventoryElement->FirstChildElement("Item");
			while (pxItemElement)
			{
				Ox93_Hash uItemHash = pxItemElement->UnsignedAttribute("specification");
				u_int uQuantity = pxItemElement->UnsignedAttribute("quantity");

				pxInv->GiveItem(uItemHash, uQuantity);

				pxItemElement = pxItemElement->NextSiblingElement("Item");
			}
		}
	}

	fclose(pxFile);
}