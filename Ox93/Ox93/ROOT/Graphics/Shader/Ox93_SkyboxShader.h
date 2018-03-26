#pragma once

#ifndef OX93_SKYBOXSHADER_H__
#define OX93_SKYBOXSHADER_H__

// Includes...
#include <d3d11.h>
#include <DirectXMath.h>
#include <Windows.h>

// Forward Declarations...
class Ox93_Skybox;

class Ox93_SkyboxShader
{
public:
	static bool Create();
	static void Destroy();

	static bool Render();

protected:
	Ox93_SkyboxShader();
	~Ox93_SkyboxShader() {}

private:
	bool InitShader(WCHAR* pwszVSFilename, WCHAR* pwszPSFilename);
	void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext* pxDeviceContext, 
		const DirectX::XMMATRIX& xWorldMatrix, 
		const DirectX::XMMATRIX& xViewMatrix, 
		const DirectX::XMMATRIX& xProjectionMatrix, 
		ID3D11ShaderResourceView* pxTexture);
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
	ID3D11SamplerState* m_pxSampleState;
	ID3D11Buffer* m_pxMatrixBuffer;
	
	static Ox93_SkyboxShader* s_pxThis;
};

#endif // OX93_SKYBOXSHADER_H__

