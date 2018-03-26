#pragma once

#ifndef OX93_MINIMAP_H__
#define OX93_MINIMAP_H__

// Forward Declarations...
class Ox93_BitMap;
class Ox93_Camera;
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
	Ox93_Camera* m_pxCamera;
};

#endif // OX93_MINIMAP_H__
