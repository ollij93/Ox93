#pragma once
#ifndef __OX93_MODEL_H__
#define __OX93_MODEL_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Graphics/Ox93_RenderSystem.h"

class Ox93_Model
{
public:
	Ox93_Model();
	~Ox93_Model();

	bool Init(Ox93_Hash uModelHash, Ox93_Hash uTextureHash);
	void Shutdown();
	void Render();

	ID3D11ShaderResourceView* GetTexture() const { return Ox93_TextureHandler::GetTextureByHash(m_uTextureHash); }

	u_int GetIndexCount() const { return m_uIndexCount; }

private:
	bool InitBuffers();
	void ShutdownBuffers();

	bool LoadModel(Ox93_Hash uModelHash);
	void ReleaseModel();

	Ox93_Vertex* m_pxVertices;
	u_int m_uVertexCount;
	u_int m_uIndexCount;

	ID3D11Buffer* m_pxVertexBuffer;
	ID3D11Buffer* m_pxIndexBuffer;

	Ox93_Hash m_uTextureHash;
};

#endif // ifndef __OX93_MODEL_H__
