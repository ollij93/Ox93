#pragma once

#ifndef OX93_CHARACTER_H__
#define OX93_CHARACTER_H__

// Includes...
#include "Ox93_Core.h"
#include "Game/Entity/PhysicalObject/Ox93_PhysicalObject.h"

// Forward Declarations...
class Ox93_Inventory;

class Ox93_Character : public Ox93_PhysicalObject
{
public:
	Ox93_Character(u_int uClassification);
	~Ox93_Character();

	virtual void Update() override;

	virtual void ReadFromChunkStream(const Ox93_ChunkStream& xChunkStream) override;
	virtual void WriteToChunkStream(Ox93_ChunkStream& xChunkStream) const override;

	virtual void InitFromSpecification(const Ox93_Specification* pxSpecification) override;

	static Ox93_Entity* Create() { return new Ox93_Character(OX93_CLASS_CHARACTER); }
	static Ox93_Entity* CreateLocalPlayer();

	static void HandleInput();

	Ox93_Vector_3 GetEyePosition() const { return GetPosition() + Ox93_Vector_3(0, m_fHeight - m_fRadius, 0); }
	Ox93_Matrix3x3 GetEyeOrientation() const { return m_xEyeOri; }

	static const Ox93_Character* GetLocalPlayer() { return s_pxLocalPlayer; }

private:
	void HandlePlayerInput();

	float m_fRadius;
	float m_fHeight;

	float m_fSprintMultiplier;
	float m_fCrawlMultiplier;

	Ox93_Matrix3x3 m_xEyeOri;
	Ox93_Inventory* m_pxInventory;

	static bool s_bFirstPerson;
	static float s_fThirdPersonZoom;
	static Ox93_Character* s_pxLocalPlayer;
	typedef Ox93_PhysicalObject PARENT;
};

#endif // OX93_CHARACTER_H__