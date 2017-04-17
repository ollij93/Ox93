// Includes...
#include "Ox93_SkyboxShader.h"
#include "Game/World/Skybox/Ox93_Skybox.h"
#include "ROOT/Graphics/Ox93_GraphicsSystem.h"

// Statics...
Ox93_SkyboxShader* Ox93_SkyboxShader::s_pxThis = nullptr;

Ox93_SkyboxShader::Ox93_SkyboxShader()
: m_pxVertexShader(nullptr)
, m_pxPixelShader(nullptr)
, m_pxLayout(nullptr)
, m_pxSampleState(nullptr)
, m_pxMatrixBuffer(nullptr)
{
}

bool Ox93_SkyboxShader::Create()
{
	Ox93_Assert(!s_pxThis, "Attempting to recreate the skybox shader.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_SkyboxShader;
	return s_pxThis->InitShader(L"../Ox93/Ox93/ROOT/Graphics/Shader/VS/Skybox.vs", L"../Ox93/Ox93/ROOT/Graphics/Shader/PS/Skybox.ps");
}

void Ox93_SkyboxShader::Destroy()
{
	if (!s_pxThis) { return; }

	s_pxThis->ShutdownShader();
	delete s_pxThis;
	s_pxThis = nullptr;
}

bool Ox93_SkyboxShader::Render()
{
	if (!s_pxThis) { return false; }

	ID3D11DeviceContext* pxDeviceContext = Ox93_D3D::GetDeviceContext();
	if (!pxDeviceContext) { return false; }

	// OJ - Get the projection and view matrices
	DirectX::XMMATRIX xProjectionMatrix;
	DirectX::XMMATRIX xViewMatrix;
	Ox93_D3D::GetProjectionMatrix(xProjectionMatrix);
	Ox93_GraphicsSystem::GetViewMatrix(xViewMatrix);

	Ox93_Skybox* pxSkybox = Ox93_Skybox::GetActiveSkybox();

	if (pxSkybox)
	{
		u_int uIndexCount = pxSkybox->GetIndexCount();
		ID3D11ShaderResourceView* pxTexture = pxSkybox->GetTexture();

		// OJ - Define and translate the world matrix by the object position 
		Ox93_Vector_3 xPosition = pxSkybox->GetPosition();
		DirectX::XMMATRIX xWorldMatrix = DirectX::XMMatrixTranslation(xPosition.x, xPosition.y, xPosition.z);

		pxSkybox->Render();

		bool bResult = s_pxThis->SetShaderParameters(pxDeviceContext, xWorldMatrix, xViewMatrix, xProjectionMatrix, pxTexture);
		if (!bResult) { return false; }

		s_pxThis->RenderShader(pxDeviceContext, uIndexCount);
	}

	return true;
}

bool Ox93_SkyboxShader::InitShader(WCHAR* pwszVSFilename, WCHAR* pwszPSFilename)
{
	ID3D11Device* pxDevice = Ox93_D3D::GetDevice();
	HWND hWnd = Ox93_System::GetWindow();
	if (!pxDevice) { return false; }

	ID3DBlob* pxErrorMessage = nullptr;
	ID3DBlob* pxVertexShaderBuffer = nullptr;
	ID3DBlob* pxPixelShaderBuffer = nullptr;

	HRESULT hResult = D3DCompileFromFile(pwszVSFilename, nullptr, nullptr, "SkyboxVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pxVertexShaderBuffer, &pxErrorMessage);
	if (FAILED(hResult))
	{
		if (pxErrorMessage)
		{
			Ox93_RenderSystem::OutputShaderErrorMessage(pxErrorMessage, hWnd, pwszVSFilename);
		}
		else
		{
			MessageBox(hWnd, pwszVSFilename, L"Missing Skybox VS Shader File", MB_OK);
		}

		return false;
	}

	hResult = D3DCompileFromFile(pwszPSFilename, nullptr, nullptr, "SkyboxPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pxPixelShaderBuffer, &pxErrorMessage);
	if (FAILED(hResult))
	{
		if (pxErrorMessage)
		{
			Ox93_RenderSystem::OutputShaderErrorMessage(pxErrorMessage, hWnd, pwszPSFilename);
		}
		else
		{
			MessageBox(hWnd, pwszPSFilename, L"Missing Skybox PS Shader File", MB_OK);
		}

		return false;
	}

	hResult = pxDevice->CreateVertexShader(pxVertexShaderBuffer->GetBufferPointer(), pxVertexShaderBuffer->GetBufferSize(), nullptr, &m_pxVertexShader);
	if (FAILED(hResult)) { return false; }

	hResult = pxDevice->CreatePixelShader(pxPixelShaderBuffer->GetBufferPointer(), pxPixelShaderBuffer->GetBufferSize(), nullptr, &m_pxPixelShader);
	if (FAILED(hResult)) { return false; }

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the RenderSystem and in the shader.
	D3D11_INPUT_ELEMENT_DESC xPolygonLayout[2];
	xPolygonLayout[0].SemanticName = "POSITION";
	xPolygonLayout[0].SemanticIndex = 0;
	xPolygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	xPolygonLayout[0].InputSlot = 0;
	xPolygonLayout[0].AlignedByteOffset = 0;
	xPolygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	xPolygonLayout[0].InstanceDataStepRate = 0;

	xPolygonLayout[1].SemanticName = "TEXCOORD";
	xPolygonLayout[1].SemanticIndex = 0;
	xPolygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	xPolygonLayout[1].InputSlot = 0;
	xPolygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	xPolygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	xPolygonLayout[1].InstanceDataStepRate = 0;

	u_int uNumElements = sizeof(xPolygonLayout) / sizeof(xPolygonLayout[0]);
	hResult = pxDevice->CreateInputLayout(xPolygonLayout, uNumElements, pxVertexShaderBuffer->GetBufferPointer(), pxVertexShaderBuffer->GetBufferSize(), &m_pxLayout);
	if (FAILED(hResult)) { return false; }

	pxVertexShaderBuffer->Release();
	pxVertexShaderBuffer = nullptr;

	pxPixelShaderBuffer->Release();
	pxPixelShaderBuffer = nullptr;

	D3D11_BUFFER_DESC xMatrixBufferDesc;
	xMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	xMatrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	xMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	xMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	xMatrixBufferDesc.MiscFlags = 0;
	xMatrixBufferDesc.StructureByteStride = 0;

	hResult = pxDevice->CreateBuffer(&xMatrixBufferDesc, nullptr, &m_pxMatrixBuffer);
	if (FAILED(hResult)) { return false; }

	D3D11_SAMPLER_DESC xSamplerDesc;
	xSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	xSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	xSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	xSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	xSamplerDesc.MipLODBias = 0.0f;
	xSamplerDesc.MaxAnisotropy = 1;
	xSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	xSamplerDesc.BorderColor[0] = 0;
	xSamplerDesc.BorderColor[1] = 0;
	xSamplerDesc.BorderColor[2] = 0;
	xSamplerDesc.BorderColor[3] = 0;
	xSamplerDesc.MinLOD = 0;
	xSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hResult = pxDevice->CreateSamplerState(&xSamplerDesc, &m_pxSampleState);
	if (FAILED(hResult)) { return false; }

	return true;
}

void Ox93_SkyboxShader::ShutdownShader()
{
	if (m_pxSampleState)
	{
		m_pxSampleState->Release();
		m_pxSampleState = nullptr;
	}

	if (m_pxMatrixBuffer)
	{
		m_pxMatrixBuffer->Release();
		m_pxMatrixBuffer = nullptr;
	}

	if (m_pxLayout)
	{
		m_pxLayout->Release();
		m_pxLayout = nullptr;
	}

	if (m_pxPixelShader)
	{
		m_pxPixelShader->Release();
		m_pxPixelShader = nullptr;
	}

	if (m_pxVertexShader)
	{
		m_pxVertexShader->Release();
		m_pxVertexShader = nullptr;
	}
}

bool Ox93_SkyboxShader::SetShaderParameters(ID3D11DeviceContext* pxDeviceContext, 
	const DirectX::XMMATRIX& xWorldMatrix, 
	const DirectX::XMMATRIX& xViewMatrix, 
	const DirectX::XMMATRIX& xProjectionMatrix, 
	ID3D11ShaderResourceView* pxTexture)
{
	DirectX::XMMATRIX xTransposedWorldMatrix = DirectX::XMMatrixTranspose(xWorldMatrix);
	DirectX::XMMATRIX xTransposedViewMatrix = DirectX::XMMatrixTranspose(xViewMatrix);
	DirectX::XMMATRIX xTransposedProjectionMatrix = DirectX::XMMatrixTranspose(xProjectionMatrix);

	D3D11_MAPPED_SUBRESOURCE xMappedResource;
	HRESULT hResult = pxDeviceContext->Map(m_pxMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &xMappedResource);
	if (FAILED(hResult)) { return false; }

	MatrixBufferType* pxData = (MatrixBufferType*)xMappedResource.pData;
	pxData->xWorld = xTransposedWorldMatrix;
	pxData->xView = xTransposedViewMatrix;
	pxData->xProjection = xTransposedProjectionMatrix;

	pxDeviceContext->Unmap(m_pxMatrixBuffer, 0);

	u_int uBufferNumber = 0;

	pxDeviceContext->VSSetConstantBuffers(uBufferNumber, 1, &m_pxMatrixBuffer);

	pxDeviceContext->PSSetShaderResources(0, 1, &pxTexture);

	return true;
}

void Ox93_SkyboxShader::RenderShader(ID3D11DeviceContext* pxDeviceContext, u_int uIndexCount)
{
	pxDeviceContext->IASetInputLayout(m_pxLayout);

	pxDeviceContext->VSSetShader(m_pxVertexShader, nullptr, 0);
	pxDeviceContext->PSSetShader(m_pxPixelShader, nullptr, 0);

	pxDeviceContext->PSSetSamplers(0, 1, &m_pxSampleState);

	pxDeviceContext->DrawIndexed(uIndexCount, 0, 0);
}