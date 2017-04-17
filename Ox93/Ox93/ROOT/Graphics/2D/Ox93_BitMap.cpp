// Includes...
#include "Ox93_BitMap.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Graphics/Ox93_RenderSystem.h"
#include "ROOT/Graphics/Shader/Ox93_BitMapShader.h"

// Statics...
std::list<Ox93_BitMap*> Ox93_BitMap::s_lpxBitMapList;

Ox93_BitMap::Ox93_BitMap()
: m_uTextureHash(OX93_HASH_UNSET)
, m_pxTexture(nullptr)
, m_uWidth(0)
, m_uHeight(0)
, m_fTextureLeft(0.f)
, m_fTextureRight(0.f)
, m_fTextureTop(0.f)
, m_fTextureBottom(0.f)
, m_iPosX(0)
, m_iPosY(0)
, m_bMovedSinceRender(true)
, m_pxVertexBuffer(nullptr)
, m_pxIndexBuffer(nullptr)
{
	Ox93_BitMapShader::AddToRenderList(this);
}

bool Ox93_BitMap::Init(Ox93_Hash uTextureHash, u_int uWidth, u_int uHeight, 
	float fTextureLeft /* = 0.f*/, float fTextureRight /* = 1.f*/, float fTextureTop /* = 0.f*/, float fTextureBottom /* = 1.f*/)
{
	m_uTextureHash = uTextureHash;
	m_pxTexture = nullptr;
	m_uWidth = uWidth;
	m_uHeight = uHeight;
	m_fTextureLeft = fTextureLeft;
	m_fTextureRight = fTextureRight;
	m_fTextureTop = fTextureTop;
	m_fTextureBottom = fTextureBottom;

	return InitBuffers();
}

bool Ox93_BitMap::Init(ID3D11ShaderResourceView* pxTexture, u_int uWidth, u_int uHeight,
	float fTextureLeft /* = 0.f*/, float fTextureRight /* = 1.f*/, float fTextureTop /* = 0.f*/, float fTextureBottom /* = 1.f*/)
{
	m_uTextureHash = OX93_HASH_UNSET;
	m_pxTexture = pxTexture;
	m_uWidth = uWidth;
	m_uHeight = uHeight;
	m_fTextureLeft = fTextureLeft;
	m_fTextureRight = fTextureRight;
	m_fTextureTop = fTextureTop;
	m_fTextureBottom = fTextureBottom;

	return InitBuffers();
}

void Ox93_BitMap::Shutdown()
{
	Ox93_BitMapShader::RemoveFromRenderList(this);
	ShutdownBuffers();
}

void Ox93_BitMap::Render()
{
	UpdateBuffers();
	RenderBuffers();
}

bool Ox93_BitMap::InitBuffers()
{
	ID3D11Device* pxDevice = Ox93_D3D::GetDevice();
	Ox93_Assert(pxDevice, "Could not get the D3D device to initialize the BitMap buffers too.");
	if (!pxDevice) { return false; }

	Ox93_Vertex* pxVertices = new Ox93_Vertex[6];
	u_int* puIndices = new u_int[6];
	if (!pxVertices || !puIndices) { return false; }

	memset(pxVertices, 0, sizeof(Ox93_Vertex) * 6);
	for (u_int u = 0; u < 6; u++)
	{
		puIndices[u] = u;
	}

	D3D11_BUFFER_DESC xVertexBufferDesc;
	xVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	xVertexBufferDesc.ByteWidth = sizeof(Ox93_Vertex) * 6;
	xVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	xVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	xVertexBufferDesc.MiscFlags = 0;
	xVertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA xVertexData;
	xVertexData.pSysMem = pxVertices;
	xVertexData.SysMemPitch = 0;
	xVertexData.SysMemSlicePitch = 0;

	HRESULT hResult = pxDevice->CreateBuffer(&xVertexBufferDesc, &xVertexData, &m_pxVertexBuffer);
	if (FAILED(hResult)) { return false; }

	D3D11_BUFFER_DESC xIndexBufferDesc;
	xIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	xIndexBufferDesc.ByteWidth = sizeof(u_int) * 6;
	xIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	xIndexBufferDesc.CPUAccessFlags = 0;
	xIndexBufferDesc.MiscFlags = 0;
	xIndexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA xIndexData;
	xIndexData.pSysMem = puIndices;
	xIndexData.SysMemPitch = 0;
	xIndexData.SysMemSlicePitch = 0;

	hResult = pxDevice->CreateBuffer(&xIndexBufferDesc, &xIndexData, &m_pxIndexBuffer);
	if (FAILED(hResult)) { return false; }

	delete[] pxVertices;
	pxVertices = nullptr;

	delete[] puIndices;
	puIndices = nullptr;

	return true;
}

bool Ox93_BitMap::UpdateBuffers()
{
	// OJ - If the position is unchanged break out early
	if(!m_bMovedSinceRender)
	{
		return true;
	}
	m_bMovedSinceRender = false;

	Ox93_Vertex* pxVertices = new Ox93_Vertex[6];
	if (!pxVertices) { return false; }

	// Set up the vertices 
	pxVertices[0].m_xPosition = Ox93_Vector_3(static_cast<float>(m_iPosX), static_cast<float>(m_iPosY - (int)m_uHeight), 0.f);
	pxVertices[0].m_xTexture = Ox93_Vector_2(m_fTextureLeft, m_fTextureBottom);
	pxVertices[0].m_xNormal = Ox93_Vector_3(0.f, 0.f, -1.f);

	pxVertices[1].m_xPosition = Ox93_Vector_3(static_cast<float>(m_iPosX), static_cast<float>(m_iPosY), 0.f);
	pxVertices[1].m_xTexture = Ox93_Vector_2(m_fTextureLeft, m_fTextureTop);
	pxVertices[1].m_xNormal = Ox93_Vector_3(0.f, 0.f, -1.f);

	pxVertices[2].m_xPosition = Ox93_Vector_3(static_cast<float>(m_iPosX + (int)m_uWidth), static_cast<float>(m_iPosY), 0.f);
	pxVertices[2].m_xTexture = Ox93_Vector_2(m_fTextureRight, m_fTextureTop);
	pxVertices[2].m_xNormal = Ox93_Vector_3(0.f, 0.f, -1.f);
	
	pxVertices[3].m_xPosition = Ox93_Vector_3(static_cast<float>(m_iPosX), static_cast<float>(m_iPosY - (int)m_uHeight), 0.f);
	pxVertices[3].m_xTexture = Ox93_Vector_2(m_fTextureLeft, m_fTextureBottom);
	pxVertices[3].m_xNormal = Ox93_Vector_3(0.f, 0.f, -1.f);

	pxVertices[4].m_xPosition = Ox93_Vector_3(static_cast<float>(m_iPosX + (int)m_uWidth), static_cast<float>(m_iPosY), 0.f);
	pxVertices[4].m_xTexture = Ox93_Vector_2(m_fTextureRight, m_fTextureTop);
	pxVertices[4].m_xNormal = Ox93_Vector_3(0.f, 0.f, -1.f);

	pxVertices[5].m_xPosition = Ox93_Vector_3(static_cast<float>(m_iPosX + (int)m_uWidth), static_cast<float>(m_iPosY - (int)m_uHeight), 0.f);
	pxVertices[5].m_xTexture = Ox93_Vector_2(m_fTextureRight, m_fTextureBottom);
	pxVertices[5].m_xNormal = Ox93_Vector_3(0.f, 0.f, -1.f);

	// Copy the new vertex buffer information into the existing dynamic buffer
	if (Ox93_D3D::GetDeviceContext())
	{
		D3D11_MAPPED_SUBRESOURCE xMappedResource;
		HRESULT hResult = Ox93_D3D::GetDeviceContext()->Map(m_pxVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &xMappedResource);
		if (FAILED(hResult)) { return false; }

		Ox93_Vertex* pxData = static_cast<Ox93_Vertex*>(xMappedResource.pData);

		memcpy(pxData, static_cast<void*>(pxVertices), sizeof(Ox93_Vertex) * 6);

		Ox93_D3D::GetDeviceContext()->Unmap(m_pxVertexBuffer, 0);
	}

	delete[] pxVertices;
	pxVertices = nullptr;

	return true;
}

void Ox93_BitMap::RenderBuffers()
{
	if (Ox93_D3D::GetDeviceContext())
	{
		u_int uStride = sizeof(Ox93_Vertex);
		u_int uOffset = 0;

		Ox93_D3D::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pxVertexBuffer, &uStride, &uOffset);
		Ox93_D3D::GetDeviceContext()->IASetIndexBuffer(m_pxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		Ox93_D3D::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}

void Ox93_BitMap::ShutdownBuffers()
{
	if (m_pxIndexBuffer)
	{
		m_pxIndexBuffer->Release();
		m_pxIndexBuffer = nullptr;
	}
	if (m_pxVertexBuffer)
	{
		m_pxVertexBuffer->Release();
		m_pxVertexBuffer = nullptr;
	}
}