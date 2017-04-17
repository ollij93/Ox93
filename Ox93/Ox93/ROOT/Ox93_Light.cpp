// Includes...
#include "Ox93_Light.h"
#include "Game/Entity/PhysicalObject/Character/Ox93_Character.h"
#include "ROOT/Graphics/2D/Ox93_RenderTexture.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Graphics/Shader/Ox93_DepthShader.h"

Ox93_Light::Ox93_Light()
: m_xViewMatrix()
, m_xOrthoMatrix()
, m_xAmbientColor()
, m_xDiffuseColor()
, m_xPosition(Ox93_Math::ZeroVector3)
, m_xDirection(Ox93_Vector_3(1.f,0.f,0.f))
, m_pxRenderTexture(nullptr)
{
	m_xOrthoMatrix = DirectX::XMMatrixOrthographicLH(50.f, 50.f, 0.f, 1000.f);
}

Ox93_Light::~Ox93_Light()
{
	if (m_pxRenderTexture)
	{
		delete m_pxRenderTexture;
		m_pxRenderTexture = nullptr;
	}
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

void Ox93_Light::SetPosition(float fX, float fY, float fZ)
{
	m_xPosition = Ox93_Vector_3(fX, fY, fZ);

	m_xViewMatrix = DirectX::XMMatrixLookAtLH(m_xPosition, m_xPosition + m_xDirection * 10.f, Ox93_Vector_3(0.f, 1.f, 0.f));
}

void Ox93_Light::SetDirection(float fX, float fY, float fZ)
{
	m_xDirection = Ox93_Vector_3(fX, fY, fZ);
	m_xDirection.Normalize();

	m_xViewMatrix = DirectX::XMMatrixLookAtLH(m_xPosition, m_xPosition + m_xDirection * 10.f, Ox93_Vector_3(0.f, 1.f, 0.f));
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