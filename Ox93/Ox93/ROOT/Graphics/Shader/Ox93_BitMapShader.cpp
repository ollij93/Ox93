// Includes...
#include <D3DCompiler.h>
#include "Ox93_BitMapShader.h"
#include "ROOT/Ox93_System.h"
#include "ROOT/Assert/Ox93_Assert.h"
#include "ROOT/Graphics/2D/Ox93_BitMap.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Graphics/Ox93_GraphicsSystem.h"
#include "ROOT/Graphics/Ox93_RenderSystem.h"

// Statics...
std::list<Ox93_BitMap*> Ox93_BitMapShader::s_lpxBitMapsToRender;
Ox93_BitMapShader* Ox93_BitMapShader::s_pxThis = nullptr;

Ox93_BitMapShader::Ox93_BitMapShader()
: m_pxVertexShader(nullptr)
, m_pxPixelShader(nullptr)
, m_pxLayout(nullptr)
, m_pxMatrixBuffer(nullptr)
, m_pxSampleState(nullptr)
{
}

Ox93_BitMapShader::~Ox93_BitMapShader()
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

bool Ox93_BitMapShader::Create()
{
	Ox93_Assert(!s_pxThis, "Attempting to recreate the bitmap shader.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_BitMapShader;
	return s_pxThis->InitShader(L"../Ox93/Ox93/ROOT/Graphics/Shader/VS/BitMap.vs", L"../Ox93/Ox93/ROOT/Graphics/Shader/PS/BitMap.ps");
}

void Ox93_BitMapShader::Destroy()
{
	if (!s_pxThis) { return; }

	delete s_pxThis;
	s_pxThis = nullptr;
}

bool Ox93_BitMapShader::Render()
{
	if (!s_pxThis) { return false; }

	ID3D11DeviceContext* pxDeviceContext = Ox93_D3D::GetDeviceContext();
	if (!pxDeviceContext) { return false; }

	// OJ - Get the ortho and view matrices
	DirectX::XMMATRIX xOrthoMatrix;
	DirectX::XMMATRIX xViewMatrix;
	DirectX::XMMATRIX xWorldMatrix = DirectX::XMMatrixIdentity();
	Ox93_D3D::GetOrthoMatrix(xOrthoMatrix);
	xViewMatrix = DirectX::XMMatrixLookAtLH({ 0.f,0.f,-1.f,0.f }, { 0.f,0.f,0.f,0.f }, { 0.f,1.f,0.f,0.f });

	std::list<Ox93_BitMap*>::const_iterator xIter;
	for (xIter = s_lpxBitMapsToRender.begin(); xIter != s_lpxBitMapsToRender.end(); ++xIter)
	{
		Ox93_BitMap* pxBitMap = *xIter;
		if (pxBitMap)
		{
			ID3D11ShaderResourceView* pxTexture = pxBitMap->GetTexture();

			pxBitMap->Render();

			bool bResult = s_pxThis->SetShaderParameters(pxDeviceContext, xWorldMatrix, xViewMatrix, xOrthoMatrix, pxTexture);
			if (!bResult) { return false; }

			s_pxThis->RenderShader(pxDeviceContext);
		}
	}
	return true;
}

bool Ox93_BitMapShader::InitShader(WCHAR* pwszVSFilename, WCHAR* pwszPSFilename)
{
	ID3D11Device* pxDevice = Ox93_D3D::GetDevice();
	HWND hWnd = Ox93_System::GetWindow();
	if (!pxDevice) { return false; }

	ID3DBlob* pxErrorMessage = nullptr;
	ID3DBlob* pxVertexShaderBuffer = nullptr;
	ID3DBlob* pxPixelShaderBuffer = nullptr;

	HRESULT hResult = D3DCompileFromFile(pwszVSFilename, 
		nullptr, 
		nullptr, 
		"BitMapVertexShader", 
		"vs_4_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 
		0, 
		&pxVertexShaderBuffer, 
		&pxErrorMessage);
	if (FAILED(hResult))
	{
		if (pxErrorMessage)
		{
			Ox93_RenderSystem::OutputShaderErrorMessage(pxErrorMessage, hWnd, pwszVSFilename);
		}
		else
		{
			MessageBox(hWnd, pwszVSFilename, L"Missing BitMap VS Shader File", MB_OK);
		}
		return false;
	}

	hResult = D3DCompileFromFile(pwszPSFilename,
		nullptr,
		nullptr,
		"BitMapPixelShader",
		"ps_4_0",
		D3D10_SHADER_ENABLE_STRICTNESS,
		0,
		&pxPixelShaderBuffer,
		&pxErrorMessage);
	if (FAILED(hResult))
	{
		if (pxErrorMessage)
		{
			Ox93_RenderSystem::OutputShaderErrorMessage(pxErrorMessage, hWnd, pwszPSFilename);
		}
		else
		{
			MessageBox(hWnd, pwszPSFilename, L"Missing BitMap PS Shader File", MB_OK);
		}

		return false;
	}

	hResult = pxDevice->CreateVertexShader(pxVertexShaderBuffer->GetBufferPointer(),
		pxVertexShaderBuffer->GetBufferSize(),
		nullptr,
		&m_pxVertexShader);
	if (FAILED(hResult)) { return false; }

	hResult = pxDevice->CreatePixelShader(pxPixelShaderBuffer->GetBufferPointer(),
		pxPixelShaderBuffer->GetBufferSize(),
		nullptr,
		&m_pxPixelShader);
	if (FAILED(hResult)) { return false; }

	D3D11_INPUT_ELEMENT_DESC xLayout[3];
	xLayout[0].SemanticName = "POSITION";
	xLayout[0].SemanticIndex = 0;
	xLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	xLayout[0].InputSlot = 0;
	xLayout[0].AlignedByteOffset = 0;
	xLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	xLayout[0].InstanceDataStepRate = 0;

	xLayout[1].SemanticName = "TEXCOORD";
	xLayout[1].SemanticIndex = 0;
	xLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	xLayout[1].InputSlot = 0;
	xLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	xLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	xLayout[1].InstanceDataStepRate = 0;

	xLayout[2].SemanticName = "NORMAL";
	xLayout[2].SemanticIndex = 0;
	xLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	xLayout[2].InputSlot = 0;
	xLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	xLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	xLayout[2].InstanceDataStepRate = 0;

	u_int uNumElements = sizeof(xLayout) / sizeof(xLayout[0]);
	hResult = pxDevice->CreateInputLayout(xLayout,
		uNumElements,
		pxVertexShaderBuffer->GetBufferPointer(),
		pxVertexShaderBuffer->GetBufferSize(),
		&m_pxLayout);
	if (FAILED(hResult)) { return false; }

	// OJ - We don't need these any more so release them
	pxVertexShaderBuffer->Release();
	pxVertexShaderBuffer = nullptr;
	pxPixelShaderBuffer->Release();
	pxPixelShaderBuffer = nullptr;

	D3D11_BUFFER_DESC xMatrixBufferDesc;
	xMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	xMatrixBufferDesc.ByteWidth = sizeof(BitMapMatrixBuffer);
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

bool Ox93_BitMapShader::SetShaderParameters(ID3D11DeviceContext* pxDeviceContext,
	const DirectX::XMMATRIX& xWorldMatrix,
	const DirectX::XMMATRIX& xViewMatrix,
	const DirectX::XMMATRIX& xOrthoMatrix,
	ID3D11ShaderResourceView* pxTexture)
{
	DirectX::XMMATRIX xTransposedWorldMatrix = DirectX::XMMatrixTranspose(xWorldMatrix);
	DirectX::XMMATRIX xTransposedViewMatrix = DirectX::XMMatrixTranspose(xViewMatrix);
	DirectX::XMMATRIX xTransposedOrthoMatrix = DirectX::XMMatrixTranspose(xOrthoMatrix);

	D3D11_MAPPED_SUBRESOURCE xMappedResource;
	HRESULT hResult = pxDeviceContext->Map(m_pxMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &xMappedResource);
	if (FAILED(hResult)) { return false; }

	BitMapMatrixBuffer* pxData = static_cast<BitMapMatrixBuffer*>(xMappedResource.pData);
	pxData->xWorld = xTransposedWorldMatrix;
	pxData->xView = xTransposedViewMatrix;
	pxData->xOrtho = xTransposedOrthoMatrix;

	pxDeviceContext->Unmap(m_pxMatrixBuffer, 0);

	u_int uBufferNumber = 0;

	pxDeviceContext->VSSetConstantBuffers(uBufferNumber, 1, &m_pxMatrixBuffer);
	pxDeviceContext->PSSetShaderResources(0, 1, &pxTexture);

	return true;
}

void Ox93_BitMapShader::RenderShader(ID3D11DeviceContext* pxDeviceContext)
{
	pxDeviceContext->IASetInputLayout(m_pxLayout);

	pxDeviceContext->VSSetShader(m_pxVertexShader, nullptr, 0);
	pxDeviceContext->PSSetShader(m_pxPixelShader, nullptr, 0);

	pxDeviceContext->PSSetSamplers(0, 1, &m_pxSampleState);

	pxDeviceContext->DrawIndexed(6, 0, 0); // 6 is the index number for a bitmap
}