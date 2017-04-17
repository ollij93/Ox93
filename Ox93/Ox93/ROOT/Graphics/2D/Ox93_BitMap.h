#pragma once

#ifndef OX93_BITMAP_H__
#define OX93_BITMAP_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Ox93_TextureHandler.h"

// Forward Declarations...
struct Ox93_Vertex;

class Ox93_BitMap
{
public:
	Ox93_BitMap();
	~Ox93_BitMap() {}

	bool Init(Ox93_Hash uTextureHash, u_int uWidth, u_int uHeight, float fTextureLeft = 0.f, float fTextureRight = 1.f, float fTextureTop = 0.f, float fTextureBottom = 1.f);
	bool Init(ID3D11ShaderResourceView* pxTexture, u_int uWidth, u_int uHeight, float fTextureLeft = 0.f, float fTextureRight = 1.f, float fTextureTop = 0.f, float fTextureBottom = 1.f);
	void Shutdown();

	void Render();
	ID3D11ShaderResourceView* GetTexture() { return m_pxTexture ? m_pxTexture : Ox93_TextureHandler::GetTextureByHash(m_uTextureHash); }

	Ox93_Vector_2 GetPosition() const { return Ox93_Vector_2(m_iPosX, m_iPosY); }
	void SetPosition(int iPosX, int iPosY) { m_bMovedSinceRender = true; m_iPosX = iPosX; m_iPosY = iPosY; }

	u_int GetHeight() const { return m_uHeight; }
	u_int GetWidth() const { return m_uWidth; }

protected:
	bool InitBuffers();
	bool UpdateBuffers();
	void RenderBuffers();
	void ShutdownBuffers();

private:
	Ox93_BitMap(const Ox93_BitMap& xBitMap) {}
	
	Ox93_Hash m_uTextureHash;
	ID3D11ShaderResourceView* m_pxTexture;
	u_int m_uWidth;
	u_int m_uHeight;
	float m_fTextureLeft;
	float m_fTextureRight;
	float m_fTextureTop;
	float m_fTextureBottom;

	int m_iPosX;
	int m_iPosY;

	bool m_bMovedSinceRender;

	ID3D11Buffer* m_pxVertexBuffer;
	ID3D11Buffer* m_pxIndexBuffer;

	static std::list<Ox93_BitMap*> s_lpxBitMapList;
};

#endif // OX93_BITMAP_H__

