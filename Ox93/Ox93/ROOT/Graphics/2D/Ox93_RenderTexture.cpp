// Includes...
#include "Ox93_RenderTexture.h"
#include "ROOT/Graphics/Ox93_D3D.h"

Ox93_RenderTexture::Ox93_RenderTexture()
	: m_pxRenderTargetTexture(nullptr)
	, m_pxRenderTargetView(nullptr)
	, m_pxShaderResourceViewMS(nullptr)
	, m_pxShaderResourceView(nullptr)
	, m_pxDepthStencilView(nullptr)
	, m_pxDepthStencilBuffer(nullptr)
	, m_xViewPort()
{
}

Ox93_RenderTexture::~Ox93_RenderTexture()
{
	if (m_pxDepthStencilBuffer)
	{
		m_pxDepthStencilBuffer->Release();
		m_pxDepthStencilBuffer = nullptr;
	}
	if (m_pxDepthStencilView)
	{
		m_pxDepthStencilView->Release();
		m_pxDepthStencilView = nullptr;
	}
	if (m_pxShaderResourceView)
	{
		m_pxShaderResourceView->Release();
		m_pxShaderResourceView = nullptr;
	}
	if (m_pxShaderResourceViewMS)
	{
		m_pxShaderResourceViewMS->Release();
		m_pxShaderResourceViewMS = nullptr;
	}
	if (m_pxRenderTargetView)
	{
		m_pxRenderTargetView->Release();
		m_pxRenderTargetView = nullptr;
	}
	if (m_pxRenderTargetTexture)
	{
		m_pxRenderTargetTexture->Release();
		m_pxRenderTargetTexture = nullptr;
	}
}

bool Ox93_RenderTexture::Init(u_int uTextureWidth, u_int uTextureHeight)
{
	ID3D11Device* pxDevice = Ox93_D3D::GetDevice();
	Ox93_Assert(pxDevice, "Couldn't get the D3D device when creating render texture");
	if (!pxDevice) { return false; }

	D3D11_TEXTURE2D_DESC xTextureDesc;
	ZeroMemory(&xTextureDesc, sizeof(xTextureDesc));
	xTextureDesc.Width = uTextureWidth;
	xTextureDesc.Height = uTextureHeight;
	xTextureDesc.MipLevels = 1;
	xTextureDesc.ArraySize = 1;
	xTextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	// Turn multisampling on if available.
	u_int uMultiSamplingCount = 1;
	u_int uMultiSamplingQuality = 0;
	for (int i = 1; i <= 16; i *= 2)
	{
		u_int uQuality = -1;
		pxDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R32G32B32A32_FLOAT, i, &uQuality);
		if (uQuality > 0)
		{
			uMultiSamplingCount = i;
			uMultiSamplingQuality = uQuality - 1;
		}
	}
	xTextureDesc.SampleDesc.Count = uMultiSamplingCount;
	xTextureDesc.SampleDesc.Quality = uMultiSamplingQuality;
	xTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	xTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	xTextureDesc.CPUAccessFlags = 0;
	xTextureDesc.MiscFlags = 0;

	HRESULT hResult = pxDevice->CreateTexture2D(&xTextureDesc, NULL, &m_pxRenderTargetTexture);
	Ox93_Assert(SUCCEEDED(hResult), "Failed to create the 2D texture when creating the render texture");
	if (FAILED(hResult)) { return false; }

	D3D11_RENDER_TARGET_VIEW_DESC xRenderTargetViewDesc;
	xRenderTargetViewDesc.Format = xTextureDesc.Format;
	xRenderTargetViewDesc.ViewDimension = (uMultiSamplingCount == 1) ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DMS;
	xRenderTargetViewDesc.Texture2D.MipSlice = 0;

	hResult = pxDevice->CreateRenderTargetView(m_pxRenderTargetTexture, &xRenderTargetViewDesc, &m_pxRenderTargetView);
	Ox93_Assert(SUCCEEDED(hResult), "Failed to create the render target view when creating a render texture");
	if (FAILED(hResult)) { return false; }

	D3D11_SHADER_RESOURCE_VIEW_DESC xShaderResourceViewDesc;
	xShaderResourceViewDesc.Format = xTextureDesc.Format;
	xShaderResourceViewDesc.ViewDimension = (uMultiSamplingCount == 1) ? D3D11_SRV_DIMENSION_TEXTURE2D : D3D11_SRV_DIMENSION_TEXTURE2DMS;
	xShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	xShaderResourceViewDesc.Texture2D.MipLevels = 1;

	hResult = pxDevice->CreateShaderResourceView(m_pxRenderTargetTexture, &xShaderResourceViewDesc, &m_pxShaderResourceViewMS);
	Ox93_Assert(SUCCEEDED(hResult), "Failed to create the shader resource view when trying to create a render texture (MS)");
	if (FAILED(hResult)) { return false; }

	xShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	xTextureDesc.SampleDesc.Count = 1;
	xTextureDesc.SampleDesc.Quality = 0;
	ID3D11Texture2D* pxTexture;	
	hResult = pxDevice->CreateTexture2D(&xTextureDesc, nullptr, &pxTexture);
	Ox93_Assert(SUCCEEDED(hResult), "Failed to create the texture for the shader resource view in Ox93_RenderTexture");
	if (FAILED(hResult)) { return false; }

	hResult = pxDevice->CreateShaderResourceView(pxTexture, &xShaderResourceViewDesc, &m_pxShaderResourceView);
	Ox93_Assert(SUCCEEDED(hResult), "Failed to create the shader resource view when trying to create a render texture");
	if (FAILED(hResult)) { return false; }

	D3D11_TEXTURE2D_DESC xDepthBufferDesc;
	ZeroMemory(&xDepthBufferDesc, sizeof(xDepthBufferDesc));

	xDepthBufferDesc.Width = uTextureWidth;
	xDepthBufferDesc.Height = uTextureHeight;
	xDepthBufferDesc.MipLevels = 1;
	xDepthBufferDesc.ArraySize = 1;
	xDepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	xDepthBufferDesc.SampleDesc.Count = uMultiSamplingCount;
	xDepthBufferDesc.SampleDesc.Quality = uMultiSamplingQuality;
	xDepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	xDepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	xDepthBufferDesc.CPUAccessFlags = 0;
	xDepthBufferDesc.MiscFlags = 0;

	hResult = pxDevice->CreateTexture2D(&xDepthBufferDesc, NULL, &m_pxDepthStencilBuffer);
	if (FAILED(hResult)) { return false; }

	D3D11_DEPTH_STENCIL_VIEW_DESC xDepthStencilViewDesc;
	ZeroMemory(&xDepthStencilViewDesc, sizeof(xDepthStencilViewDesc));

	xDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	xDepthStencilViewDesc.ViewDimension = (uMultiSamplingCount == 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
	xDepthStencilViewDesc.Texture2D.MipSlice = 0;

	hResult = pxDevice->CreateDepthStencilView(m_pxDepthStencilBuffer, &xDepthStencilViewDesc, &m_pxDepthStencilView);
	if (FAILED(hResult)) { return false; }

	m_xViewPort.Width = (float)uTextureWidth;
	m_xViewPort.Height = (float)uTextureHeight;
	m_xViewPort.MinDepth = 0.0f;
	m_xViewPort.MaxDepth = 1.0f;
	m_xViewPort.TopLeftX = 0.0f;
	m_xViewPort.TopLeftY = 0.0f;

	return true;
}

void Ox93_RenderTexture::SetAsRenderTarget()
{
	ID3D11DeviceContext* pxDeviceContext = Ox93_D3D::GetDeviceContext();
	if (!pxDeviceContext) { return; }

	pxDeviceContext->OMSetRenderTargets(1, &m_pxRenderTargetView, m_pxDepthStencilView);

	pxDeviceContext->RSSetViewports(1, &m_xViewPort);
}

void Ox93_RenderTexture::ClearRenderTarget(float fR, float fG, float fB, float fAlpha)
{
	ID3D11DeviceContext* pxDeviceContext = Ox93_D3D::GetDeviceContext();
	if (!pxDeviceContext) { return; }

	float afColor[4] = { fR, fG, fB, fAlpha };

	pxDeviceContext->ClearRenderTargetView(m_pxRenderTargetView, afColor);
	pxDeviceContext->ClearDepthStencilView(m_pxDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

void Ox93_RenderTexture::UpdateShaderResource()
{
	ID3D11DeviceContext* pxDeviceContext = Ox93_D3D::GetDeviceContext();
	ID3D11Resource* pxResource;
	ID3D11Resource* pxResourceMS;

	m_pxShaderResourceView->GetResource(&pxResource);
	m_pxShaderResourceViewMS->GetResource(&pxResourceMS);

	pxDeviceContext->ResolveSubresource(pxResource, 0, pxResourceMS, 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
}