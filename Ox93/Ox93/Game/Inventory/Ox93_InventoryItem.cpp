// Includes...
#include "Ox93_InventoryItem.h"
#include "Game/Entity/PhysicalObject/Character/Ox93_Character.h"
#include "Game/Entity/SelectionObject/Ox93_SelectionObject.h"
#include "Game/World/Terrain/Ox93_TerrainSystem.h"
#include "ROOT/Specification/Ox93_Specification.h"
#include "ROOT/Specification/Ox93_Specification_System.h"

// Global Variables...
static const float fOX93_SELECTION_MAX_RANGE = 25.f;
static const float fOX93_SELECTION_MIN_RANGE = 2.f;


Ox93_InventoryItem::Ox93_InventoryItem()
: m_uQuantity(0)
, m_pszName()
, m_uItemHash(OX93_HASH_UNSET)
, m_uObjectSpecHash(OX93_HASH_UNSET)
, m_pxSelectionObject(nullptr)
{
}

Ox93_InventoryItem::~Ox93_InventoryItem()
{
	if (m_pxSelectionObject)
	{
		delete m_pxSelectionObject;
		m_pxSelectionObject = nullptr;
	}
	if (m_pszName)
	{
		delete m_pszName;
		m_pszName = nullptr;
	}
}

void Ox93_InventoryItem::InitFromSpecification(Ox93_Hash uItemHash)
{
	const Ox93_Specification* pxSpec = Ox93_Specification_System::GetSpecificationByHash(uItemHash);
	if (pxSpec)
	{
		m_uItemHash = uItemHash;

		m_pszName = pxSpec->GetName();
		m_uObjectSpecHash = pxSpec->GetValueAsHash(uOX93_SPEC_PARAM_OBJECT, OX93_HASH_UNSET);
	}
}

void Ox93_InventoryItem::OnSelect()
{
	const Ox93_Specification* pxSpec = Ox93_Specification_System::GetSpecificationByHash(m_uObjectSpecHash);
	if (pxSpec)
	{
		m_pxSelectionObject = new Ox93_SelectionObject(OX93_CLASS_SELECTIONOBJECT);
		m_pxSelectionObject->InitFromSpecification(pxSpec);
	}
}
void Ox93_InventoryItem::OnDeselect()
{
	if (m_pxSelectionObject)
	{
		delete m_pxSelectionObject;
		m_pxSelectionObject = nullptr;
	}
}

void Ox93_InventoryItem::OnActivate()
{
	if (m_pxSelectionObject && m_uQuantity > 0)
	{
		Ox93_PhysicalObject* pxObject = m_pxSelectionObject->CreatePhysicalObject();
		if (pxObject)
		{
			bool bSuccess = Ox93_TerrainSystem::AddPhysicalObject(pxObject);
			if (bSuccess)
			{
				m_uQuantity -= 1;
			}
			else
			{
				delete pxObject;
				pxObject = nullptr;
			}
		}
	}
}

void Ox93_InventoryItem::Update()
{
	const Ox93_Character* pxPlayer = Ox93_Character::GetLocalPlayer();
	if (pxPlayer)
	{
		Ox93_Vector_3 xEyePos = pxPlayer->GetEyePosition();
		Ox93_Matrix3x3 xEyeOri = pxPlayer->GetEyeOrientation();
		Ox93_Vector_3 xEyeDir(xEyeOri.e20, xEyeOri.e21, xEyeOri.e22);
		xEyeDir.y = 0.f;
		xEyeDir.Normalize();
		xEyeDir *= 6.f;

		//Ox93_Vector_3 xPlacePoint = xEyePos + xEyeDir;
		//xPlacePoint = Ox93_TerrainSystem::GetClosestCentre(xPlacePoint);

		//if(m_pxSelectionObject)
		//{
		//	m_pxSelectionObject->SetPosition(xPlacePoint);
		//	m_pxSelectionObject->Unhide();

		//	// If no items remain hide the object
		//	if (m_uQuantity == 0)
		//	{
		//		m_pxSelectionObject->Hide();
		//	}
		//}

	}
}