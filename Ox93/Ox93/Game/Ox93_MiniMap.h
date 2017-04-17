#pragma once

#ifndef OX93_MINIMAP_H__
#define OX93_MINIMAP_H__

// Includes...
#include "Ox93_Core.h"

// Forward Declarations...
class Ox93_BitMap;
class Ox93_RenderTexture;

class Ox93_MiniMap
{
public:
	Ox93_MiniMap();
	~Ox93_MiniMap();

	bool Init();
	void Update();

private:
	Ox93_BitMap* m_pxBitMap;
	Ox93_RenderTexture* m_pxRenderTexture;
};

#endif // OX93_MINIMAP_H__
