#pragma once

#ifndef OX93_TERRAINSHADER_H__
#define OX93_TERRAINSHADER_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Math/Ox93_Math.h"

// Forward Declarations...
class Ox93_Entity_Renderable;
class Ox93_Light;
class Ox93_Terrain;

class Ox93_TerrainShader
{
public:
	static bool Create();
	static void Destroy();

	static bool Render(const Ox93_Light* pxLight);

	static void AddToRenderList(Ox93_Terrain* pxRenderable) { if (pxRenderable) { s_lpxEntitiesToRender.push_back(pxRenderable); } }
	static void RemoveFromRenderList(Ox93_Terrain* pxRenderable) { s_lpxEntitiesToRender.remove(pxRenderable); }

protected:
	Ox93_TerrainShader();
	~Ox93_TerrainShader() {}

	bool InitShader(WCHAR* pwszVSFilename, WCHAR* pwszPSFilename);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* pxDeviceContext, 
		const DirectX::XMMATRIX& xWorldMatrix, 
		const DirectX::XMMATRIX& xViewMatrix, 
		const DirectX::XMMATRIX& xProjectionMatrix,
		const DirectX::XMMATRIX& xLightViewMatrix,
		const DirectX::XMMATRIX& xLightProjectionMatrix,
		ID3D11ShaderResourceView* pxDepthTexture,
		Ox93_Vector_3 xLightDirection, 
		Ox93_Color xAmbientColor, 
		Ox93_Color xDiffuseColor);
	void RenderShader(ID3D11DeviceContext* pxDeviceContext, u_int uIndexCount);

private:
	struct MatrixBuffer
	{
		DirectX::XMMATRIX xWorld;
		DirectX::XMMATRIX xView;
		DirectX::XMMATRIX xProjection;
		DirectX::XMMATRIX xLightView;
		DirectX::XMMATRIX xLightProjection;
	};
	struct LightBuffer
	{
		Ox93_Color xAmbientColor;
		Ox93_Color xDiffuseColor;
	};
	struct LightBuffer2
	{
		Ox93_Vector_3 xLightDirection;
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

	static std::list<Ox93_Terrain*> s_lpxEntitiesToRender;
	static Ox93_TerrainShader* s_pxThis;
};

#endif // OX93_TERRAINSHADER_H__

