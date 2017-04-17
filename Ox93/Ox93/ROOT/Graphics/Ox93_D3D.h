#ifndef __OX93_D3D_H__
#define __OX93_D3D_H__

#include "Ox93_Core.h"
#include "ROOT/Math/Ox93_Math.h"

class Ox93_D3D
{
public:
	static bool Create(int, int, bool, HWND, bool, float, float);
	static void Destroy();

	static ID3D11Device* GetDevice();
	static ID3D11DeviceContext* GetDeviceContext();

	static float GetFOV() { return (s_pxThis) ? s_pxThis->m_fFOV : 0.f; }
	static void SetFOV(float fFOV);
	static void IncreaseFOV() { SetFOV(GetFOV() + Ox93_Math::PI / 36.f); }
	static void DecreaseFOV() { SetFOV(GetFOV() - Ox93_Math::PI / 36.f); }

	static void GetProjectionMatrix(DirectX::XMMATRIX&);
	static void GetWorldMatrix(DirectX::XMMATRIX&);
	static void GetOrthoMatrix(DirectX::XMMATRIX&);

	static void GetVideoCardInfo(char*, int&);

	static void BeginScene(float, float, float, float);
	static void EndScene();

	static void SetZBufferEnabled(bool bSet);
	static void RestoreDefaultRenderTarget();
	static void ResetViewport();

	static float s_fReadOnlyDegreesFOV;

protected:
	Ox93_D3D();
	Ox93_D3D(const Ox93_D3D&);
	~Ox93_D3D();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	DirectX::XMMATRIX m_projectionMatrix;
	DirectX::XMMATRIX m_worldMatrix;
	DirectX::XMMATRIX m_orthoMatrix;
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11BlendState* m_alphaEnableBlendingState;
	D3D11_VIEWPORT m_viewPort;

	float m_fFOV;
	float m_fAspectRatio;
	float m_fScreenNear;
	float m_fScreenDepth;

	static Ox93_D3D* s_pxThis;
};

#endif // ifndef __OX93_D3D_H__