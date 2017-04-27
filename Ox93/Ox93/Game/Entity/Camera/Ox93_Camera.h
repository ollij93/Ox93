#pragma once

#ifndef __OX93_CAMERA_H__
#define __OX93_CAMERA_H__

// Includes...
#include "Ox93_Core.h"
#include "Game/Entity/Ox93_Entity.h"
#include "ROOT/Math/Ox93_Math.h"

class Ox93_Camera : public Ox93_Entity
{
public:
	Ox93_Camera(u_int uClassification);
	~Ox93_Camera();

	void HandleInput();
	void Render();

	// Getters & Setters...
	void GetViewMatrix(DirectX::XMMATRIX& xViewMatrix) const { xViewMatrix = m_xViewMatrix; }

	// Statics...
	static void RenderActive() { if (s_pxActiveCamera) { s_pxActiveCamera->Render(); } }

	// Statics Getters & Setters...
	static Ox93_Camera* GetActive() { return s_pxActiveCamera; }
	static void SetActive(Ox93_Camera* pxCamera) { s_pxActiveCamera = pxCamera; }

private:
	DirectX::XMMATRIX m_xViewMatrix;

	// Statics...
	static Ox93_Camera* s_pxActiveCamera;

	typedef Ox93_Entity PARENT;
};

#endif // ifndef __OX93_CAMERA_H__
