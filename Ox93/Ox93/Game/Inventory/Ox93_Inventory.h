#pragma once

#ifndef OX93_INVENTORY_H__
#define OX93_INVENTORY_H__

// Includes...
#include <list>
#include "ROOT/Assert/Ox93_Assert.h"
#include "Types/Ox93_Hash.h"

// Forward Declarations...
class Ox93_InventoryItem;

class Ox93_Inventory
{
public:
	Ox93_Inventory();
	~Ox93_Inventory();

	static void SetPlayerInventory(Ox93_Inventory* pxInventory) { Ox93_Assert(!s_pxPlayerInventory, "Overwriting player inventory."); s_pxPlayerInventory = pxInventory; }

	static void* PlayerInventoryItemName(u_int uItem);
	static void* PlayerInventoryItemQuantity(u_int uItem);

	static void PlayerSelectItem(int iItem);

	void ActivateCurrentItem();
	void UpdateCurrentItem();

	void GiveItem(Ox93_Hash uItemHash, u_int uQuantity);

	void SaveToFile(const WCHAR* pwszFilename);

private:
	std::list<Ox93_InventoryItem*> m_lpxItemList;
	Ox93_InventoryItem* m_pxSelectedItem;

	static Ox93_Inventory* s_pxPlayerInventory;
};

#endif // OX93_INVENTORY_H__