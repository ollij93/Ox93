#pragma once

#ifndef __OX93_CAMERA_H__
#define __OX93_CAMERA_H__

// Includes...
#include "Ox93_Core.h"
#include "ROOT/Math/Ox93_Math.h"

class Ox93_Camera
{
public:
	Ox93_Camera();
	~Ox93_Camera() {};

	void HandleInput();
	
	void SetPosition(float fX, float fY, float fZ) { m_xPosition = Ox93_Vector_3(fX, fY, fZ); }
	void SetPosition(Ox93_Vector_3 xPosition) { m_xPosition = xPosition; }
	Ox93_Vector_3 GetPosition() const { return m_xPosition; }

	void SetOrientation(Ox93_Matrix3x3 xOrientation) { m_xOrientation = xOrientation; }
	Ox93_Matrix3x3 GetOrientation() const { return m_xOrientation; }

	void Render();
	void GetViewMatrix(DirectX::XMMATRIX& xViewMatrix) const { xViewMatrix = m_xViewMatrix; }

private:
	DirectX::XMMATRIX m_xViewMatrix;

	Ox93_Vector_3 m_xPosition;
	Ox93_Matrix3x3 m_xOrientation;
};

#endif // ifndef __OX93_CAMERA_H__
