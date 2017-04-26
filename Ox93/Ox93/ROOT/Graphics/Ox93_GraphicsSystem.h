#ifndef __OX93_GRAPHICS_H__
#define __OX93_GRAPHICS_H__

// Includes...
#include "Ox93_Core.h"
#include "Game/World/Skybox/Ox93_Skybox.h"
#include "ROOT/Graphics/Ox93_D3D.h"
#include "ROOT/Graphics/Ox93_RenderSystem.h"

class Ox93_GraphicsSystem
{
public:
	static bool Create(u_int uScreenWidth, u_int uScreenHeight, HWND hWnd);
	static void Destroy();

	static bool Frame();

	const static bool s_bFullscreen;
	const static bool s_bVSyncEnabled;
	const static float s_fScreenDepth;
	const static float s_fScreenNear;

	static float s_fFPS;

protected:
	Ox93_GraphicsSystem();
	~Ox93_GraphicsSystem() {};

	bool Init(u_int uScreenWidth, u_int uScreenHeight, HWND hWnd);
	void Shutdown();

private:
	bool Render();
	void CentreSkybox();

	Ox93_D3D* m_pxD3D;
	Ox93_RenderSystem* m_pxRenderSystem;

	float m_fPrevUpdateTime;
	u_int m_uUpdateCounter;

	static Ox93_GraphicsSystem* s_pxThis;
};

#endif //ifdef __OX93_GRAPHICS_H__