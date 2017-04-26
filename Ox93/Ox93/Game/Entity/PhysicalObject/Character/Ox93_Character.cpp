// Includes...
#include "Ox93_Character.h"
#include "Game/Entity/Camera/Ox93_Camera.h"
#include "Game/Inventory/Ox93_Inventory.h"
#include "Game/LoadSave/ChunkStream/Ox93_ChunkStream.h"
#include "Game/LoadSave/Ox93_LoadSystem.h"
#include "Game/LoadSave/Ox93_SaveSystem.h"
#include "Game/Menu/Ox93_MenuSystem.h"
#include "Game/World/Terrain/Ox93_TerrainSystem.h"
#include "ROOT/Graphics/Ox93_GraphicsSystem.h"
#include "ROOT/Ox93_InputSystem.h"
#include "ROOT/Specification/Ox93_Specification.h"
#include "ROOT/Specification/Ox93_Specification_System.h"

// Global Variables...
static const float fOX93_GRAVITATIONAL_STRENGTH = 15.f;
static const float fOX93_MOUSE_SENSITIVITY = 0.01f;
static const float fOX93_MOVEMENT_SPEED = 0.15f;
static const float fOX93_JUMP_SPEED = 5.5f;
static const float fOX93_JUMP_DISTANCE = 0.3f;

// Statics...
bool Ox93_Character::s_bFirstPerson = true;
float Ox93_Character::s_fThirdPersonZoom = 3.f;
Ox93_Character* Ox93_Character::s_pxLocalPlayer = nullptr;

Ox93_Character::Ox93_Character(u_int uClassification)
: PARENT(uClassification)
, m_fRadius(0.7f)
, m_fHeight(2.f)
, m_xEyeOri(Ox93_Math::IdentityMatrix3x3)
, m_pxInventory(nullptr)
{
}

Ox93_Character::~Ox93_Character()
{
}

void Ox93_Character::ReadFromChunkStream(const Ox93_ChunkStream& xChunkStream)
{
	PARENT::ReadFromChunkStream(xChunkStream);

	xChunkStream >> m_xEyeOri.e00;
	xChunkStream >> m_xEyeOri.e01;
	xChunkStream >> m_xEyeOri.e02;
	xChunkStream >> m_xEyeOri.e10;
	xChunkStream >> m_xEyeOri.e11;
	xChunkStream >> m_xEyeOri.e12;
	xChunkStream >> m_xEyeOri.e20;
	xChunkStream >> m_xEyeOri.e21;
	xChunkStream >> m_xEyeOri.e22;

	bool bLocalPlayer;
	xChunkStream >> bLocalPlayer;

	if (bLocalPlayer)
	{
		if (!s_pxLocalPlayer)
		{
			s_pxLocalPlayer = this;
			m_pxInventory = new Ox93_Inventory;
			Ox93_Inventory::SetPlayerInventory(m_pxInventory);
			Ox93_LoadSystem::LoadInventoryFromFile(m_pxInventory, L"Saves/Olli.inv");
		}
		else
		{
			Ox93_Assert(false, "More than one character marked as local player the oldest will be used. This should not be happening!");
		}
	}
}

void Ox93_Character::WriteToChunkStream(Ox93_ChunkStream& xChunkStream) const
{
	PARENT::WriteToChunkStream(xChunkStream);
	
	xChunkStream << m_xEyeOri.e00;
	xChunkStream << m_xEyeOri.e01;
	xChunkStream << m_xEyeOri.e02;
	xChunkStream << m_xEyeOri.e10;
	xChunkStream << m_xEyeOri.e11;
	xChunkStream << m_xEyeOri.e12;
	xChunkStream << m_xEyeOri.e20;
	xChunkStream << m_xEyeOri.e21;
	xChunkStream << m_xEyeOri.e22;

	xChunkStream << (s_pxLocalPlayer == this);

	// TODO-OJ : This probably shouldn't go here?
	if (s_pxLocalPlayer == this && m_pxInventory)
	{
		Ox93_SaveSystem::SaveInventoryToFile(m_pxInventory, L"Saves/Olli.inv");
	}
}

Ox93_Entity* Ox93_Character::CreateLocalPlayer()
{
	if (!s_pxLocalPlayer)
	{
		s_pxLocalPlayer = new Ox93_Character(OX93_CLASS_CHARACTER);
		s_pxLocalPlayer->SetPosition(1.f, 10.f, 4.f);
		const Ox93_Specification* pxSpec = Ox93_Specification_System::GetSpecificationByHash(uOX93_SPEC_NAME_PLAYER);
		if (pxSpec)
		{
			s_pxLocalPlayer->InitFromSpecification(pxSpec);
		}
		s_pxLocalPlayer->m_pxInventory = new Ox93_Inventory;
		Ox93_Inventory::SetPlayerInventory(s_pxLocalPlayer->m_pxInventory);
		return s_pxLocalPlayer;
	}
	return nullptr;
}

void Ox93_Character::InitFromSpecification(const Ox93_Specification* pxSpecification)
{
	if (pxSpecification)
	{
		PARENT::InitFromSpecification(pxSpecification);

		m_fRadius = pxSpecification->GetValueAsFloat(uOX93_SPEC_PARAM_RADIUS, 0.f);
		m_fHeight = pxSpecification->GetValueAsFloat(uOX93_SPEC_PARAM_HEIGHT, 0.f);
	}
}

void Ox93_Character::Update()
{
	PARENT::Update();

	m_xVelocity.y -= fOX93_GRAVITATIONAL_STRENGTH * Ox93_Entity::GetRunningUpdateTime() / 1000.f;

	Ox93_Vector_3 xTerrainIntersect;
	Ox93_Vector_3 xSpherePos = GetPosition() + Ox93_Vector_3(0.f, m_fRadius, 0.f);
	bool bIntersecting = Ox93_TerrainSystem::GetClosestPointInRange(xSpherePos, m_fRadius, &xTerrainIntersect);

	if (bIntersecting)
	{
		// Move the Character vertically enough to just be touching the terrain (counteracts gravity dragging through the surface)
		Ox93_Vector_3 xAdjustPos = (xSpherePos - xTerrainIntersect);
		float fAdjustDist = m_fRadius - xAdjustPos.Length();
		xAdjustPos.Normalize();
		xAdjustPos *= fAdjustDist;
		SetPosition(GetPosition() + xAdjustPos);

		// Also set vertical momentum to zero as the character is now stood on the slope
		m_xVelocity.y = 0.f;
	}

	Ox93_Camera* pxCamera = Ox93_Camera::GetActive();
	if (pxCamera)
	{
		Ox93_Vector_3 xPosAdjust;
		if (s_bFirstPerson)
		{
			xPosAdjust = Ox93_Vector_3(0.f, m_fHeight - m_fRadius, 0.f);
		}
		else
		{
			xPosAdjust = Ox93_Vector_3(-m_xEyeOri.e20, -m_xEyeOri.e21, -m_xEyeOri.e22);
			xPosAdjust.Normalize();
			xPosAdjust *= s_fThirdPersonZoom;
			xPosAdjust += Ox93_Vector_3(0.f, m_fHeight - m_fRadius, 0.f);
		}
		pxCamera->SetPosition(GetPosition() + xPosAdjust);
		pxCamera->SetOrientation(m_xEyeOri);
	}

	if (m_pxInventory)
	{
		m_pxInventory->UpdateCurrentItem();
	}
}

void Ox93_Character::HandleInput()
{
	if (s_pxLocalPlayer)
	{
		s_pxLocalPlayer->HandlePlayerInput();
	}
}

void Ox93_Character::HandlePlayerInput()
{
	// Check for mouse movement
	if (Ox93_InputSystem::GetMouseMoved())
	{
		const Ox93_Vector_2 xMouseDisp = Ox93_InputSystem::GetMouseDisplacement();
		float fDeltaPhi = -xMouseDisp.x * fOX93_MOUSE_SENSITIVITY;
		float fDeltaTheta = -xMouseDisp.y * fOX93_MOUSE_SENSITIVITY;

		m_xEyeOri.RotateLocalX(fDeltaTheta);
		// Check we're not over rotating:
		if (m_xEyeOri.e11 < 0.f)
		{
			if (m_xEyeOri.e21 < 0.f)
			{
				// Looking down
				m_xEyeOri.RotateLocalX(-asin(m_xEyeOri.e11));
			}
			else
			{
				// Looking up
				m_xEyeOri.RotateLocalX(asin(m_xEyeOri.e11));

			}
		}

		m_xEyeOri.RotateWorldY(fDeltaPhi);
		m_xOrientation.RotateWorldY(fDeltaPhi);

		Ox93_InputSystem::SetCursorPosition(Ox93_InputSystem::GetMouseClickLocation());
	}

	// Handle Mouse Click
	if (Ox93_InputSystem::GetRMBClicked())
	{
		// If we have a selection object get it to create its object
		if (m_pxInventory)
		{
			m_pxInventory->ActivateCurrentItem();
		}
	}

	// Handle the keyboard controls
	if (!Ox93_InputSystem::IsKeyDown(CTRL_KEY)) { m_fSprintMultiplier = 1.0; }
	if (!Ox93_InputSystem::IsKeyDown(SHIFT_KEY)) { m_fCrawlMultiplier = 1.0; }
	
	for (int iKey = 0; iKey < 256; iKey++)
	{
		if (!Ox93_InputSystem::IsKeyDown(iKey)) { continue; }

		// Process the action for each keyboard key being held
		switch (iKey)
		{
			case TAB_KEY:
			{
				// Switch back to no item
				Ox93_Inventory::PlayerSelectItem(-1);
				break;
			}
			case CTRL_KEY:
			{
				m_fSprintMultiplier = 2.0;
				break;
			}
			case SHIFT_KEY:
			{
				m_fCrawlMultiplier = 0.5;
				break;
			}
			case W_KEY:
			{
				// Move forward
				Ox93_Vector_3 xDir(m_xOrientation.e20, 0, m_xOrientation.e22);
				xDir.Normalize();

				m_xPosition += xDir * fOX93_MOVEMENT_SPEED * m_fSprintMultiplier * m_fCrawlMultiplier;
				break;
			}
			case A_KEY:
			{
				// Move left
				Ox93_Vector_3 xDir(m_xOrientation.e22, 0, -m_xOrientation.e20);
				xDir.Normalize();

				m_xPosition -= xDir * fOX93_MOVEMENT_SPEED * m_fCrawlMultiplier;
				break;
			}
			case S_KEY:
			{
				// Move backward
				Ox93_Vector_3 xDir(m_xOrientation.e20, 0, m_xOrientation.e22);
				xDir.Normalize();

				m_xPosition -= xDir * fOX93_MOVEMENT_SPEED * m_fCrawlMultiplier;
				break;
			}
			case D_KEY:
			{
				// Move right
				Ox93_Vector_3 xDir(m_xOrientation.e22, 0, -m_xOrientation.e20);
				xDir.Normalize();

				m_xPosition += xDir * fOX93_MOVEMENT_SPEED * m_fCrawlMultiplier;
				break;
			}
			case E_KEY:
			{
				// Open Inventory
				Ox93_System::Pause();
				Ox93_MenuSystem::OpenInventoryMenu();
				break;
			}
			case SPACE_KEY:
			{
				// Jump
				Ox93_Vector_3 xUpJunk = Ox93_Vector_3(0.f, m_fRadius, 0.f);
				if (Ox93_InputSystem::KeyJustPressed(SPACE_KEY) && Ox93_TerrainSystem::GetClosestPointInRange(GetPosition() + xUpJunk, m_fRadius + fOX93_JUMP_DISTANCE, &xUpJunk))
				{
					m_xVelocity.y = fOX93_JUMP_SPEED;
				}
				break;
			}
			case F3_KEY:
			{
				if (Ox93_InputSystem::KeyJustPressed(F3_KEY))
				{
					// Toggle 1st/3rd Person
					s_bFirstPerson = !s_bFirstPerson;
				}
				break;
			}
			case ESC_KEY:
			{
				if (Ox93_InputSystem::KeyJustPressed(ESC_KEY))
				{
					Ox93_System::Pause();
					Ox93_MenuSystem::OpenPauseMenu();
				}
				break;
			}
			default:
				break;
		}
	}
}