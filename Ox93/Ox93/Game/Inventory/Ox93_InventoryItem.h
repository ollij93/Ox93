#pragma once

#ifndef OX93_INVENTORYITEM_H__
#define OX93_INVENTORYITEM_H__

// Includes...
#include "Types/Ox93_Hash.h"

// Forward Declarations...
class Ox93_Specification;
class Ox93_SelectionObject;

class Ox93_InventoryItem
{
	friend class Ox93_Inventory;

public:
	Ox93_InventoryItem();
	~Ox93_InventoryItem();

	void InitFromSpecification(Ox93_Hash uItemHash);

	void Update();
	void OnSelect();
	void OnDeselect();
	void OnActivate();

private:
	u_int m_uQuantity;
	const char* m_pszName;
	Ox93_Hash m_uItemHash;
	Ox93_Hash m_uObjectSpecHash;
	Ox93_SelectionObject* m_pxSelectionObject;
};

#endif // OX93_INVENTORYITEM_H__

