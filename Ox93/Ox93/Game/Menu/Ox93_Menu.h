#pragma once

#ifndef OX93_MENU_H__
#define OX93_MENU_H__

// Includes...
#include "Game/Menu/Ox93_MenuButton.h"

// Forward Declarations...
class Ox93_BitMap;
class Ox93_MiniMap;

class Ox93_Menu
{
	friend class Ox93_MenuSystem;
public:
	void HandleInput();
	void Update();

protected:
	Ox93_Menu();
	~Ox93_Menu();
	bool LoadFromFile(const WCHAR* pwszFilename);

private:
	Ox93_Hash m_uNameHash;
	Ox93_BitMap* m_pxBackgroundImage;
	Ox93_MiniMap* m_pxMiniMap;
	std::list<Ox93_MenuButton*> m_lpxButtons;
};

#endif // OX93_MENU_H__
