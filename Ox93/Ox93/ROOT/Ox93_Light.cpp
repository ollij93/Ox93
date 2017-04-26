// Includes...
#include "Ox93_Light.h"
#include "Game/Entity/PhysicalObject/Character/Ox93_Character.h"
#include "Game/LoadSave/ChunkStream/Ox93_ChunkStream.h"
#include "ROOT/Graphics/2D/Ox93_RenderTexture.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Graphics/Shader/Ox93_DepthShader.h"

// Statics...
std::list<Ox93_Light*> Ox93_Light::s_lpxLightsList;

Ox93_Light::Ox93_Light(u_int uClassification)
: PARENT(uClassification)
, m_xViewMatrix()
, m_xOrthoMatrix()
, m_xAmbientColor()
, m_xDiffuseColor()
, m_pxRenderTexture(nullptr)
, m_bFollowPlayer(false)
{
	m_xOrthoMatrix = DirectX::XMMatrixOrthographicLH(50.f, 50.f, 0.f, 1000.f);

	InitRenderTexture();
	AddToLightList(this);
}

Ox93_Light::~Ox93_Light()
{
	if (m_pxRenderTexture)
	{
		delete m_pxRenderTexture;
		m_pxRenderTexture = nullptr;
	}

	RemoveFromLightList(this);
}

void Ox93_Light::Update()
{
	const Ox93_Character *pxPlayer = Ox93_Character::GetLocalPlayer();
	if (m_bFollowPlayer && pxPlayer)
	{
		SetPosition(pxPlayer->GetPosition() - GetDirection() * 50.0f);
	}
}

void Ox93_Light::ReadFromChunkStream(const Ox93_ChunkStream& xChunkStream)
{
	PARENT::ReadFromChunkStream(xChunkStream);
	xChunkStream >> m_xAmbientColor.fRed;
	xChunkStream >> m_xAmbientColor.fGreen;
	xChunkStream >> m_xAmbientColor.fBlue;
	xChunkStream >> m_xAmbientColor.fAlpha;

	xChunkStream >> m_xDiffuseColor.fRed;
	xChunkStream >> m_xDiffuseColor.fGreen;
	xChunkStream >> m_xDiffuseColor.fBlue;
	xChunkStream >> m_xDiffuseColor.fAlpha;

	xChunkStream >> m_bFollowPlayer;

	// Ensure position and orientation setters are called to correctly set matrices
	SetPosition(m_xPosition);
	SetOrientation(m_xOrientation);
}

void Ox93_Light::WriteToChunkStream(Ox93_ChunkStream& xChunkStream) const
{
	PARENT::WriteToChunkStream(xChunkStream);
	xChunkStream << m_xAmbientColor.fRed;
	xChunkStream << m_xAmbientColor.fGreen;
	xChunkStream << m_xAmbientColor.fBlue;
	xChunkStream << m_xAmbientColor.fAlpha;

	xChunkStream << m_xDiffuseColor.fRed;
	xChunkStream << m_xDiffuseColor.fGreen;
	xChunkStream << m_xDiffuseColor.fBlue;
	xChunkStream << m_xDiffuseColor.fAlpha;

	xChunkStream << m_bFollowPlayer;
}

void Ox93_Light::SetPosition(float fX, float fY, float fZ)
{
	PARENT::SetPosition(fX, fY, fZ);

	m_xViewMatrix = DirectX::XMMatrixLookAtLH(m_xPosition, m_xPosition + GetDirection() * 10.f, Ox93_Vector_3(0.f, 1.f, 0.f));
}

void Ox93_Light::SetPosition(Ox93_Vector_3 xPosition)
{
	PARENT::SetPosition(xPosition);

	m_xViewMatrix = DirectX::XMMatrixLookAtLH(m_xPosition, m_xPosition + GetDirection() * 10.f, Ox93_Vector_3(0.f, 1.f, 0.f));
}

void Ox93_Light::SetOrientation(Ox93_Matrix3x3 xOrientation)
{
	PARENT::SetOrientation(xOrientation);

	m_xViewMatrix = DirectX::XMMatrixLookAtLH(m_xPosition, m_xPosition + GetDirection() * 10.f, Ox93_Vector_3(0.f, 1.f, 0.f));
}

bool Ox93_Light::InitRenderTexture()
{
	m_pxRenderTexture = new Ox93_RenderTexture;
	return m_pxRenderTexture->Init(2048, 2048);
}

void Ox93_Light::SetAmbientColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
	m_xAmbientColor = { fRed, fGreen, fBlue, fAlpha };
}

void Ox93_Light::SetDiffuseColor(float fRed, float fGreen, float fBlue, float fAlpha)
{
	m_xDiffuseColor = { fRed, fGreen, fBlue, fAlpha };
}

void Ox93_Light::Render()
{
	if (m_pxRenderTexture)
	{
		m_pxRenderTexture->SetAsRenderTarget();
		m_pxRenderTexture->ClearRenderTarget(1.f, 1.f, 1.f, 1.f);

		DirectX::XMMATRIX xOrthoMatrix = m_xOrthoMatrix;
		DirectX::XMMATRIX xViewMatrix = m_xViewMatrix;
		Ox93_DepthShader::Render(xViewMatrix, xOrthoMatrix);

		m_pxRenderTexture->UpdateShaderResource();

		Ox93_D3D::RestoreDefaultRenderTarget();
		Ox93_D3D::ResetViewport();
	}
}

void Ox93_Light::RenderList()
{
	/*
	 * This function is used to render the render textures of all the lights in the list
	 */
	std::list<Ox93_Light*>::const_iterator xIter;
	for (xIter = s_lpxLightsList.begin(); xIter != s_lpxLightsList.end(); ++xIter)
	{
		Ox93_Light* pxLight = *xIter;
		if (pxLight)
		{
			pxLight->Render();
		}
	}
}