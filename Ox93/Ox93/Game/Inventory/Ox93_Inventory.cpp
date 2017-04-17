// Includes...
#include "Ox93_Inventory.h"
#include "Game/Inventory/Ox93_InventoryItem.h"
#include "TinyXML/tinyxml2.h"

// Statics...
Ox93_Inventory* Ox93_Inventory::s_pxPlayerInventory = nullptr;

Ox93_Inventory::Ox93_Inventory()
: m_lpxItemList()
, m_pxSelectedItem(nullptr)
{
}

Ox93_Inventory::~Ox93_Inventory()
{
}

void* Ox93_Inventory::PlayerInventoryItemName(u_int uItem)
{
	if (s_pxPlayerInventory && uItem < s_pxPlayerInventory->m_lpxItemList.size())
	{
		std::list<Ox93_InventoryItem*>::const_iterator xIter;
		u_int uCurrentItem = 0;
		for (xIter = s_pxPlayerInventory->m_lpxItemList.begin(); xIter != s_pxPlayerInventory->m_lpxItemList.end(); ++xIter)
		{
			if (uCurrentItem == uItem)
			{
				Ox93_InventoryItem* pxItem = *xIter;
				if (pxItem)
				{
					return &(pxItem->m_pszName);
				}
				break;
			}
			uCurrentItem++;
		}
	}
	return nullptr;
}

void* Ox93_Inventory::PlayerInventoryItemQuantity(u_int uItem)
{
	if (s_pxPlayerInventory && uItem < s_pxPlayerInventory->m_lpxItemList.size())
	{
		std::list<Ox93_InventoryItem*>::const_iterator xIter;
		u_int uCurrentItem = 0;
		for (xIter = s_pxPlayerInventory->m_lpxItemList.begin(); xIter != s_pxPlayerInventory->m_lpxItemList.end(); ++xIter)
		{
			if (uCurrentItem == uItem)
			{
				Ox93_InventoryItem* pxItem = *xIter;
				if (pxItem)
				{
					return &(pxItem->m_uQuantity);
				}
				break;
			}
			uCurrentItem++;
		}
	}
	return nullptr;
}

void Ox93_Inventory::PlayerSelectItem(int iItem)
{
	if (!s_pxPlayerInventory) { return; }

	// OJ - If negetive value is given then deselect current item
	if (iItem < 0)
	{
		if (s_pxPlayerInventory->m_pxSelectedItem)
		{
			s_pxPlayerInventory->m_pxSelectedItem->OnDeselect();
			s_pxPlayerInventory->m_pxSelectedItem = nullptr;
			return;
		}
	}

	Ox93_Assert(iItem < (int)s_pxPlayerInventory->m_lpxItemList.size(), "Attempting to select an item out of range.");
	if (iItem < s_pxPlayerInventory->m_lpxItemList.size() && iItem >= 0)
	{
		std::list<Ox93_InventoryItem*>::const_iterator xIter;
		xIter = s_pxPlayerInventory->m_lpxItemList.begin();
		for (u_int u = 0; u < iItem; u++)
		{
			xIter++;
		}

		if (s_pxPlayerInventory->m_pxSelectedItem)
		{
			s_pxPlayerInventory->m_pxSelectedItem->OnDeselect();
		}

		s_pxPlayerInventory->m_pxSelectedItem = *xIter;
		s_pxPlayerInventory->m_pxSelectedItem->OnSelect();
	}
}

void Ox93_Inventory::ActivateCurrentItem()
{
	if (m_pxSelectedItem)
	{
		m_pxSelectedItem->OnActivate();
	}
}

void Ox93_Inventory::UpdateCurrentItem()
{
	if (m_pxSelectedItem)
	{
		m_pxSelectedItem->Update();
	}
}

void Ox93_Inventory::GiveItem(Ox93_Hash uItemHash, u_int uQuantity)
{
	// Check if the inventory already contains this item and if so just add the quantity to its current value
	std::list<Ox93_InventoryItem*>::const_iterator xIter;
	for (xIter = m_lpxItemList.begin(); xIter != m_lpxItemList.end(); ++xIter)
	{
		Ox93_InventoryItem* pxIterItem = *xIter;
		if (pxIterItem && pxIterItem->m_uItemHash == uItemHash)
		{
			pxIterItem->m_uQuantity += uQuantity;
			return;
		}
	}

	// Otherwise we create the item and add it to the list
	Ox93_InventoryItem* pxItem = new Ox93_InventoryItem;
	pxItem->InitFromSpecification(uItemHash);
	pxItem->m_uQuantity = uQuantity;
	m_lpxItemList.push_back(pxItem);
}

void Ox93_Inventory::SaveToFile(const WCHAR* pwszFilename)
{
	FILE* pxFile;
	_wfopen_s(&pxFile, pwszFilename, L"wb");

	Ox93_Assert(pxFile, "Failed to open file for saving inventory");
	if (pxFile)
	{
		tinyxml2::XMLDocument xDoc;

		tinyxml2::XMLElement* pxInventoryElement = xDoc.NewElement("Inventory");
		if (pxInventoryElement)
		{
			std::list<Ox93_InventoryItem*>::const_iterator xIter;
			for (xIter = m_lpxItemList.begin(); xIter != m_lpxItemList.end(); ++xIter)
			{
				Ox93_InventoryItem* pxItem = *xIter;
				if (!pxItem) { continue; }

				tinyxml2::XMLElement* pxItemElement;
				pxItemElement = xDoc.NewElement("Item");
				if (pxItemElement)
				{
					pxItemElement->SetAttribute("specification", pxItem->m_uItemHash);
					pxItemElement->SetAttribute("quantity", pxItem->m_uQuantity);

					pxInventoryElement->LinkEndChild(pxItemElement);
				}
			}
		}

		xDoc.LinkEndChild(pxInventoryElement);

		xDoc.SaveFile(pxFile);
		xDoc.Clear();
	}
}