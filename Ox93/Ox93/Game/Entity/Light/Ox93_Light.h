#pragma once

#ifndef __OX93_LIGHT_H__
#define __OX93_LIGHT_H__

// Includes...
#include "Ox93_Core.h"
#include "Game/Entity/Ox93_Entity.h"
#include "ROOT/Graphics/2D/Ox93_RenderTexture.h"
#include "ROOT/Math/Ox93_Math.h"

class Ox93_Light : public Ox93_Entity
{
public:
	Ox93_Light(u_int uClassification);
	~Ox93_Light();
	static Ox93_Entity* Create() { return new Ox93_Light(OX93_CLASS_LIGHT); }

	bool InitRenderTexture();

	void Render();

	// Overrides...
	virtual void Update() override;
	virtual void ReadFromChunkStream(const Ox93_ChunkStream& xChunkStream) override;
	virtual void WriteToChunkStream(Ox93_ChunkStream& xChunkStream) const override;

	// Getters & Setters...
	virtual void SetPosition(Ox93_Vector_3 xPosition) override;
	virtual void SetOrientation(Ox93_Matrix3x3 xOrientation) override;
	void SetAmbientColor(float fRed, float fGreen, float fBlue, float fAlpha);
	void SetDiffuseColor(float fRed, float fGreen, float fBlue, float fAlpha);

	Ox93_Color GetAmbientColor() const { return m_xAmbientColor; }
	Ox93_Color GetDiffuseColor() const { return m_xDiffuseColor; }
	ID3D11ShaderResourceView* GetShaderResource() const { return m_pxRenderTexture ? m_pxRenderTexture->GetShaderResourceView() : nullptr; }
	void GetViewMatrix(DirectX::XMMATRIX& xViewMatrix) const { xViewMatrix = m_xViewMatrix; }
	void GetProjectionMatrix(DirectX::XMMATRIX& xProjectionMatrix) const { xProjectionMatrix = m_xOrthoMatrix; }

	// Statics ...
	static void RenderList();

	// Static Getters...
	static const std::list<Ox93_Light*> GetList() { return s_lpxLightsList; }

protected:
	// Statics...
	static void AddToLightList(Ox93_Light* pxLight) { s_lpxLightsList.push_back(pxLight); }
	static void RemoveFromLightList(Ox93_Light* pxLight) { s_lpxLightsList.remove(pxLight); }

private:
	DirectX::XMMATRIX m_xViewMatrix;
	DirectX::XMMATRIX m_xOrthoMatrix;
	Ox93_Color m_xAmbientColor;
	Ox93_Color m_xDiffuseColor;
	Ox93_RenderTexture* m_pxRenderTexture;
	bool m_bFollowPlayer;

	// Statics...
	static std::list<Ox93_Light*> s_lpxLightsList;

	typedef Ox93_Entity PARENT;
};

#endif // ifndef __OX93_LIGHT_H__
