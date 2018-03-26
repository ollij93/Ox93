// External Includes...
#include <fstream>

// Includes...
#include "Ox93_RenderSystem.h"
#include "Game/Entity/PhysicalObject/Character/Ox93_Character.h"
#include "ROOT/Assert/Ox93_Assert.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Graphics/Shader/Ox93_BitMapShader.h"
#include "ROOT/Graphics/Shader/Ox93_DepthShader.h"
#include "ROOT/Graphics/Shader/Ox93_LightShader.h"
#include "ROOT/Graphics/Shader/Ox93_SkyboxShader.h"
#include "ROOT/Graphics/Shader/Ox93_TerrainShader.h"

// Static Variables...
Ox93_RenderSystem* Ox93_RenderSystem::s_pxThis = nullptr;

Ox93_RenderSystem::Ox93_RenderSystem()
{
}

Ox93_RenderSystem::~Ox93_RenderSystem()
{
}

bool Ox93_RenderSystem::Create()
{
	Ox93_Assert(!s_pxThis, "Attempting to recreate the RenderSystem");
	if (s_pxThis) { return false; }

	// 3D
	Ox93_DepthShader::Create();
	Ox93_LightShader::Create();
	Ox93_SkyboxShader::Create();
	Ox93_TerrainShader::Create();
	// 2D
	Ox93_BitMapShader::Create();

	s_pxThis = new Ox93_RenderSystem;
	return s_pxThis->Init();
}

void Ox93_RenderSystem::Destroy()
{
	if (!s_pxThis) { return; }

	// 3D
	Ox93_DepthShader::Destroy();
	Ox93_LightShader::Destroy();
	Ox93_SkyboxShader::Destroy();
	Ox93_TerrainShader::Destroy();
	// 2D
	Ox93_BitMapShader::Destroy();

	s_pxThis->Shutdown();
	delete s_pxThis;
	s_pxThis = nullptr;
}

bool Ox93_RenderSystem::Init()
{
	return true;
}


void Ox93_RenderSystem::Shutdown()
{
}

bool Ox93_RenderSystem::Render()
{
	if (!s_pxThis) { return false; }

	s_pxThis->RenderShaders();
	return true;
}

void Ox93_RenderSystem::RenderShaders()
{
	// Ensure the Z Buffer is enabled before 3D rendering
	Ox93_D3D::SetZBufferEnabled(true);

	Ox93_Light::RenderList();
	if(Ox93_Character::GetLocalPlayer())
	{
		Ox93_Character::GetLocalPlayer()->SetCameraActive();
	}
	Ox93_Camera::RenderActive();

	Ox93_LightShader::Render();
	Ox93_SkyboxShader::Render();
	Ox93_TerrainShader::Render();

	/////////////////////////
	// 2D RENDERING
	/////////////////////////
	
	// Disable the Z buffer for 2D rendering
	Ox93_D3D::SetZBufferEnabled(false);
	
	Ox93_BitMapShader::Render();
}

/////////////////////////////////////////////////////
// Global function for all shaders
/////////////////////////////////////////////////////
void Ox93_RenderSystem::OutputShaderErrorMessage(ID3D10Blob* pxErrorMessage, HWND hWnd, WCHAR* pszShaderFilename)
{
	char* pcCompileErrors = (char*)(pxErrorMessage->GetBufferPointer());
	u_long ulBufferSize = pxErrorMessage->GetBufferSize();

	std::ofstream fOutStream;
	fOutStream.open("shader-error.txt");
	for (int i = 0; i < ulBufferSize; i++)
	{
		fOutStream << pcCompileErrors[i];
	}
	fOutStream.close();

	pxErrorMessage->Release();
	pxErrorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hWnd, L"Error compiling shader.  Check shader-error.txt for message.", pszShaderFilename, MB_OK);
}