#pragma once

#ifndef __OX93_TERRAIN_H__
#define __OX93_TERRAIN_H__

// Includes...
#include "Ox93_Core.h"
#include "Game/Entity/Ox93_Entity_Renderable.h"
#include "LibNoise/noise.h"

// Forwards Declarations...
class Ox93_PhysicalObject;

// Globals...
static const float fOX93_TERRAIN_SCALE = 3.f;
static const u_int uOX93_TERRAIN_NUMSIDEFACES = 64;

struct Ox93_TerrainVertex
{
	Ox93_Vector_3 m_xPosition;
	Ox93_Color m_xColor;
	Ox93_Vector_3 m_xNormal;
};

class Ox93_Terrain
{
public:
	Ox93_Terrain();
	~Ox93_Terrain();

	void Render();

	bool GenerateRandom(int iTileX, int iTileZ, noise::module::Perlin xModule);

	u_int GetIndexCount() const { return 6 * uOX93_TERRAIN_NUMSIDEFACES * uOX93_TERRAIN_NUMSIDEFACES; }
	Ox93_Vector_3 GetPosition() const { return m_xPosition; }

	bool GetClosestPointInRange(Ox93_Vector_3 xPos, float fRange, Ox93_Vector_3* pxResultPos) const;
	bool GetClosestPointToLine(Ox93_Vector_3 xDir, Ox93_Vector_3 xPos, Ox93_Vector_3* pxResult, float* pfDist, float fRange, float fNear = 0.f, bool bForcePositive = false) const;
	float GetHeightAtPoint(Ox93_Vector_3 xPos);
	Ox93_Vector_3 GetClosestCentre(Ox93_Vector_3 xPos);

	bool AddPhysicalObject(Ox93_PhysicalObject* pxObject);

private:
	bool InitBuffers(ID3D11Device* pxDevice, int iTileX, int iTileZ, noise::module::Perlin xModule);

	float m_afHeightMap[uOX93_TERRAIN_NUMSIDEFACES + 1][uOX93_TERRAIN_NUMSIDEFACES + 1];
	Ox93_PhysicalObject* m_apxPhysicalObjects[uOX93_TERRAIN_NUMSIDEFACES][uOX93_TERRAIN_NUMSIDEFACES][2];

	Ox93_Vector_3 m_xPosition;
	Ox93_Matrix3x3 m_xOrientation;

	ID3D11Buffer* m_pxVertexBuffer;
	ID3D11Buffer* m_pxIndexBuffer;

	static const u_int uVERSION = 0;
};

#endif // ifndef __OX93_TERRAIN_H__