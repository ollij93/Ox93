#pragma once

#ifndef __OX93_LIGHTSHADER_H__
#define __OX93_LIGHTSHADER_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Math/Ox93_Math.h"

// Forward Declarations...
class Ox93_Entity_Renderable;
class Ox93_Light;

class Ox93_LightShader
{
public:
	static bool Create();
	static void Destroy();

	static bool Render(const Ox93_Light* pxLight);

	static void AddToRenderList(Ox93_Entity_Renderable* pxRenderable) { if (pxRenderable) { s_lpxEntitesToRender.push_back(pxRenderable); } }
	static void RemoveFromRenderList(Ox93_Entity_Renderable* pxRenderable) { s_lpxEntitesToRender.remove(pxRenderable); }

protected:
	Ox93_LightShader();
	~Ox93_LightShader() {}

private:
	bool InitShader(WCHAR* pwszVSFilename, WCHAR* pwszPSFilename);
	void ShutdownShader();
	
	bool SetShaderParameters(ID3D11DeviceContext* pxDeviceContext, 
		const DirectX::XMMATRIX& xWorldMatrix, 
		const DirectX::XMMATRIX& xViewMatrix, 
		const DirectX::XMMATRIX& xProjectionMatrix,
		const DirectX::XMMATRIX& xLightViewMatrix,
		const DirectX::XMMATRIX& xLightProjectionMatrix,
		ID3D11ShaderResourceView* pxTexture, 
		ID3D11ShaderResourceView* pxDepthTexture,
		Ox93_Vector_3 xLightDirection, 
		Ox93_Color xAmbientColor, 
		Ox93_Color xDiffuseColor);
	void RenderShader(ID3D11DeviceContext* pxDeviceContext, u_int uIndexCount);

	struct MatrixBufferType
	{
		DirectX::XMMATRIX xWorld;
		DirectX::XMMATRIX xView;
		DirectX::XMMATRIX xProjection;
		DirectX::XMMATRIX xLightView;
		DirectX::XMMATRIX xLightProjection;
	};

	struct LightBufferType
	{
		Ox93_Color xAmbientColor;
		Ox93_Color xDiffuseColor;
	};
	
	struct LightBufferType2
	{
		Ox93_Vector_3 xLightPosition;
		float fPADDING;
	};

	ID3D11VertexShader* m_pxVertexShader;
	ID3D11PixelShader* m_pxPixelShader;
	ID3D11InputLayout* m_pxLayout;
	ID3D11SamplerState* m_pxSampleStateWrap;
	ID3D11SamplerState* m_pxSampleStateClamp;
	ID3D11Buffer* m_pxMatrixBuffer;
	ID3D11Buffer* m_pxLightBuffer;
	ID3D11Buffer* m_pxLightBuffer2;

	static std::list<Ox93_Entity_Renderable*> s_lpxEntitesToRender;

	static Ox93_LightShader* s_pxThis;
};

#endif // ifndef __OX93_LIGHTSHADER_H__
