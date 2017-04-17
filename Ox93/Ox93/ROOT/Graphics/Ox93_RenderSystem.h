#ifndef __OX93_RENDERSYSTEM_H__
#define __OX93_RENDERSYSTEM_H__

#pragma once

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Ox93_Light.h"
#include "ROOT/Ox93_TextureHandler.h"
#include "ROOT/Math/Ox93_Math.h"

class Ox93_Entity_Renderable;

struct Ox93_Vertex
{
	Ox93_Vector_3 m_xPosition;
	Ox93_Vector_2 m_xTexture;
	Ox93_Vector_3 m_xNormal;
};

class Ox93_RenderSystem
{
public:
	static bool Create();
	static void Destroy();

	static bool Render();
	
	static void OutputShaderErrorMessage(ID3D10Blob* pxErrorMessage, HWND hWnd, WCHAR* pszShaderFilename);

protected:
	Ox93_RenderSystem();
	~Ox93_RenderSystem();

	bool Init();
	void Shutdown();
	void RenderShaders();

private:
	Ox93_Light* m_pxLight;

	static Ox93_RenderSystem* s_pxThis;
};

#endif // ifdef __OX93_RENDERSYSTEM_H__
