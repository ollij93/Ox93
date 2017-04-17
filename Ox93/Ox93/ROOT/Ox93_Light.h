#pragma once

#ifndef __OX93_LIGHT_H__
#define __OX93_LIGHT_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Graphics/2D/Ox93_RenderTexture.h"
#include "ROOT/Math/Ox93_Math.h"

class Ox93_Light
{
public:
	Ox93_Light();
	~Ox93_Light();
	bool InitRenderTexture();

	void Render();

	void SetAmbientColor(float fRed, float fGreen, float fBlue, float fAlpha);
	void SetDiffuseColor(float fRed, float fGreen, float fBlue, float fAlpha);
	void SetPosition(float fX, float fY, float fZ);
	void SetDirection(float fX, float fY, float fZ);

	Ox93_Color GetAmbientColor() const { return m_xAmbientColor; }
	Ox93_Color GetDiffuseColor() const { return m_xDiffuseColor; }
	Ox93_Vector_3 GetPosition() const { return m_xPosition; }
	Ox93_Vector_3 GetDirection() const { return m_xDirection; }
	ID3D11ShaderResourceView* GetShaderResource() const { return m_pxRenderTexture ? m_pxRenderTexture->GetShaderResourceView() : nullptr; }
	void GetViewMatrix(DirectX::XMMATRIX& xViewMatrix) const { xViewMatrix = m_xViewMatrix; }
	void GetProjectionMatrix(DirectX::XMMATRIX& xProjectionMatrix) const { xProjectionMatrix = m_xOrthoMatrix; }

private:
	DirectX::XMMATRIX m_xViewMatrix;
	DirectX::XMMATRIX m_xOrthoMatrix;
	Ox93_Color m_xAmbientColor;
	Ox93_Color m_xDiffuseColor;
	Ox93_Vector_3 m_xPosition;
	Ox93_Vector_3 m_xDirection;
	Ox93_RenderTexture* m_pxRenderTexture;
};

#endif // ifndef __OX93_LIGHT_H__