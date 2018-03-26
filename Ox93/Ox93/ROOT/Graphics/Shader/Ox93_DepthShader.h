#pragma once

#ifndef OX93_DEPTHSHADER_H__
#define OX93_DEPTHSHADER_H__

// Includes...
#include <list>
#include <d3d11.h>
#include <DirectXMath.h>
#include <Windows.h>

// Forward Declarations...
class Ox93_Entity_Renderable;

class Ox93_DepthShader
{
public:
	static bool Create();
	static void Destroy();

	static bool Render(const DirectX::XMMATRIX& xProjectionMatrix, const DirectX::XMMATRIX& xViewMatrix);

	static void AddToRenderList(Ox93_Entity_Renderable* pxRenderable) { if (pxRenderable) { s_lpxEntitiesToRender.push_back(pxRenderable); } }
	static void RemoveFromRenderList(Ox93_Entity_Renderable* pxRenderable) { s_lpxEntitiesToRender.remove(pxRenderable); }


protected:
	Ox93_DepthShader();
	~Ox93_DepthShader() {}

private:
	bool InitShader(WCHAR* pwszVSFilename, WCHAR* pwszPSFilename);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* pxDeviceContext,
		const DirectX::XMMATRIX& xWorldMatrix,
		const DirectX::XMMATRIX& xViewMatrix,
		const DirectX::XMMATRIX& xProjectionMatrix);
	void RenderShader(ID3D11DeviceContext* pxDeviceContext, u_int uIndexCount);

	struct MatrixBufferType
	{
		DirectX::XMMATRIX xWorld;
		DirectX::XMMATRIX xView;
		DirectX::XMMATRIX xProjection;
	};

	ID3D11VertexShader* m_pxVertexShader;
	ID3D11PixelShader* m_pxPixelShader;
	ID3D11InputLayout* m_pxLayout;
	ID3D11Buffer* m_pxMatrixBuffer;
	
	static std::list<Ox93_Entity_Renderable*> s_lpxEntitiesToRender;

	static Ox93_DepthShader* s_pxThis;
};

#endif // OX93_DEPTHSHADER_H__

