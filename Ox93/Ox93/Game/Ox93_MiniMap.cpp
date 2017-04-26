// Includes...
#include "Ox93_MiniMap.h"
#include "Game/Camera/Ox93_Camera.h"
#include "ROOT/Graphics/2D/Ox93_BitMap.h"
#include "ROOT/Graphics/2D/Ox93_RenderTexture.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Graphics/Ox93_GraphicsSystem.h"
#include "ROOT/Graphics/Shader/Ox93_TerrainShader.h"

// Globals...
static const float fOX93_MINIMAP_FOV = Ox93_Math::PI / 8.f;
static const float fOX93_MINIMAP_VIEWHEIGHT = 70.f;


Ox93_MiniMap::Ox93_MiniMap()
: m_pxBitMap(nullptr)
, m_pxRenderTexture(nullptr)
{
}

Ox93_MiniMap::~Ox93_MiniMap()
{
	if (m_pxBitMap)
	{
		m_pxBitMap->Shutdown();
		delete m_pxBitMap;
		m_pxBitMap = nullptr;
	}
	if (m_pxRenderTexture)
	{
		delete m_pxRenderTexture;
		m_pxRenderTexture = nullptr;
	}
}

bool Ox93_MiniMap::Init()
{
	if (m_pxRenderTexture)
	{
		delete m_pxRenderTexture;
		m_pxRenderTexture = nullptr;
	}
	m_pxRenderTexture = new Ox93_RenderTexture;
	bool bResult = m_pxRenderTexture->Init(800, 600);
	if (!bResult) { return false; }

	if (m_pxBitMap)
	{
		m_pxBitMap->Shutdown();
		delete m_pxBitMap;
		m_pxBitMap = nullptr;
	}
	m_pxBitMap = new Ox93_BitMap;

	bResult = m_pxBitMap->Init(m_pxRenderTexture->GetShaderResourceView(), 200, 150);
	if (!bResult) { return false; }

	m_pxBitMap->SetPosition(200, 300);

	return true;
}

void Ox93_MiniMap::Update()
{
	Ox93_D3D::SetZBufferEnabled(true);
	const float fFOV = Ox93_D3D::GetFOV();
	Ox93_D3D::SetFOV(fOX93_MINIMAP_FOV);

	m_pxRenderTexture->SetAsRenderTarget();
	m_pxRenderTexture->ClearRenderTarget(0.f,0.f,0.f,1.f);

	Ox93_Vector_3 xCameraPos;
	Ox93_Matrix3x3 xCameraOri;
	Ox93_Camera* pxCamera = Ox93_GraphicsSystem::GetCamera();
	if (pxCamera)
	{
		xCameraPos = pxCamera->GetPosition();
		xCameraOri = pxCamera->GetOrientation();
		pxCamera->SetPosition(xCameraPos + Ox93_Vector_3(0.f, fOX93_MINIMAP_VIEWHEIGHT, 0.f));

		Ox93_Matrix3x3 xSetOri(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, -1.f, 0.f);
		pxCamera->SetOrientation(xSetOri);

		pxCamera->Render();

		// Light Shader
		Ox93_LightShader::Render();

		// Terrain Shader
		Ox93_TerrainShader::Render();

		pxCamera->SetPosition(xCameraPos);
		pxCamera->SetOrientation(xCameraOri);
	}

	Ox93_D3D::SetFOV(fFOV);
	Ox93_D3D::RestoreDefaultRenderTarget();

	m_pxRenderTexture->UpdateShaderResource();
}
