#pragma once

#ifndef OX93_TERRAINSYSTEM_H__
#define OX93_TERRAINSYSTEM_H__

#include "LibNoise/module/perlin.h"

// Includes...
#include <list>
#include "ROOT/Math/Ox93_Math.h"

// Forward Declarations...
class Ox93_PhysicalObject;
class Ox93_Terrain;

class Ox93_TerrainSystem
{
public:
	static bool Create(int iSeed);
	static void Destroy();

	static void Update();

	static int GetCurrentSeed() { return s_pxThis ? s_pxThis->m_iSeed : 0; }

	static bool GetClosestPointInRange(Ox93_Vector_3 xPos, float fRange, Ox93_Vector_3* pxResultPos);
	static bool GetClosestPointToLine(Ox93_Vector_3 xDir, Ox93_Vector_3 xPos, Ox93_Vector_3* pxResult, float fRange, float fNear = 0.f, bool bForcePositive = false);
	static float GetHeightAtPoint(Ox93_Vector_3 xPos);
	static Ox93_Vector_3 GetClosestCentre(Ox93_Vector_3 xPos);

	static bool AddPhysicalObject(Ox93_PhysicalObject* pxObject);

protected:
	Ox93_TerrainSystem();
	~Ox93_TerrainSystem() {};
	bool Init(int iSeed);
	void Shutdown();

private:
	struct Ox93_TerrainType
	{
		int iTileX;
		int iTileZ;
		Ox93_Terrain* pxTerrain;
	};
	std::list<std::list<Ox93_TerrainType*>*> m_lplpxTerrainTiles;
	noise::module::Perlin m_xModule;
	int m_iSeed;
	int m_iTileMinX;
	int m_iTileMaxX;
	int m_iTileMinZ;
	int m_iTileMaxZ;

	static Ox93_TerrainSystem* s_pxThis;
};

#endif // OX93_TERRAINSYSTEM_H__

