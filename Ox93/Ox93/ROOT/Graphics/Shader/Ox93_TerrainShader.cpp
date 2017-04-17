// Includes...
#include "Ox93_TerrainShader.h"
#include "Game/World/Terrain/Ox93_Terrain.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Graphics/Ox93_GraphicsSystem.h"

// Statics...
std::list<Ox93_Terrain*> Ox93_TerrainShader::s_lpxEntitiesToRender;
Ox93_TerrainShader* Ox93_TerrainShader::s_pxThis = nullptr;

Ox93_TerrainShader::Ox93_TerrainShader()
: m_pxVertexShader(nullptr)
, m_pxPixelShader(nullptr)
, m_pxLayout(nullptr)
, m_pxSampleStateWrap(nullptr)
, m_pxSampleStateClamp(nullptr)
, m_pxMatrixBuffer(nullptr)
, m_pxLightBuffer(nullptr)
, m_pxLightBuffer2(nullptr)
{
}

bool Ox93_TerrainShader::Create()
{
	Ox93_Assert(!s_pxThis, "Attempting to recreate the terrain shader.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_TerrainShader;
	return s_pxThis->InitShader(L"../Ox93/Ox93/ROOT/Graphics/Shader/VS/Terrain.vs", L"../Ox93/Ox93/ROOT/Graphics/Shader/PS/Terrain.ps");
}

void Ox93_TerrainShader::Destroy()
{
	if (!s_pxThis) { return; }

	s_pxThis->ShutdownShader();
	delete s_pxThis;
	s_pxThis = nullptr;
}

bool Ox93_TerrainShader::Render(const Ox93_Light* pxLight)
{
	if (!s_pxThis || !pxLight) { return false; }

	ID3D11DeviceContext* pxDeviceContext = Ox93_D3D::GetDeviceContext();
	if (!pxDeviceContext) { return false; }

	// OJ - Get the projection and view matrices
	DirectX::XMMATRIX xProjectionMatrix;
	DirectX::XMMATRIX xViewMatrix;
	DirectX::XMMATRIX xLightViewMatrix;
	DirectX::XMMATRIX xLightProjectionMatrix;
	Ox93_D3D::GetProjectionMatrix(xProjectionMatrix);
	Ox93_GraphicsSystem::GetViewMatrix(xViewMatrix);
	pxLight->GetViewMatrix(xLightViewMatrix);
	pxLight->GetProjectionMatrix(xLightProjectionMatrix);

	std::list<Ox93_Terrain*>::const_iterator xIter;
	for (xIter = s_lpxEntitiesToRender.begin(); xIter != s_lpxEntitiesToRender.end(); ++xIter)
	{
		Ox93_Terrain* pxRenderable = *xIter;
		if (pxRenderable)
		{
			u_int uIndexCount = pxRenderable->GetIndexCount();

			// OJ - Define and translate the world matrix by the object position 
			Ox93_Vector_3 xPosition = pxRenderable->GetPosition();
			DirectX::XMMATRIX xWorldMatrix = DirectX::XMMatrixTranslation(xPosition.x, xPosition.y, xPosition.z);

			pxRenderable->Render();

			bool bResult = s_pxThis->SetShaderParameters(pxDeviceContext, 
				xWorldMatrix, 
				xViewMatrix, 
				xProjectionMatrix,
				xLightViewMatrix,
				xLightProjectionMatrix,
				pxLight->GetShaderResource(),
				pxLight->GetDirection(), 
				pxLight->GetAmbientColor(), 
				pxLight->GetDiffuseColor());
			if (!bResult) { return false; }

			s_pxThis->RenderShader(pxDeviceContext, uIndexCount);
		}
	}
	return true;
}

bool Ox93_TerrainShader::InitShader(WCHAR* pwszVSFilename, WCHAR* pwszPSFilename)
{
	ID3D11Device* pxDevice = Ox93_D3D::GetDevice();
	HWND hWnd = Ox93_System::GetWindow();
	if (!pxDevice) { return false; }

	ID3DBlob* pxErrorMessage = nullptr;
	ID3DBlob* pxVertexShaderBuffer = nullptr;
	ID3DBlob* pxPixelShaderBuffer = nullptr;

	HRESULT hResult = D3DCompileFromFile(pwszVSFilename, nullptr, nullptr, "TerrainVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pxVertexShaderBuffer, &pxErrorMessage);
	if (FAILED(hResult))
	{
		if (pxErrorMessage)
		{
			Ox93_RenderSystem::OutputShaderErrorMessage(pxErrorMessage, hWnd, pwszVSFilename);
		}
		else
		{
			MessageBox(hWnd, pwszVSFilename, L"Missing Terrain VS Shader File", MB_OK);
		}

		return false;
	}

	hResult = D3DCompileFromFile(pwszPSFilename, nullptr, nullptr, "TerrainPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pxPixelShaderBuffer, &pxErrorMessage);
	if (FAILED(hResult))
	{
		if (pxErrorMessage)
		{
			Ox93_RenderSystem::OutputShaderErrorMessage(pxErrorMessage, hWnd, pwszPSFilename);
		}
		else
		{
			MessageBox(hWnd, pwszPSFilename, L"Missing Terrain PS Shader File", MB_OK);
		}

		return false;
	}

	hResult = pxDevice->CreateVertexShader(pxVertexShaderBuffer->GetBufferPointer(), pxVertexShaderBuffer->GetBufferSize(), nullptr, &m_pxVertexShader);
	if (FAILED(hResult)) { return false; }

	hResult = pxDevice->CreatePixelShader(pxPixelShaderBuffer->GetBufferPointer(), pxPixelShaderBuffer->GetBufferSize(), nullptr, &m_pxPixelShader);
	if (FAILED(hResult)) { return false; }

	// Create the vertex input layout description.
	// This setup needs to match the TerrainVertex structure in the code and in the shader
	D3D11_INPUT_ELEMENT_DESC xPolygonLayout[3];
	xPolygonLayout[0].SemanticName = "POSITION";
	xPolygonLayout[0].SemanticIndex = 0;
	xPolygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	xPolygonLayout[0].InputSlot = 0;
	xPolygonLayout[0].AlignedByteOffset = 0;
	xPolygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	xPolygonLayout[0].InstanceDataStepRate = 0;

	xPolygonLayout[1].SemanticName = "COLOR";
	xPolygonLayout[1].SemanticIndex = 0;
	xPolygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	xPolygonLayout[1].InputSlot = 0;
	xPolygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	xPolygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	xPolygonLayout[1].InstanceDataStepRate = 0;

	xPolygonLayout[2].SemanticName = "NORMAL";
	xPolygonLayout[2].SemanticIndex = 0;
	xPolygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	xPolygonLayout[2].InputSlot = 0;
	xPolygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	xPolygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	xPolygonLayout[2].InstanceDataStepRate = 0;

	u_int uNumElements = sizeof(xPolygonLayout) / sizeof(xPolygonLayout[0]);
	hResult = pxDevice->CreateInputLayout(xPolygonLayout, 
		uNumElements, 
		pxVertexShaderBuffer->GetBufferPointer(),
		pxVertexShaderBuffer->GetBufferSize(),
		&m_pxLayout);
	if (FAILED(hResult)) { return false; }

	pxVertexShaderBuffer->Release();
	pxVertexShaderBuffer = nullptr;
	pxPixelShaderBuffer->Release();
	pxPixelShaderBuffer = nullptr;

	D3D11_BUFFER_DESC xMatrixBufferDesc;
	xMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	xMatrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
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

	hResult = pxDevice->CreateSamplerState(&xSamplerDesc, &m_pxSampleStateWrap);
	if (FAILED(hResult)) { return false; }

	xSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	xSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	xSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	hResult = pxDevice->CreateSamplerState(&xSamplerDesc, &m_pxSampleStateClamp);
	if (FAILED(hResult)) { return false; }

	D3D11_BUFFER_DESC xLightBufferDesc;
	xLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	xLightBufferDesc.ByteWidth = sizeof(LightBuffer);
	xLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	xLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	xLightBufferDesc.MiscFlags = 0;
	xLightBufferDesc.StructureByteStride = 0;

	hResult = pxDevice->CreateBuffer(&xLightBufferDesc, nullptr, &m_pxLightBuffer);
	if (FAILED(hResult)) { return false; }

	D3D11_BUFFER_DESC xLightBufferDesc2;
	xLightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	xLightBufferDesc2.ByteWidth = sizeof(LightBuffer2);
	xLightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	xLightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	xLightBufferDesc2.MiscFlags = 0;
	xLightBufferDesc2.StructureByteStride = 0;

	hResult = pxDevice->CreateBuffer(&xLightBufferDesc2, nullptr, &m_pxLightBuffer2);
	if (FAILED(hResult)) { return false; }

	return true;
}

void Ox93_TerrainShader::ShutdownShader()
{
	if (m_pxLightBuffer2)
	{
		m_pxLightBuffer2->Release();
		m_pxLightBuffer2 = nullptr;
	}

	if (m_pxLightBuffer)
	{
		m_pxLightBuffer->Release();
		m_pxLightBuffer = nullptr;
	}

	if (m_pxSampleStateClamp)
	{
		m_pxSampleStateClamp->Release();
		m_pxSampleStateClamp = nullptr;
	}

	if (m_pxSampleStateWrap)
	{
		m_pxSampleStateWrap->Release();
		m_pxSampleStateWrap = nullptr;
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

bool Ox93_TerrainShader::SetShaderParameters(ID3D11DeviceContext* pxDeviceContext, 
	const DirectX::XMMATRIX& xWorldMatrix,
	const DirectX::XMMATRIX& xViewMatrix,
	const DirectX::XMMATRIX& xProjectionMatrix,
	const DirectX::XMMATRIX& xLightViewMatrix,
	const DirectX::XMMATRIX& xLightProjectionMatrix,
	ID3D11ShaderResourceView* pxDepthTexture,
	Ox93_Vector_3 xLightDirection,
	Ox93_Color xAmbientColor,
	Ox93_Color xDiffuseColor)
{
	DirectX::XMMATRIX xTransposedWorldMatrix = DirectX::XMMatrixTranspose(xWorldMatrix);
	DirectX::XMMATRIX xTransposedViewMatrix = DirectX::XMMatrixTranspose(xViewMatrix);
	DirectX::XMMATRIX xTransposedProjectionMatrix = DirectX::XMMatrixTranspose(xProjectionMatrix);
	DirectX::XMMATRIX xTransposedLightViewMatrix = DirectX::XMMatrixTranspose(xLightViewMatrix);
	DirectX::XMMATRIX xTransposedLightProjectionMatrix = DirectX::XMMatrixTranspose(xLightProjectionMatrix);

	D3D11_MAPPED_SUBRESOURCE xMappedResource;
	HRESULT hResult = pxDeviceContext->Map(m_pxMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &xMappedResource);
	if (FAILED(hResult)) { return false; }

	MatrixBuffer* pxData = static_cast<MatrixBuffer*>(xMappedResource.pData);
	pxData->xWorld = xTransposedWorldMatrix;
	pxData->xView = xTransposedViewMatrix;
	pxData->xProjection = xTransposedProjectionMatrix;
	pxData->xLightView = xTransposedLightViewMatrix;
	pxData->xLightProjection = xTransposedLightProjectionMatrix;

	pxDeviceContext->Unmap(m_pxMatrixBuffer, 0);

	u_int uBufferNumber = 0;

	pxDeviceContext->VSSetConstantBuffers(uBufferNumber, 1, &m_pxMatrixBuffer);

	pxDeviceContext->PSSetShaderResources(0, 1, &pxDepthTexture);


	hResult = pxDeviceContext->Map(m_pxLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &xMappedResource);
	if (FAILED(hResult)) { return false; }

	LightBuffer* pxData2 = static_cast<LightBuffer*>(xMappedResource.pData);
	pxData2->xAmbientColor = xAmbientColor;
	pxData2->xDiffuseColor = xDiffuseColor;

	pxDeviceContext->Unmap(m_pxLightBuffer, 0);

	uBufferNumber = 0;

	pxDeviceContext->PSSetConstantBuffers(uBufferNumber, 1, &m_pxLightBuffer);

	hResult = pxDeviceContext->Map(m_pxLightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &xMappedResource);
	if (FAILED(hResult)) { return false; }

	LightBuffer2* pxData3 = (LightBuffer2*)xMappedResource.pData;
	pxData3->xLightDirection = xLightDirection;
	pxData3->fPADDING = 0.f;

	pxDeviceContext->Unmap(m_pxLightBuffer2, 0);

	uBufferNumber = 1;

	pxDeviceContext->VSSetConstantBuffers(uBufferNumber, 1, &m_pxLightBuffer2);

	return true;
}

void Ox93_TerrainShader::RenderShader(ID3D11DeviceContext* pxDeviceContext, u_int uIndexCount)
{
	pxDeviceContext->IASetInputLayout(m_pxLayout);

	pxDeviceContext->VSSetShader(m_pxVertexShader, nullptr, 0);
	pxDeviceContext->PSSetShader(m_pxPixelShader, nullptr, 0);

	pxDeviceContext->PSSetSamplers(0, 1, &m_pxSampleStateClamp);
	pxDeviceContext->PSSetSamplers(1, 1, &m_pxSampleStateWrap);

	pxDeviceContext->DrawIndexed(uIndexCount, 0, 0);
}