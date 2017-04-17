// Includes...
#include "Ox93_Camera.h"
#include "ROOT/Ox93_InputSystem.h"

// Global Variables...
static const float fOX93_MOUSE_SENSITIVITY = 0.01f;
static const float fOX93_MOVEMENT_SPEED = 0.15f;

Ox93_Camera::Ox93_Camera()
: m_xViewMatrix()
, m_xPosition(Ox93_Math::ZeroVector3)
, m_xOrientation(Ox93_Math::IdentityMatrix3x3)
{
}

void Ox93_Camera::HandleInput()
{
	// Check for mouse movement
	if (Ox93_InputSystem::IsLMButtonDown() && Ox93_InputSystem::GetMouseMoved())
	{
		const Ox93_Vector_2 xMouseDisp = Ox93_InputSystem::GetMouseDisplacement();
		float fDeltaPhi = -xMouseDisp.x * fOX93_MOUSE_SENSITIVITY;
		float fDeltaTheta = -xMouseDisp.y * fOX93_MOUSE_SENSITIVITY;

		m_xOrientation.RotateLocalX(fDeltaTheta);
		m_xOrientation.RotateWorldY(fDeltaPhi);

		Ox93_InputSystem::SetCursorPosition(Ox93_InputSystem::GetMouseClickLocation());
	}

	// Handle the keyboard controls
	for (int iKey = 0; iKey < 256; iKey++)
	{
		if (!Ox93_InputSystem::IsKeyDown(iKey)) { continue; }

		// Process the action for each keyboard key being held
		switch (iKey)
		{
			case W_KEY:
			{
				// Move forward
				const Ox93_Vector_3 xDir(m_xOrientation.e20, m_xOrientation.e21, m_xOrientation.e22);
				m_xPosition += xDir * fOX93_MOVEMENT_SPEED;
				break;
			}
			case A_KEY:
			{
				// Move left
				Ox93_Vector_3 xDir(m_xOrientation.e22, 0, -m_xOrientation.e20);
				xDir.Normalize();

				m_xPosition -= xDir * fOX93_MOVEMENT_SPEED;
				break;
			}
			case S_KEY:
			{
				// Move backward
				const Ox93_Vector_3 xDir(m_xOrientation.e20, m_xOrientation.e21, m_xOrientation.e22);
				m_xPosition -= xDir * fOX93_MOVEMENT_SPEED;
				break;
				break;
			}
			case D_KEY:
			{
				// Move right
				Ox93_Vector_3 xDir(m_xOrientation.e22, 0, -m_xOrientation.e20);
				xDir.Normalize();

				m_xPosition += xDir * fOX93_MOVEMENT_SPEED;
				break;
			}
			case SPACE_KEY:
			{
				//Move vertically up
				m_xPosition.y += fOX93_MOVEMENT_SPEED;
				break;
			}
			case SHIFT_KEY:
			{
				//Move vertically down
				m_xPosition.y -= fOX93_MOVEMENT_SPEED;
				break;
			}
			default:
				break;
		}
	}
}

void Ox93_Camera::Render()
{
	DirectX::XMVECTOR xPos = GetPosition();
	DirectX::XMVECTOR xUpVec = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);
	DirectX::XMVECTOR xLookAt = DirectX::XMVectorSet(0.f, 0.f, 1.f, 0.f);

	Ox93_Matrix3x3 xOrientation = GetOrientation();

	// Create the rotation matrix from the objects orientation
	DirectX::XMMATRIX xRotationMatrix(xOrientation.e00, xOrientation.e01, xOrientation.e02, 0,
								xOrientation.e10, xOrientation.e11, xOrientation.e12, 0,
								xOrientation.e20, xOrientation.e21, xOrientation.e22, 0,
								0, 0, 0, 1 );

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	xLookAt = DirectX::XMVector3TransformCoord(xLookAt, xRotationMatrix);
	xUpVec = DirectX::XMVector3TransformCoord(xUpVec, xRotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	xLookAt = DirectX::XMVectorAdd(xLookAt, xPos);

	// Finally create the view matrix from the three updated vectors.
	m_xViewMatrix = DirectX::XMMatrixLookAtLH( xPos, xLookAt, xUpVec);
}