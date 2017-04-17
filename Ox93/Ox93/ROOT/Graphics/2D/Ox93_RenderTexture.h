#pragma once

#ifndef OX93_RENDERTEXTURE_H__
#define OX93_RENDERTEXTURE_H__

// Includes...
#include "Ox93_Core.h"

class Ox93_RenderTexture
{
public:
	Ox93_RenderTexture();
	~Ox93_RenderTexture();

	bool Init(u_int uTextureWidth, u_int uTextureHeight);
	void SetAsRenderTarget();
	void ClearRenderTarget(float fR, float fG, float fB, float fAlpha);
	void UpdateShaderResource();

	ID3D11ShaderResourceView* GetShaderResourceView() { return m_pxShaderResourceView; }

private:
	ID3D11Texture2D* m_pxRenderTargetTexture;
	ID3D11RenderTargetView* m_pxRenderTargetView;
	ID3D11ShaderResourceView* m_pxShaderResourceViewMS;
	ID3D11ShaderResourceView* m_pxShaderResourceView;
	ID3D11DepthStencilView* m_pxDepthStencilView;
	ID3D11Texture2D* m_pxDepthStencilBuffer;
	D3D11_VIEWPORT m_xViewPort;
};

#endif
