// Includes...
#include "Ox93_Specification_System.h"
#include "ROOT/Specification/Ox93_Specification.h"

// TinyXml
#include "TinyXML/tinyxml2.h"

// Statics...
Ox93_Specification_System* Ox93_Specification_System::s_pxThis = nullptr;

Ox93_Specification_System::Ox93_Specification_System()
: m_xSpecifications()
{
}

Ox93_Specification_System::~Ox93_Specification_System()
{
}

bool Ox93_Specification_System::Create()
{
	Ox93_Assert(!s_pxThis, "Attempting to re-create the specification system.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_Specification_System;
	return s_pxThis->Init();
}

void Ox93_Specification_System::Destroy()
{
	if (s_pxThis)
	{
		s_pxThis->Shutdown();
		delete s_pxThis;
		s_pxThis = nullptr;
	}
}

const Ox93_Specification* Ox93_Specification_System::GetSpecificationByHash(Ox93_Hash uSpecHash)
{
	if (s_pxThis)
	{
		for (u_int u = 0; u < s_pxThis->m_xSpecifications.size(); u++)
		{
			if (s_pxThis->m_xSpecifications[u] && 
				s_pxThis->m_xSpecifications[u]->m_uHash != OX93_HASH_UNSET &&
				s_pxThis->m_xSpecifications[u]->m_uHash == uSpecHash)
			{
				return s_pxThis->m_xSpecifications[u];
			}
		}
	}
	return nullptr;
}

bool Ox93_Specification_System::Init()
{
	WIN32_FIND_DATA xFindData;
	HANDLE hFile = FindFirstFile(L"Specs/*.spec", &xFindData);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		do {
			if (!(xFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				WCHAR* pwszFilename = new WCHAR[256];
				pwszFilename[0] = L'S';
				pwszFilename[1] = L'p';
				pwszFilename[2] = L'e';
				pwszFilename[3] = L'c';
				pwszFilename[4] = L's';
				pwszFilename[5] = L'/';
				int i;
				for (i = 0; xFindData.cFileName[i]; i++)
				{
					if (i >= 250)
					{
						Ox93_Assert(false, "String copying failed, filename too long?");
						break;
					}

					pwszFilename[i + 6] = xFindData.cFileName[i];
				}
				pwszFilename[i + 6] = 0;

				LoadSpecsFromFile(pwszFilename);
			}
		} while (FindNextFile(hFile, &xFindData));
		FindClose(hFile);
	}

	return true;
}

void Ox93_Specification_System::Shutdown()
{
	while (m_xSpecifications.size() > 0)
	{
		Ox93_Specification* pxSpec = m_xSpecifications.back();
		pxSpec->Shutdown();
		delete pxSpec;
		m_xSpecifications.pop_back();
	}
}

void Ox93_Specification_System::AddSpecificationToList(Ox93_Specification* pxSpec)
{
	Ox93_Assert(s_pxThis, "Null this pointer when trying to add a specification to the list.");
	if (s_pxThis)
	{
		s_pxThis->m_xSpecifications.push_back(pxSpec);
	}
}

void Ox93_Specification_System::LoadSpecsFromFile(const WCHAR* pwszFilename)
{
	FILE* pxFile;
	_wfopen_s(&pxFile, pwszFilename, L"rb");
	Ox93_Assert(pxFile, "Failed to open file for loading specification.");
	if (pxFile)
	{
		tinyxml2::XMLDocument xSpecDoc;
		xSpecDoc.LoadFile(pxFile);
		
		tinyxml2::XMLElement* pxSpecElement;
		pxSpecElement = xSpecDoc.FirstChildElement("Specification");
		while (pxSpecElement)
		{
			Ox93_Specification* pxNewSpec = new Ox93_Specification;
			pxNewSpec->LoadParamsFromElement(pxSpecElement);

			pxSpecElement = pxSpecElement->NextSiblingElement("Specification");
		}
	}

	fclose(pxFile);
}