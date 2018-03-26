// Includes...
#include "Ox93_TerrainSystem.h"
#include "Game/Entity/PhysicalObject/Character/Ox93_Character.h"
#include "Game/Entity/PhysicalObject/Ox93_PhysicalObject.h"
#include "Game/World/Terrain/Ox93_Terrain.h"
#include "ROOT/Assert/Ox93_Assert.h"

// Static variables...
Ox93_TerrainSystem* Ox93_TerrainSystem::s_pxThis = nullptr;

Ox93_TerrainSystem::Ox93_TerrainSystem()
: m_lplpxTerrainTiles()
, m_xModule()
, m_iSeed(0)
, m_iTileMinX(0)
, m_iTileMaxX(0)
, m_iTileMinZ(0)
, m_iTileMaxZ(0)
{
}

bool Ox93_TerrainSystem::Create(int iSeed)
{
	Ox93_Assert(!s_pxThis, "Ox93_TerrainSystem is already created.");
	if (s_pxThis) { return false; }

	s_pxThis = new Ox93_TerrainSystem;
	return s_pxThis->Init(iSeed);
}

void Ox93_TerrainSystem::Destroy()
{
	if (!s_pxThis) { return; }

	s_pxThis->Shutdown();
	delete s_pxThis;
	s_pxThis = nullptr;
}

void Ox93_TerrainSystem::Update()
{
	if (!s_pxThis) { return; }

	Ox93_Vector_3 xPos = Ox93_Vector_3(1.f, 1.f, 1.f);
	const Ox93_Character* pxPlayer = Ox93_Character::GetLocalPlayer();
	if (pxPlayer)
	{
		xPos = pxPlayer->GetPosition();
	}

	float fTileX, fTileZ;
	fTileX = xPos.x / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES * Ox93_Math::Sqrt3 / 2.f);
	fTileZ = xPos.z / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES);

	int iTileX, iTileZ;
	iTileX = floor(fTileX);
	iTileZ = floor(fTileZ);

	// If the player is too close to an edge we need to create a new set of tiles
	if (s_pxThis->m_iTileMaxX < iTileX + 2)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainTypes = new std::list<Ox93_TerrainType*>;
		for (int z = s_pxThis->m_iTileMinZ; z <= s_pxThis->m_iTileMaxZ; z++)
		{
			Ox93_TerrainType* pxTerrainType = new Ox93_TerrainType;
			pxTerrainType->pxTerrain = new Ox93_Terrain;

			pxTerrainType->pxTerrain->GenerateRandom(s_pxThis->m_iTileMaxX + 1, z, s_pxThis->m_xModule);

			pxTerrainType->iTileX = s_pxThis->m_iTileMaxX + 1;
			pxTerrainType->iTileZ = z;

			plpxTerrainTypes->push_back(pxTerrainType);
		}

		s_pxThis->m_lplpxTerrainTiles.push_back(plpxTerrainTypes);

		s_pxThis->m_iTileMaxX += 1;
	}
	if (s_pxThis->m_iTileMinX > iTileX - 2)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainTypes = new std::list<Ox93_TerrainType*>;
		for (int z = s_pxThis->m_iTileMinZ; z <= s_pxThis->m_iTileMaxZ; z++)
		{
			Ox93_TerrainType* pxTerrainType = new Ox93_TerrainType;
			pxTerrainType->pxTerrain = new Ox93_Terrain;

			pxTerrainType->pxTerrain->GenerateRandom(s_pxThis->m_iTileMinX - 1, z, s_pxThis->m_xModule);

			pxTerrainType->iTileX = s_pxThis->m_iTileMinX - 1;
			pxTerrainType->iTileZ = z;

			plpxTerrainTypes->push_back(pxTerrainType);
		}

		s_pxThis->m_lplpxTerrainTiles.push_front(plpxTerrainTypes);

		s_pxThis->m_iTileMinX -= 1;
	}
	if (s_pxThis->m_iTileMaxZ < iTileZ + 2)
	{
		std::list<std::list<Ox93_TerrainType*>*>::const_iterator xIter;
		for (xIter = s_pxThis->m_lplpxTerrainTiles.begin(); xIter != s_pxThis->m_lplpxTerrainTiles.end(); ++xIter)
		{
			std::list<Ox93_TerrainType*>* plpxTerrainList = *xIter;
			if (plpxTerrainList)
			{
				Ox93_TerrainType* pxTerrainType = new Ox93_TerrainType;
				pxTerrainType->pxTerrain = new Ox93_Terrain;
				
				pxTerrainType->pxTerrain->GenerateRandom(plpxTerrainList->front()->iTileX, s_pxThis->m_iTileMaxZ + 1, s_pxThis->m_xModule);
			
				pxTerrainType->iTileX = plpxTerrainList->front()->iTileX;
				pxTerrainType->iTileZ = s_pxThis->m_iTileMaxZ + 1;

				plpxTerrainList->push_back(pxTerrainType);
			}
		}

		s_pxThis->m_iTileMaxZ += 1;
	}
	if (s_pxThis->m_iTileMinZ > iTileZ - 2)
	{
		std::list<std::list<Ox93_TerrainType*>*>::const_iterator xIter;
		for (xIter = s_pxThis->m_lplpxTerrainTiles.begin(); xIter != s_pxThis->m_lplpxTerrainTiles.end(); ++xIter)
		{
			std::list<Ox93_TerrainType*>* plpxTerrainList = *xIter;
			if (plpxTerrainList)
			{
				Ox93_TerrainType* pxTerrainType = new Ox93_TerrainType;
				pxTerrainType->pxTerrain = new Ox93_Terrain;

				pxTerrainType->pxTerrain->GenerateRandom(plpxTerrainList->front()->iTileX, s_pxThis->m_iTileMinZ - 1, s_pxThis->m_xModule);

				pxTerrainType->iTileX = plpxTerrainList->front()->iTileX;
				pxTerrainType->iTileZ = s_pxThis->m_iTileMinZ - 1;

				plpxTerrainList->push_front(pxTerrainType);
			}
		}

		s_pxThis->m_iTileMinZ -= 1;
	}
	// If the player is too far from an edge we can delete the tiles along that edge
	if (s_pxThis->m_iTileMaxX > iTileX + 4)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainTypes = s_pxThis->m_lplpxTerrainTiles.back();
		if (plpxTerrainTypes)
		{
			while (plpxTerrainTypes->size() > 0)
			{
				Ox93_TerrainType* pxTerrainType = plpxTerrainTypes->back();
				if (pxTerrainType && pxTerrainType->pxTerrain)
				{
					delete pxTerrainType->pxTerrain;
					pxTerrainType->pxTerrain = nullptr;
				}
				delete pxTerrainType;
				pxTerrainType = nullptr;

				plpxTerrainTypes->pop_back();
			}

			delete plpxTerrainTypes;
			plpxTerrainTypes = nullptr;
		}
		s_pxThis->m_lplpxTerrainTiles.pop_back();

		s_pxThis->m_iTileMaxX -= 1;
	}
	if (s_pxThis->m_iTileMinX < iTileX - 4)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainTypes = s_pxThis->m_lplpxTerrainTiles.front();
		if (plpxTerrainTypes)
		{
			while (plpxTerrainTypes->size() > 0)
			{
				Ox93_TerrainType* pxTerrainType = plpxTerrainTypes->back();
				if (pxTerrainType && pxTerrainType->pxTerrain)
				{
					delete pxTerrainType->pxTerrain;
					pxTerrainType->pxTerrain = nullptr;
				}
				delete pxTerrainType;
				pxTerrainType = nullptr;

				plpxTerrainTypes->pop_back();
			}

			delete plpxTerrainTypes;
			plpxTerrainTypes = nullptr;
		}
		s_pxThis->m_lplpxTerrainTiles.pop_front();

		s_pxThis->m_iTileMinX += 1;
	}
	if (s_pxThis->m_iTileMaxZ > iTileZ + 4)
	{
		std::list<std::list<Ox93_TerrainType*>*>::const_iterator xIter;
		for (xIter = s_pxThis->m_lplpxTerrainTiles.begin(); xIter != s_pxThis->m_lplpxTerrainTiles.end(); ++xIter)
		{
			std::list<Ox93_TerrainType*>* plpxTerrainList = *xIter;
			if (plpxTerrainList)
			{
				Ox93_TerrainType* pxTerrainType = plpxTerrainList->back();
				if (pxTerrainType && pxTerrainType->pxTerrain)
				{
					delete pxTerrainType->pxTerrain;
					pxTerrainType->pxTerrain = nullptr;

					delete pxTerrainType;
					pxTerrainType = nullptr;
				}

				plpxTerrainList->pop_back();
			}
		}

		s_pxThis->m_iTileMaxZ -= 1;
	}
	if (s_pxThis->m_iTileMinZ < iTileZ - 4)
	{
		std::list<std::list<Ox93_TerrainType*>*>::const_iterator xIter;
		for (xIter = s_pxThis->m_lplpxTerrainTiles.begin(); xIter != s_pxThis->m_lplpxTerrainTiles.end(); ++xIter)
		{
			std::list<Ox93_TerrainType*>* plpxTerrainList = *xIter;
			if (plpxTerrainList)
			{
				Ox93_TerrainType* pxTerrainType = plpxTerrainList->front();
				if (pxTerrainType && pxTerrainType->pxTerrain)
				{
					delete pxTerrainType->pxTerrain;
					pxTerrainType->pxTerrain = nullptr;

					delete pxTerrainType;
					pxTerrainType = nullptr;
				}

				plpxTerrainList->pop_front();
			}
		}

		s_pxThis->m_iTileMinZ += 1;
	}
}

bool Ox93_TerrainSystem::Init(int iSeed)
{
	m_iSeed = iSeed;

	m_xModule.SetSeed(iSeed);

	int iTileXMax = 2;
	int iTileXMin = -2;
	int iTileZMax = 2;
	int iTileZMin = -2;

	for (int x = iTileXMin; x <= iTileXMax; x++)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainList = new std::list<Ox93_TerrainType*>;

		for (int z = iTileZMin; z <= iTileZMax; z++)
		{
			Ox93_TerrainType* pxTerrainType = new Ox93_TerrainType;
			pxTerrainType->pxTerrain = new Ox93_Terrain;

			pxTerrainType->pxTerrain->GenerateRandom(x, z, m_xModule);

			pxTerrainType->iTileX = x;
			pxTerrainType->iTileZ = z;

			plpxTerrainList->push_back(pxTerrainType);
		}

		m_lplpxTerrainTiles.push_back(plpxTerrainList);
	}

	m_iTileMaxX = iTileXMax;
	m_iTileMinX = iTileXMin;
	m_iTileMaxZ = iTileZMax;
	m_iTileMinZ = iTileZMin;
	
	return true;
}

void Ox93_TerrainSystem::Shutdown()
{
	while (m_lplpxTerrainTiles.size() > 0)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainTypes = m_lplpxTerrainTiles.back();
		while (plpxTerrainTypes && plpxTerrainTypes->size() > 0)
		{
			Ox93_TerrainType* pxTerrainType = plpxTerrainTypes->back();
			if (pxTerrainType && pxTerrainType->pxTerrain)
			{
				delete pxTerrainType->pxTerrain;
				pxTerrainType->pxTerrain = nullptr;
			}
			delete pxTerrainType;
			pxTerrainType = nullptr;

			plpxTerrainTypes->pop_back();
		}
		plpxTerrainTypes->clear();
		delete plpxTerrainTypes;
		plpxTerrainTypes = nullptr;

		m_lplpxTerrainTiles.pop_back();
	}
}

bool Ox93_TerrainSystem::GetClosestPointInRange(Ox93_Vector_3 xPos, float fRange, Ox93_Vector_3* pxResultPos)
{
	if (!s_pxThis) { return false; }

	float fTileX, fTileZ;
	fTileX = xPos.x / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES * Ox93_Math::Sqrt3 / 2.f);
	fTileZ = xPos.z / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES);

	int iTileX, iTileZ;
	iTileX = floor(fTileX);
	iTileZ = floor(fTileZ);

	Ox93_Vector_3 xPositions[9] = { Ox93_Math::ZeroVector3 };
	bool bValidRange[9] = { false };
	
	std::list<std::list<Ox93_TerrainType*>*>::const_iterator xIterX;
	for (xIterX = s_pxThis->m_lplpxTerrainTiles.begin(); xIterX != s_pxThis->m_lplpxTerrainTiles.end(); ++xIterX)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainTypes = *xIterX;
		if (!plpxTerrainTypes) { continue; }

		std::list<Ox93_TerrainType*>::const_iterator xIterZ;
		for (xIterZ = plpxTerrainTypes->begin(); xIterZ != plpxTerrainTypes->end(); ++xIterZ)
		{
			Ox93_TerrainType* pxTerrainType = *xIterZ;
			if (!pxTerrainType || !pxTerrainType->pxTerrain) { continue; }

			if (pxTerrainType->iTileX <= iTileX + 1
				&& pxTerrainType->iTileX >= iTileX - 1
				&& pxTerrainType->iTileZ <= iTileZ + 1
				&& pxTerrainType->iTileZ >= iTileZ - 1)
			{
				int iIndex = 3 * (pxTerrainType->iTileX - iTileX + 1) + (pxTerrainType->iTileZ - iTileZ + 1);

				bValidRange[iIndex] = pxTerrainType->pxTerrain->GetClosestPointInRange(xPos, fRange, &xPositions[iIndex]);
			}
		}
	}

	float fDist = fRange + 1.f;
	for (int i = 0; i < 9; i++)
	{
		if (bValidRange[i] && (xPositions[i] - xPos).Length() < fDist)
		{
			fDist = (xPositions[i] - xPos).Length();
			*pxResultPos = xPositions[i];
		}
	}

	return (fDist < fRange);
}

bool Ox93_TerrainSystem::GetClosestPointToLine(Ox93_Vector_3 xDir, Ox93_Vector_3 xPos, Ox93_Vector_3* pxResult, float fRange, float fNear /* = 0.f */, bool bForcePositive /* = false */)
{
	if (!s_pxThis) { return false; }

	float fTileX, fTileZ;
	fTileX = xPos.x / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES * Ox93_Math::Sqrt3 / 2.f);
	fTileZ = xPos.z / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES);

	int iTileX, iTileZ;
	iTileX = floor(fTileX);
	iTileZ = floor(fTileZ);

	Ox93_Vector_3 xPositions[9] = { Ox93_Math::ZeroVector3 };
	float fDistances[9] = { fRange + 1.f };
	bool bValidRange[9] = { false };

	std::list<std::list<Ox93_TerrainType*>*>::const_iterator xIterX;
	for (xIterX = s_pxThis->m_lplpxTerrainTiles.begin(); xIterX != s_pxThis->m_lplpxTerrainTiles.end(); ++xIterX)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainTypes = *xIterX;
		if (!plpxTerrainTypes) { continue; }

		std::list<Ox93_TerrainType*>::const_iterator xIterZ;
		for (xIterZ = plpxTerrainTypes->begin(); xIterZ != plpxTerrainTypes->end(); ++xIterZ)
		{
			Ox93_TerrainType* pxTerrainType = *xIterZ;
			if (!pxTerrainType || !pxTerrainType->pxTerrain) { continue; }

			if (pxTerrainType->iTileX <= iTileX + 1
				&& pxTerrainType->iTileX >= iTileX - 1
				&& pxTerrainType->iTileZ <= iTileZ + 1
				&& pxTerrainType->iTileZ >= iTileZ - 1)
			{
				int iIndex = 3 * (pxTerrainType->iTileX - iTileX + 1) + (pxTerrainType->iTileZ - iTileZ + 1);

				bValidRange[iIndex] = pxTerrainType->pxTerrain->GetClosestPointToLine(xDir, xPos, &xPositions[iIndex], &fDistances[iIndex], fRange, fNear, bForcePositive);
			}
		}
	}

	float fDist = fRange + 1.f;
	for (int i = 0; i < 9; i++)
	{
		if (bValidRange[i] && fDistances[i] < fDist)
		{
			fDist = fDistances[i];
			*pxResult = xPositions[i];
		}
	}

	if (fDist < fRange)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float Ox93_TerrainSystem::GetHeightAtPoint(Ox93_Vector_3 xPos)
{
	// Adjust position to remove zagged edge effect
	const float fFrac = Ox93_Math::Sqrt3 / 2.f;
	int iX = floor((xPos.x) / (fOX93_TERRAIN_SCALE * fFrac));
	const float fX = static_cast<float>(iX) * fFrac * fOX93_TERRAIN_SCALE;

	float fAdjustedPosZ = xPos.z - abs(xPos.x - (iX + abs(iX % 2)) * fFrac * fOX93_TERRAIN_SCALE) / Ox93_Math::Sqrt3;

	float fTileX, fTileZ;
	fTileX = xPos.x / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES * Ox93_Math::Sqrt3 / 2.f);
	fTileZ = fAdjustedPosZ / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES);

	int iTileX, iTileZ;
	iTileX = floor(fTileX);
	iTileZ = floor(fTileZ);

	std::list<std::list<Ox93_TerrainType*>*>::const_iterator xIterX;
	for (xIterX = s_pxThis->m_lplpxTerrainTiles.begin(); xIterX != s_pxThis->m_lplpxTerrainTiles.end(); ++xIterX)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainTypes = *xIterX;
		if (!plpxTerrainTypes) { continue; }

		std::list<Ox93_TerrainType*>::const_iterator xIterZ;
		for (xIterZ = plpxTerrainTypes->begin(); xIterZ != plpxTerrainTypes->end(); ++xIterZ)
		{
			Ox93_TerrainType* pxTerrainType = *xIterZ;
			if (!pxTerrainType || !pxTerrainType->pxTerrain) { continue; }

			if (pxTerrainType->iTileX == iTileX
				&& pxTerrainType->iTileZ == iTileZ)
			{
				return pxTerrainType->pxTerrain->GetHeightAtPoint(xPos);
			}
		}
	}

	return 0.f;
}

Ox93_Vector_3 Ox93_TerrainSystem::GetClosestCentre(Ox93_Vector_3 xPos)
{
	if (!s_pxThis) { return Ox93_Math::ZeroVector3; }

	// Adjust position to remove zagged edge effect
	const float fFrac = Ox93_Math::Sqrt3 / 2.f;
	int iX = floor((xPos.x) / (fOX93_TERRAIN_SCALE * fFrac));
	const float fX = static_cast<float>(iX) * fFrac * fOX93_TERRAIN_SCALE;

	float fAdjustedPosZ = xPos.z - abs(xPos.x - (iX + abs(iX % 2)) * fFrac * fOX93_TERRAIN_SCALE) / Ox93_Math::Sqrt3;

	float fTileX, fTileZ;
	fTileX = xPos.x / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES * Ox93_Math::Sqrt3 / 2.f);
	fTileZ = fAdjustedPosZ / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES);

	int iTileX, iTileZ;
	iTileX = floor(fTileX);
	iTileZ = floor(fTileZ);

	std::list<std::list<Ox93_TerrainType*>*>::const_iterator xIterX;
	for (xIterX = s_pxThis->m_lplpxTerrainTiles.begin(); xIterX != s_pxThis->m_lplpxTerrainTiles.end(); ++xIterX)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainTypes = *xIterX;
		if (!plpxTerrainTypes) { continue; }

		std::list<Ox93_TerrainType*>::const_iterator xIterZ;
		for (xIterZ = plpxTerrainTypes->begin(); xIterZ != plpxTerrainTypes->end(); ++xIterZ)
		{
			Ox93_TerrainType* pxTerrainType = *xIterZ;
			if (!pxTerrainType || !pxTerrainType->pxTerrain) { continue; }

			if (pxTerrainType->iTileX == iTileX
				&& pxTerrainType->iTileZ == iTileZ)
			{
				return pxTerrainType->pxTerrain->GetClosestCentre(xPos);
			}
		}
	}

	return Ox93_Math::ZeroVector3;
}

bool Ox93_TerrainSystem::AddPhysicalObject(Ox93_PhysicalObject* pxObject)
{
	if (!pxObject) { return false; }

	Ox93_Vector_3 xPos = pxObject->GetPosition();

	// Adjust position to remove zagged edge effect
	const float fFrac = Ox93_Math::Sqrt3 / 2.f;
	int iX = floor((xPos.x) / (fOX93_TERRAIN_SCALE * fFrac));
	const float fX = static_cast<float>(iX) * fFrac * fOX93_TERRAIN_SCALE;

	float fAdjustedPosZ = xPos.z - abs(xPos.x - (iX + abs(iX % 2)) * fFrac * fOX93_TERRAIN_SCALE) / Ox93_Math::Sqrt3;

	float fTileX, fTileZ;
	fTileX = xPos.x / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES * Ox93_Math::Sqrt3 / 2.f);
	fTileZ = fAdjustedPosZ / (fOX93_TERRAIN_SCALE * (float)uOX93_TERRAIN_NUMSIDEFACES);

	int iTileX, iTileZ;
	iTileX = floor(fTileX);
	iTileZ = floor(fTileZ);

	std::list<std::list<Ox93_TerrainType*>*>::const_iterator xIterX;
	for (xIterX = s_pxThis->m_lplpxTerrainTiles.begin(); xIterX != s_pxThis->m_lplpxTerrainTiles.end(); ++xIterX)
	{
		std::list<Ox93_TerrainType*>* plpxTerrainTypes = *xIterX;
		if (!plpxTerrainTypes) { continue; }

		std::list<Ox93_TerrainType*>::const_iterator xIterZ;
		for (xIterZ = plpxTerrainTypes->begin(); xIterZ != plpxTerrainTypes->end(); ++xIterZ)
		{
			Ox93_TerrainType* pxTerrainType = *xIterZ;
			if (!pxTerrainType || !pxTerrainType->pxTerrain) { continue; }

			if (pxTerrainType->iTileX == iTileX
				&& pxTerrainType->iTileZ == iTileZ)
			{
				return pxTerrainType->pxTerrain->AddPhysicalObject(pxObject);
			}
		}
	}

	return false;
}