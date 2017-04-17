// Includes...
#include "Ox93_DepthShader.h"
#include "Game/Entity/Ox93_Entity_Renderable.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Graphics/Ox93_RenderSystem.h"

// Statics...
std::list<Ox93_Entity_Renderable*> Ox93_DepthShader::s_lpxEntitiesToRender;
Ox93_DepthShader* Ox93_DepthShader::s_pxThis = nullptr;


Ox93_DepthShader::Ox93_DepthShader()
: m_pxVertexShader(nullptr)
, m_pxPixelShader(nullptr)
, m_pxLayout(nullptr)
, m_pxMatrixBuffer(nullptr)
{
}

bool Ox93_DepthShader::Create()
{
	Ox93_Assert(!s_pxThis, "Attempting to recreate the depth shader.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_DepthShader;
	return s_pxThis->InitShader(L"../Ox93/Ox93/ROOT/Graphics/Shader/VS/Depth.vs", L"../Ox93/Ox93/ROOT/Graphics/Shader/PS/Depth.ps");
}

void Ox93_DepthShader::Destroy()
{
	if (!s_pxThis) { return; }

	s_pxThis->ShutdownShader();
	delete s_pxThis;
	s_pxThis = nullptr;
}

bool Ox93_DepthShader::Render(const DirectX::XMMATRIX& xViewMatrix, const DirectX::XMMATRIX& xProjectionMatrix)
{
	if (!s_pxThis) { return false; }

	ID3D11DeviceContext* pxDeviceContext = Ox93_D3D::GetDeviceContext();
	if (!pxDeviceContext) { return false; }

	std::list<Ox93_Entity_Renderable*>::const_iterator xIter;
	for (xIter = s_lpxEntitiesToRender.begin(); xIter != s_lpxEntitiesToRender.end(); ++xIter)
	{
		Ox93_Entity_Renderable* pxRenderable = *xIter;
		if (pxRenderable)
		{
			u_int uIndexCount = pxRenderable->GetIndexCount();

			// OJ - Define and translate the world matrix by the object position 
			Ox93_Vector_3 xPosition = pxRenderable->GetPosition();
			DirectX::XMMATRIX xWorldMatrix = DirectX::XMMatrixTranslation(xPosition.x, xPosition.y, xPosition.z);

			// OJ - Rotate the world matrix by the objects orientation
			Ox93_Matrix3x3 xOrientation = pxRenderable->GetOrientation();
			DirectX::XMMATRIX xOrientationMatrix(xOrientation.e00, xOrientation.e01, xOrientation.e02, 0,
				xOrientation.e10, xOrientation.e11, xOrientation.e12, 0,
				xOrientation.e20, xOrientation.e21, xOrientation.e22, 0,
				0, 0, 0, 1);
			xWorldMatrix = DirectX::XMMatrixMultiply(xOrientationMatrix, xWorldMatrix);

			pxRenderable->Render();

			bool bResult = s_pxThis->SetShaderParameters(pxDeviceContext,
				xWorldMatrix,
				xViewMatrix,
				xProjectionMatrix);
			if (!bResult) { return false; }

			s_pxThis->RenderShader(pxDeviceContext, uIndexCount);
		}
	}
	return true;
}

bool Ox93_DepthShader::InitShader(WCHAR* pwszVSFilename, WCHAR* pwszPSFilename)
{
	ID3D11Device* pxDevice = Ox93_D3D::GetDevice();
	HWND hWnd = Ox93_System::GetWindow();
	if (!pxDevice) { return false; }

	ID3DBlob* pxErrorMessage = nullptr;
	ID3DBlob* pxVertexShaderBuffer = nullptr;
	ID3DBlob* pxPixelShaderBuffer = nullptr;

	HRESULT hResult = D3DCompileFromFile(pwszVSFilename, nullptr, nullptr, "DepthVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pxVertexShaderBuffer, &pxErrorMessage);
	if (FAILED(hResult))
	{
		if (pxErrorMessage)
		{
			Ox93_RenderSystem::OutputShaderErrorMessage(pxErrorMessage, hWnd, pwszVSFilename);
		}
		else
		{
			MessageBox(hWnd, pwszVSFilename, L"Missing Depth VS Shader File", MB_OK);
		}

		return false;
	}
	hResult = D3DCompileFromFile(pwszPSFilename, nullptr, nullptr, "DepthPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pxPixelShaderBuffer, &pxErrorMessage);
	if (FAILED(hResult))
	{
		if (pxErrorMessage)
		{
			Ox93_RenderSystem::OutputShaderErrorMessage(pxErrorMessage, hWnd, pwszPSFilename);
		}
		else
		{
			MessageBox(hWnd, pwszPSFilename, L"Missing Depth PS Shader File", MB_OK);
		}

		return false;
	}

	hResult = pxDevice->CreateVertexShader(pxVertexShaderBuffer->GetBufferPointer(), pxVertexShaderBuffer->GetBufferSize(), nullptr, &m_pxVertexShader);
	if (FAILED(hResult)) { return false; }

	hResult = pxDevice->CreatePixelShader(pxPixelShaderBuffer->GetBufferPointer(), pxPixelShaderBuffer->GetBufferSize(), nullptr, &m_pxPixelShader);
	if (FAILED(hResult)) { return false; }

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the RenderSystem and in the shader.
	D3D11_INPUT_ELEMENT_DESC xPolygonLayout[1];
	xPolygonLayout[0].SemanticName = "POSITION";
	xPolygonLayout[0].SemanticIndex = 0;
	xPolygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	xPolygonLayout[0].InputSlot = 0;
	xPolygonLayout[0].AlignedByteOffset = 0;
	xPolygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	xPolygonLayout[0].InstanceDataStepRate = 0;

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

	return true;
}

void Ox93_DepthShader::ShutdownShader()
{
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

bool Ox93_DepthShader::SetShaderParameters(ID3D11DeviceContext* pxDeviceContext,
	const DirectX::XMMATRIX& xWorldMatrix,
	const DirectX::XMMATRIX& xViewMatrix,
	const DirectX::XMMATRIX& xProjectionMatrix)
{
	if (!pxDeviceContext) { return false; }

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

	pxDeviceContext->VSSetConstantBuffers(0, 1, &m_pxMatrixBuffer);

	return true;
}

void Ox93_DepthShader::RenderShader(ID3D11DeviceContext* pxDeviceContext, u_int uIndexCount)
{
	pxDeviceContext->IASetInputLayout(m_pxLayout);

	pxDeviceContext->VSSetShader(m_pxVertexShader, nullptr, 0);
	pxDeviceContext->PSSetShader(m_pxPixelShader, nullptr, 0);
	
	pxDeviceContext->DrawIndexed(uIndexCount, 0, 0);
}