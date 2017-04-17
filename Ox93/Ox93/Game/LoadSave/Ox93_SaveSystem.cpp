
// TinyXML
#include "TinyXML/tinyxml2.h"

// Includes...
#include "Ox93_SaveSystem.h"
#include "Game/Entity/Ox93_Entity.h"
#include "Game/Inventory/Ox93_Inventory.h"
#include "Game/LoadSave/ChunkStream/Ox93_ChunkStream.h"
#include "Game/World/Terrain/Ox93_TerrainSystem.h"
#include "ROOT/Graphics/Ox93_D3D.h"

// Statics...
Ox93_SaveSystem* Ox93_SaveSystem::s_pxThis = nullptr;

Ox93_SaveSystem::Ox93_SaveSystem()
{
}

Ox93_SaveSystem::~Ox93_SaveSystem()
{
}

bool Ox93_SaveSystem::Create()
{
	Ox93_Assert(!s_pxThis, "Attempting to recreate the savesystem.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_SaveSystem;
	return s_pxThis->Init();
}

void Ox93_SaveSystem::Destroy()
{
	if (s_pxThis)
	{
		s_pxThis->Shutdown();
		delete s_pxThis;
		s_pxThis = nullptr;
	}
}

bool Ox93_SaveSystem::Init()
{
	return true;
}

void Ox93_SaveSystem::Shutdown()
{
}

void Ox93_SaveSystem::SaveCurrentWorldToFile(const WCHAR* pwszFilename)
{
	FILE* pxSaveFile;
	_wfopen_s(&pxSaveFile, pwszFilename, L"wb");

	Ox93_Assert(pxSaveFile, "Failed to open file for saving!");
	if (pxSaveFile)
	{
		// Prepare the file
		tinyxml2::XMLDocument xSaveDoc;

		// Prepare the world element
		tinyxml2::XMLElement* pxWorldElement = xSaveDoc.NewElement("World");
		if(pxWorldElement)
		{
			// Set the seed
			pxWorldElement->SetAttribute("seed", Ox93_TerrainSystem::GetCurrentSeed());

			// Go through all entities
			const std::list<Ox93_Entity*> lpxEntities = Ox93_Entity::GetList();
			std::list<Ox93_Entity*>::const_iterator xIter;
			for (xIter = lpxEntities.begin(); xIter != lpxEntities.end(); ++xIter)
			{
				const Ox93_Entity* pxEntity = *xIter;
				if (!pxEntity || pxEntity->GetClassification() == OX93_CLASS_SELECTIONOBJECT) { continue; }

				// Create element and set attributes
				tinyxml2::XMLElement* pxEntityElement = xSaveDoc.NewElement("Entity");
				pxEntityElement->SetAttribute("class", pxEntity->GetClassification());
				pxEntityElement->SetAttribute("specification", pxEntity->GetSpecificationHash());

				// Save values
				Ox93_ChunkStream xSaveStream(pxEntityElement, &xSaveDoc);
				pxEntity->WriteToChunkStream(xSaveStream);

				// Link the entity element as a child of the world element
				pxWorldElement->LinkEndChild(pxEntityElement);
			}

			// Link the world element to the save document
			xSaveDoc.LinkEndChild(pxWorldElement);
		}		

		xSaveDoc.SaveFile(pxSaveFile);

		xSaveDoc.Clear();
	}

	fclose(pxSaveFile);
}

void Ox93_SaveSystem::SavePlayerSettings(const WCHAR* pwszFilename)
{
	FILE* pxFile;
	_wfopen_s(&pxFile, pwszFilename, L"wb");

	Ox93_Assert(pxFile, "Failed to open file for saving player settings");
	if (pxFile)
	{
		tinyxml2::XMLDocument xDoc;

		tinyxml2::XMLElement* pxElement = xDoc.NewElement("FOV");
		pxElement->SetAttribute("value", Ox93_D3D::GetFOV());

		xDoc.LinkEndChild(pxElement);

		xDoc.SaveFile(pxFile);
		xDoc.Clear();
	}

	fclose(pxFile);
}

void Ox93_SaveSystem::SaveInventoryToFile(Ox93_Inventory* pxInv, const WCHAR* pwszFilename)
{
	if (!pxInv) { return; }

	pxInv->SaveToFile(pwszFilename);
}