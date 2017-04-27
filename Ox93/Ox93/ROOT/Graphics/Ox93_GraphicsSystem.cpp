// Includes...
#include "Ox93_GraphicsSystem.h"
#include "Game/Entity/Camera/Ox93_Camera.h"
#include "ROOT/Specification/Ox93_Specification_System.h"

// Global Variables...
static const u_int g_uFPSFrameSpan = 100;

// Static Variables...
Ox93_GraphicsSystem* Ox93_GraphicsSystem::s_pxThis = nullptr;

const bool Ox93_GraphicsSystem::s_bFullscreen = false;
const bool Ox93_GraphicsSystem::s_bVSyncEnabled = true;
const float Ox93_GraphicsSystem::s_fScreenDepth = 1000.f;
const float Ox93_GraphicsSystem::s_fScreenNear = 0.1f;
float Ox93_GraphicsSystem::s_fFPS = 0.f;

Ox93_GraphicsSystem::Ox93_GraphicsSystem()
: m_pxD3D(nullptr)
, m_pxRenderSystem(nullptr)
, m_fPrevUpdateTime(0.f)
, m_uUpdateCounter(g_uFPSFrameSpan)
{
}

bool Ox93_GraphicsSystem::Create(u_int uScreenWidth, u_int uScreenHeight, HWND hWnd)
{
	Ox93_Assert(!s_pxThis, "Trying to recreate the Ox93_GraphicsSystem.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_GraphicsSystem;
	return s_pxThis->Init(uScreenWidth, uScreenHeight, hWnd);
}

void Ox93_GraphicsSystem::Destroy()
{
	if (!s_pxThis) { return; }

	s_pxThis->Shutdown();
	delete s_pxThis;
	s_pxThis = nullptr;
}

bool Ox93_GraphicsSystem::Init(u_int uScreenWidth, u_int uScreenHeight, HWND hWnd)
{
	// Create and initialize the Direct3D class
	bool bResult = Ox93_D3D::Create(uScreenWidth, uScreenHeight, s_bVSyncEnabled, hWnd, s_bFullscreen, s_fScreenDepth, s_fScreenNear);
	Ox93_Assert(bResult, "Failed to create Ox93_D3D.");
	if (!bResult) { return false; }

	// Create and initialize the texture handler
	bResult = Ox93_TextureHandler::Create();
	Ox93_Assert(bResult, "Failed to create Ox93_TextureHandler.");
	if (!bResult) { return false; }

	/* 
	 * Create and initialize the default camera
	 * Since this is an entity there is no need to delete
	 * it as the EntitySystem takes care of that
	 */
	Ox93_Camera* pxCamera = new Ox93_Camera(OX93_CLASS_CAMERA);
	if (!pxCamera) { return false; }

	pxCamera->SetPosition(0.0f, 0.0f, -10.0f);
	Ox93_Camera::SetActive(pxCamera);

	// Create and initialize the Render System
	bResult = Ox93_RenderSystem::Create();
	Ox93_Assert(bResult, "Failed to create Ox93_RenderSystem.");
	if (!bResult) { return false; }

	m_fPrevUpdateTime = timeGetTime();
	m_uUpdateCounter = g_uFPSFrameSpan;

	return true;
}


void Ox93_GraphicsSystem::Shutdown()
{
	Ox93_RenderSystem::Destroy();

	Ox93_TextureHandler::Destroy();
	Ox93_D3D::Destroy();
}


bool Ox93_GraphicsSystem::Frame()
{
	if (!s_pxThis) { return false; }

	// Track and display the FPS counter
	s_pxThis->m_uUpdateCounter--;
	if (s_pxThis->m_uUpdateCounter == 0)
	{
		s_pxThis->m_uUpdateCounter = g_uFPSFrameSpan;

		const float fThisUpdateTime = timeGetTime();
		s_fFPS = g_uFPSFrameSpan * 1000.f / (fThisUpdateTime - s_pxThis->m_fPrevUpdateTime);
		s_pxThis->m_fPrevUpdateTime = fThisUpdateTime;
	}

	s_pxThis->CentreSkybox();
	return s_pxThis->Render();
}


bool Ox93_GraphicsSystem::Render()
{
	// Clear the buffer to a black background
	m_pxD3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Do the render processing.
	Ox93_RenderSystem::Render();

	// Present the rendered scene to the screen.
	m_pxD3D->EndScene();

	return true;
}

void Ox93_GraphicsSystem::CentreSkybox()
{
	Ox93_Skybox* pxSkybox = Ox93_Skybox::GetActiveSkybox();
	if (!pxSkybox)
	{
		const Ox93_Specification* pxSpec = Ox93_Specification_System::GetSpecificationByHash(uOX93_SPEC_NAME_SKYBOX);
		if (pxSpec)
		{
			pxSkybox = Ox93_Skybox::Create();
			pxSkybox->InitFromSpecification(pxSpec);
		}
	}

	if (Ox93_Camera::GetActive())
	{
		pxSkybox->SetPosition(Ox93_Camera::GetActive()->GetPosition());
	}
}