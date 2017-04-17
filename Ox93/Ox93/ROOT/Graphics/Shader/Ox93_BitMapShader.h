#pragma once

#ifndef OX93_BITMAPSHADER_H__
#define OX93_BITMAPSHADER_H__

// Includes...
#include "Ox93_Core.h"

// Forward Declarations...
class Ox93_BitMap;

class Ox93_BitMapShader
{
public:
	static bool Create();
	static void Destroy();

	static bool Render();

	static void AddToRenderList(Ox93_BitMap* pxBitMap) { if (pxBitMap) { s_lpxBitMapsToRender.push_back(pxBitMap); } }
	static void RemoveFromRenderList(Ox93_BitMap* pxBitMap) { s_lpxBitMapsToRender.remove(pxBitMap); }

protected:
	Ox93_BitMapShader();
	~Ox93_BitMapShader();

	bool InitShader(WCHAR* pwszVSFilename, WCHAR* pwszPSFilename);

	bool SetShaderParameters(ID3D11DeviceContext* pxDeviceContext, const DirectX::XMMATRIX& xWorldMatrix, const DirectX::XMMATRIX& xViewMatrix, const DirectX::XMMATRIX& xOrthoMatrix, ID3D11ShaderResourceView* pxTexture);
	void RenderShader(ID3D11DeviceContext* pxDeviceContext);

private:
	struct BitMapMatrixBuffer
	{
		DirectX::XMMATRIX xWorld;
		DirectX::XMMATRIX xView;
		DirectX::XMMATRIX xOrtho;
	};

	ID3D11VertexShader* m_pxVertexShader;
	ID3D11PixelShader* m_pxPixelShader;
	ID3D11InputLayout* m_pxLayout;
	ID3D11Buffer* m_pxMatrixBuffer;
	ID3D11SamplerState* m_pxSampleState;

	static std::list<Ox93_BitMap*> s_lpxBitMapsToRender;

	static Ox93_BitMapShader* s_pxThis;
};

#endif // OX93_BITMAPSHADER_H__