// Includes...
#include "Ox93_Skybox.h"
#include "Game/LoadSave/ChunkStream/Ox93_ChunkStream.h"
#include "ROOT/Graphics/Ox93_RenderSystem.h"
#include "ROOT/Graphics/Shader/Ox93_SkyboxShader.h"
#include "ROOT/Specification/Ox93_Specification.h"

// Statics...
Ox93_Skybox* Ox93_Skybox::s_pxActiveSkybox = nullptr;

Ox93_Skybox::Ox93_Skybox()
: m_fRadius(0.f)
, m_uVerticalFaces(0)
, m_uHorizontalFaces(0)
, m_uTextureHash(uOX93_TEXTURE_HASH_SKYBOX)
, m_xPosition(Ox93_Math::ZeroVector3)
, m_pxVertexBuffer(nullptr)
, m_pxIndexBuffer(nullptr)
{
	s_pxActiveSkybox = this;
}

Ox93_Skybox::~Ox93_Skybox()
{
	if (m_pxVertexBuffer)
	{
		m_pxVertexBuffer->Release();
		m_pxVertexBuffer = nullptr;
	}

	if (m_pxIndexBuffer)
	{
		m_pxIndexBuffer->Release();
		m_pxIndexBuffer = nullptr;
	}
}

void Ox93_Skybox::InitFromSpecification(const Ox93_Specification* pxSpecification)
{
	if (pxSpecification)
	{
		m_fRadius = pxSpecification->GetValueAsFloat(uOX93_SPEC_PARAM_RADIUS, 0.f);
		m_uVerticalFaces = pxSpecification->GetValueAsUInt(uOX93_SPEC_PARAM_VERT_FACES, 0);
		m_uHorizontalFaces = pxSpecification->GetValueAsUInt(uOX93_SPEC_PARAM_HOR_FACES, 0);
		m_uTextureHash = pxSpecification->GetValueAsHash(uOX93_SPEC_PARAM_TEXTURE, OX93_HASH_UNSET);
	}

	InitBuffers(Ox93_D3D::GetDevice());
}

void Ox93_Skybox::Render()
{
	ID3D11DeviceContext* pxDeviceContext = Ox93_D3D::GetDeviceContext();
	if (!pxDeviceContext) { return; }

	u_int uSize = sizeof(Ox93_Vertex);
	u_int uOffset = 0;

	pxDeviceContext->IASetVertexBuffers(0, 1, &m_pxVertexBuffer, &uSize, &uOffset);
	pxDeviceContext->IASetIndexBuffer(m_pxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Ox93_Skybox::InitBuffers(ID3D11Device* pxDevice)
{
	Ox93_Vertex* pxVertices = new Ox93_Vertex[6 * m_uHorizontalFaces * m_uVerticalFaces];
	u_int* puIndices = new u_int[6 * m_uHorizontalFaces * m_uVerticalFaces];

	for (u_int uVertFace = 0; uVertFace < m_uVerticalFaces; uVertFace++)
	{
		const float fThetaUpper = 2.f * Ox93_Math::PI * uVertFace / m_uVerticalFaces;
		const float fThetaLower = 2.f * Ox93_Math::PI * (uVertFace + 1) / m_uVerticalFaces;

		const float fYUpper = m_fRadius * cos(fThetaUpper);
		const float fYLower = m_fRadius * cos(fThetaLower);

		for (u_int uHorFace = 0; uHorFace < m_uHorizontalFaces; uHorFace++)
		{
			const float fPhiHigh = 2.f * Ox93_Math::PI * uHorFace / m_uHorizontalFaces;
			const float fPhiLow = 2.f * Ox93_Math::PI * (uHorFace + 1) / m_uHorizontalFaces;

			const float fXUpperHigh = m_fRadius * sin(fThetaUpper) * cos(fPhiHigh);
			const float fXUpperLow = m_fRadius * sin(fThetaUpper) * cos(fPhiLow);
			const float fXLowerHigh = m_fRadius * sin(fThetaLower) * cos(fPhiHigh);
			const float fXLowerLow = m_fRadius * sin(fThetaLower) * cos(fPhiLow);

			const float fZUpperHigh = m_fRadius * sin(fThetaUpper) * sin(fPhiHigh);
			const float fZUpperLow = m_fRadius * sin(fThetaUpper) * sin(fPhiLow);
			const float fZLowerHigh = m_fRadius * sin(fThetaLower) * sin(fPhiHigh);
			const float fZLowerLow = m_fRadius * sin(fThetaLower) * sin(fPhiLow);

			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 0].m_xPosition = { fXUpperLow, fYUpper, fZUpperLow };
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 1].m_xPosition = { fXUpperHigh, fYUpper, fZUpperHigh };
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 2].m_xPosition = { fXLowerLow, fYLower, fZLowerLow };
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 3].m_xPosition = { fXLowerHigh, fYLower, fZLowerHigh };
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 4].m_xPosition = { fXLowerLow, fYLower, fZLowerLow };
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 5].m_xPosition = { fXUpperHigh, fYUpper, fZUpperHigh };

			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 0].m_xNormal = pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 0].m_xPosition;
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 1].m_xNormal = pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 1].m_xPosition;
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 2].m_xNormal = pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 2].m_xPosition;
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 3].m_xNormal = pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 3].m_xPosition;
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 4].m_xNormal = pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 4].m_xPosition;
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 5].m_xNormal = pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 5].m_xPosition;

			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 0].m_xTexture = { 1.f,1.f };
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 1].m_xTexture = { 0.f,1.f };
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 2].m_xTexture = { 1.f,0.f };
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 3].m_xTexture = { 0.f,0.f };
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 4].m_xTexture = { 1.f,0.f };
			pxVertices[6 * (uVertFace * m_uHorizontalFaces + uHorFace) + 5].m_xTexture = { 0.f,1.f };
		}
	}

	for (u_int u = 0; u < m_uHorizontalFaces * m_uVerticalFaces * 6; u++)
	{
		puIndices[u] = u;
	}

	D3D11_BUFFER_DESC xVertexBufferDesc;
	xVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	xVertexBufferDesc.ByteWidth = sizeof(Ox93_Vertex) * 6 * m_uHorizontalFaces * m_uVerticalFaces;
	xVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	xVertexBufferDesc.CPUAccessFlags = 0;
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
	xIndexBufferDesc.ByteWidth = sizeof(u_int) * 6 * m_uHorizontalFaces * m_uVerticalFaces;
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

	delete puIndices;
	puIndices = nullptr;

	return true;
}